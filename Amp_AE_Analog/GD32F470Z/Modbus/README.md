# Modbus RTU - Поток обработки данных

## Приём -> Обработка -> Ответ

### 1. Приём байтов из UART
**`ModbusRtuRoutine.c`** -> `ModbusRtuRoutine()`

Вызывается в главном цикле `main_application.c`. Работает как state machine:

- **RtuIdle**: ждём первый байт. Как только пришёл -- переходим в `RtuReceive`, запускаем таймер тишины (5мс)
- **RtuReceive**: складываем байты в `BuffRtu[uartNum][]`, на каждом байте сбрасываем таймер
- **Таймаут тишины** (3.5 символа): фрейм завершён -- передаём в обработку

### 2. Обработка RTU-фрейма
**`ModbusRtuFrame.c`** -> `ModbusRtuFrame_Process()`

Получает сырой фрейм: `[Addr(1)][PDU(N)][CRC_Lo(1)][CRC_Hi(1)]`

1. Проверка CRC16 (используется `../Unicorn2/crc16.c :: CRC16()`)
2. Проверка адреса (сравнение с deviceAddress, broadcast = 0)
3. Извлечение PDU -- `Buff[1..N]` (без адреса и CRC)
4. Вызов диспетчера -> `ModbusCommandProcess(NumUART, pdu, &pduSize)`
5. Формирование ответа:
   - Успех: `[Addr][ResponsePDU][CRC]`
   - Ошибка: `[Addr][FC|0x80][ExceptionCode][CRC]`
   - Broadcast: ответ не отправляется

### 3. Диспетчер команд
**`CommandParcerModbus.c`** -> `ModbusCommandProcess()`

Смотрит `Buff[0]` (function code) и маршрутизирует:

| FC     | Функция                    | Файл                        |
|--------|----------------------------|-----------------------------|
| `0x03` | `ReadHoldingsRegisters()`  | `ReadHoldingRegisters.c`    |
| `0x04` | `ReadInputRegisters()`     | `ReadInputRegisters.c`      |
| `0x06` | `WriteSingleRegister()`    | `WriteSingleRegister.c`     |
| `0x10` | `WriteMultipleRegisters()` | `WriteMultipleRegisters.c`  |
| другой | -> `_IllegalFunction`      |                             |

### 4. Маппинг регистров -> параметры
**`ModbusUtils.c`** -> `GetHoldingRegisterMapping()` / `GetInputRegisterMapping()`

Каждый обработчик вызывает маппер, чтобы определить:
- **Тип параметра**: `PARAM_INT`, `PARAM_FLOAT` или `PARAM_TELEMETRY`
- **Индекс параметра**: номер в enum из `CommandList.h`
- **Word offset**: 0 = старшие 16 бит, 1 = младшие 16 бит

```
Holding Registers:
  [0..67]    -> Int params    (34 x 2 рег)   -> GetHoldingRegisterMapping()
  [68..173]  -> Float params  (53 x 2 рег)   -> GetHoldingRegisterMapping()

Input Registers:
  [0..165]   -> Telemetry     (83 x 2 рег)   -> GetInputRegisterMapping()
```

### 5. Доступ к данным
**`../Unicorn2/AccessIntParam.c`** -> `AccessIntParam()`
**`../Unicorn2/AccessFloatParam.c`** -> `AccessFloatParam()`
**`../Unicorn2/AccessTelemParam.c`** -> `AccessTelemParam()`

Обработчики вызывают эти функции с нужным `_PAM_RO` / `_PAM_WO`:
- **Чтение**: получаем значение из `gParamApp` / `gParamSystem` / hardware drivers
- **Запись**: валидация -> запись в `gParamApp` / `gParamSystem` -> `SaveParamApp()` / `SaveParamSystem()` -> flash

### 6. Конвертация ошибок
**`ErrorHandler.c`** -> `ConvertUnicornErrorIntoModbusError()`

Access-функции возвращают Unicorn-коды (`_ErrorWriteROParam`, `_ErrorUnCorrParam`...).
Обработчики конвертируют их в стандартные Modbus exception codes (`_IllegalDataAddress`, `_IllegalDataValue`...).

### 7. Отправка ответа
**`ModbusRtuRoutine.c`** -> состояние `RtuTransmit`

После `ModbusRtuFrame_Process()` ответ лежит в `BuffRtu[]`.
Рутина порционно отправляет его через `RS485_PushTxFIFOBuf()` -> UART TX FIFO.

## Схема потока

```
RS485 UART RX
    |
    v
ModbusRtuRoutine.c          <- побайтный приём + таймаут тишины
    |
    v
ModbusRtuFrame.c            <- CRC проверка, адрес, извлечение PDU
    |
    v
CommandParcerModbus.c       <- switch по FC
    |
    |---> ReadHoldingRegisters.c --> ModbusUtils.c --> AccessIntParam.c
    |                                             --> AccessFloatParam.c
    |---> ReadInputRegisters.c  --> ModbusUtils.c --> AccessTelemParam.c
    |---> WriteSingleRegister.c --> ModbusUtils.c --> AccessIntParam.c / AccessFloatParam.c
    +---> WriteMultipleRegisters.c -> ModbusUtils.c -> AccessIntParam.c / AccessFloatParam.c
    |
    v                              ErrorHandler.c <- конвертация ошибок
ModbusRtuFrame.c            <- формирование ответа + CRC
    |
    v
ModbusRtuRoutine.c          <- отправка через RS485 TX FIFO
    |
    v
RS485 UART TX
```

## Переключение протокола

Параметр `_IP_ProtocolMode` (Int param, сохраняется в `gParamApp` -> flash):
- `0` = Unicorn (по умолчанию)
- `1` = Modbus RTU

Переключение вступает в силу после перезагрузки устройства.
При загрузке `main_application.c` проверяет `gParamApp.ProtocolMode` и инициализирует
либо `Unicorn2Routine()`, либо `ModbusRtuRoutine()`.
