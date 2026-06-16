# Modbus RTU - Поток обработки данных

> **Внимание**: формат — кастомный, не стандартный Modbus. Один "register address" в запросе соответствует одному **4-байтному параметру**. Поле ByteCount — 1-байтное (как в стандартном Modbus), но данные — 4 байта на параметр.

## Формат кадров

### FC 0x03 / 0x04 — Read Holding / Read Input Registers

```
Запрос  (8 байт):  [Addr][FC][StartParam_H][StartParam_L][Qty_H][Qty_L][CRC_L][CRC_H]
Ответ  (5+4N байт):[Addr][FC][BC][P0_B3][P0_B2][P0_B1][P0_B0]...[CRC_L][CRC_H]
```

- `StartParam`: индекс первого параметра (16-bit BE)
- `Qty`: количество параметров (1..60)
- `BC` = `Qty * 4` (1 байт)
- Каждый параметр — 4 байта big-endian

**Пример** (чтение 1 параметра телеметрии, индекс 0):
```
Запрос:  [01][04][00][00][00][01][CRC_L][CRC_H]                          = 8 байт
Ответ:   [01][04][04][P_B3][P_B2][P_B1][P_B0][CRC_L][CRC_H]              = 9 байт
```

### FC 0x06 — Write Single Register / Parameter

```
Запрос (10 байт):  [Addr][06][ParamIdx_H][ParamIdx_L][V_B3][V_B2][V_B1][V_B0][CRC_L][CRC_H]
Ответ  (10 байт):  echo
```

- Значение — 4 байта big-endian (полный 32-битный параметр)

### FC 0x10 — Write Multiple Registers / Parameters

```
Запрос (10+4N байт): [Addr][10][StartParam_H][StartParam_L][Qty_H][Qty_L][BC][Data 4*N][CRC_L][CRC_H]
Ответ  (8 байт):     [Addr][10][StartParam_H][StartParam_L][Qty_H][Qty_L][CRC_L][CRC_H]
```

- `BC` = `Qty * 4` (1 байт)
- Данные: N параметров подряд, каждый 4 байта big-endian
- `Qty`: 1..60

## Карта адресов параметров

```
Holding registers (FC 0x03 / 0x06 / 0x10):
  [0 .. _IPCount-1]                          -> INT params
  [_IPCount .. _IPCount + _FPCount - 1]      -> FLOAT params

Input registers (FC 0x04):
  [0 .. _TelPCount-1]                        -> TELEMETRY params
```

Каждый адрес = один параметр (4 байта), без word-offset.

## Поток обработки

### 1. Приём байтов из UART
**`ModbusRtuRoutine.c`** -> `ModbusRtuRoutine()`

State machine с захватом таймстампов байтов в IRQ (Timer12, 1us):
- **RtuIdle**: первый байт -> переход в `RtuReceive`
- **RtuReceive**: накопление в `BuffRtu[uartNum][]`, проверка inter-char gap (t1.5)
- **Тишина t3.5**: конец кадра -> передача в `ModbusRtuFrame_Process()`

### 2. Обработка RTU-фрейма
**`ModbusRtuFrame.c`** -> `ModbusRtuFrame_Process()`

1. Проверка CRC16
2. Проверка адреса (broadcast = 0)
3. Вызов диспетчера -> `ModbusCommandProcess(NumUART, pdu, &pduSize)`
4. Формирование ответа: `[Addr][ResponsePDU][CRC]` или exception `[Addr][FC|0x80][Code][CRC]`

### 3. Диспетчер команд
**`CommandParcerModbus.c`** -> `ModbusCommandProcess()`

Проверяет минимальный размер PDU и маршрутизирует:

| FC     | Min PDU | Функция                    | Файл                        |
|--------|---------|----------------------------|-----------------------------|
| `0x03` | 5       | `ReadHoldingsRegisters()`  | `ReadHoldingRegisters.c`    |
| `0x04` | 5       | `ReadInputRegisters()`     | `ReadInputRegisters.c`      |
| `0x06` | 7       | `WriteSingleRegister()`    | `WriteSingleRegister.c`     |
| `0x10` | 10      | `WriteMultipleRegisters()` | `WriteMultipleRegisters.c`  |
| другой | —       | -> `_IllegalFunction`      |                             |

### 4. Маппинг адресов в параметры
**`ModbusUtils.c`** -> `GetHoldingRegisterMapping()` / `GetInputRegisterMapping()`

Возвращает тип (`PARAM_INT` / `PARAM_FLOAT` / `PARAM_TELEMETRY`) и индекс параметра.

### 5. Доступ к данным
**`../Unicorn2/AccessIntParam.c`** -> `AccessIntParam(IntParam_t, int32_t *, _PAM_RO/_PAM_WO)`
**`../Unicorn2/AccessFloatParam.c`** -> `AccessFloatParam(FloatParam_t, float *, _PAM_RO/_PAM_WO)`
**`../Unicorn2/AccessTelemParam.c`** -> `AccessTelemParam(TelimParam_t, float *)`

Все типы — 4-байтные.

### 6. Конвертация ошибок
**`ErrorHandler.c`** -> `ConvertUnicornErrorIntoModbusError()`

Unicorn-коды -> стандартные Modbus exception codes.

### 7. Отправка ответа
**`ModbusRtuRoutine.c`** -> состояние `RtuTransmit`

Ответ из `BuffRtu[]` порционно проталкивается в `RS485_PushTxFIFOBuf()`.

## Лимиты

- Max quantity для Read/Write: **60 параметров** (вписывается в буфер 256 байт)
- Max байтов данных: 240 (60 * 4)
- Max размер фрейма Read response: `1 + 1 + 1 + 240 + 2 = 245` байт
- Max размер фрейма Write Multiple request: `1 + 1 + 2 + 2 + 1 + 240 + 2 = 249` байт

## Схема потока

```
RS485 UART RX
    |
    v
ModbusRtuRoutine.c          <- побайтный приём + t3.5/t1.5 таймауты
    |
    v
ModbusRtuFrame.c            <- CRC16, проверка адреса, извлечение PDU
    |
    v
CommandParcerModbus.c       <- проверка min PDU + switch по FC
    |
    |---> ReadHoldingRegisters.c  --> ModbusUtils.c --> AccessIntParam / AccessFloatParam
    |---> ReadInputRegisters.c    --> ModbusUtils.c --> AccessTelemParam
    |---> WriteSingleRegister.c   --> ModbusUtils.c --> AccessIntParam / AccessFloatParam
    +---> WriteMultipleRegisters.c -> ModbusUtils.c --> AccessIntParam / AccessFloatParam
    |
    v                              ErrorHandler.c <- Unicorn -> Modbus exception
ModbusRtuFrame.c            <- формирование ответа + CRC
    |
    v
ModbusRtuRoutine.c          <- TX FIFO
    |
    v
RS485 UART TX
```

## Переключение протокола

Параметр `_IP_ProtocolMode` (Int param, сохраняется в `gParamApp` -> flash):
- `0` = Unicorn (по умолчанию)
- `1` = Modbus RTU

Переключение вступает в силу после перезагрузки устройства.
