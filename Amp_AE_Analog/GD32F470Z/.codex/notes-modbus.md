# Modbus RTU notes for current development

## Scope

Это основная активная область разработки в текущем режиме.

По умолчанию для задач по протоколу сначала смотреть:

- `Modbus/ModbusRtuRoutine.c`
- `Modbus/ModbusRtuFrame.c`
- `Modbus/CommandParcerModbus.c`
- `Modbus/ModbusUtils.c`
- `Modbus/ReadHoldingRegisters.c`
- `Modbus/ReadInputRegisters.c`
- `Modbus/WriteSingleRegister.c`
- `Modbus/WriteMultipleRegisters.c`
- `Modbus/ReadDeviceIdentification.c`
- `Modbus/ReadFileRecord.c`
- `Unicorn2/AccessIntParam.c`
- `Unicorn2/AccessFloatParam.c`
- `Unicorn2/AccessTelemParam.c`

## Normative source

Для форматов и правил Modbus использовать `Modbus_Application_Protocol_V1_1b3-2.pdf`.
Для Read Device Identification нормативный раздел — 6.21.

Объекты Read Device Identification:

- `0x00` VendorName = `DEVICE_VENDOR_NAME`;
- `0x01` ProductCode = `_TypeDev` как ASCII `0xXXXX`;
- `0x02` MajorMinorRevision = `_VerDev` как ASCII `major.minor`;
- `0x04` ProductName = `UnitDescription`.

Реализация использует conformity `0x82` и не сегментирует ответ: весь набор должен поместиться в один PDU.

## Current protocol model

Реализация не является стандартным word-oriented Modbus mapping.

В этом проекте:

- один Modbus register address = один параметр;
- один параметр всегда 4 байта;
- `FC 0x03` и `FC 0x06/0x10` работают с INT/FLOAT параметрами;
- `FC 0x04` работает с telemetry параметрами;
- сериализация значений идёт big-endian по 4 байта на параметр.

## Active function codes

Сейчас диспетчер `ModbusCommandProcess()` маршрутизирует:

- `0x03` -> `ReadHoldingsRegisters()`
- `0x04` -> `ReadInputRegisters()`
- `0x06` -> `WriteSingleRegister()`
- `0x2B/0x0E` -> `ReadDeviceIdentification()`; standard Basic/Regular identification objects in one response
- `0x10` -> `WriteMultipleRegisters()`
- `0x14` -> `ReadFileRecord()`; file 1 contains the full device description

Если добавляется новый FC, первая точка входа — `Modbus/CommandParcerModbus.c`.

## Processing pipeline

Поток запроса такой:

1. `ModbusRtuRoutine()`
   - читает байты из RS485 FIFO;
   - собирает RTU frame;
   - контролирует `t1.5` и `t3.5`;
   - по завершению кадра передаёт его в `ModbusRtuFrame_Process()`.

2. `ModbusRtuFrame_Process()`
   - проверяет CRC16;
   - проверяет slave address / broadcast;
   - передаёт PDU в `ModbusCommandProcess()`;
   - собирает normal response или exception response.

3. `ModbusCommandProcess()`
   - проверяет минимальный размер PDU;
   - маршрутизирует запрос в конкретный handler.

4. Handler (`Read*` / `Write*`)
   - валидирует диапазон адресов и количество;
   - через `ModbusUtils.c` переводит register address в тип параметра и индекс;
   - через `Access*Param` читает/пишет данные.

5. `ErrorHandler.c`
   - конвертирует внутренние коды Unicorn в Modbus exception codes.

6. `ModbusRtuRoutine()`
   - отправляет готовый ответ через TX FIFO.

## Address mapping

`Modbus/ModbusUtils.c` задаёт карту адресов.

Holding register space:

- `0 .. _IPCount - 1` -> INT params
- `_IPCount .. _IPCount + _FPCount - 1` -> FLOAT params

Input register space:

- `0 .. _TelPCount - 1` -> TELEMETRY params

Следствие:

- любое изменение доступного Modbus address space обычно завязано на `_IPCount`, `_FPCount`, `_TelPCount` и enum/списки параметров из Unicorn2;
- Modbus layer сам по себе почти не хранит карту вручную, он опирается на диапазоны.

## Important behavior details

### RTU timing

`ModbusRtuRoutine.c`:

- использует timestamp принятого байта из IRQ;
- считает `t3.5` и `t1.5` в микросекундах;
- для baud > 19200 использует фиксированные пороги;
- добавляет запас по времени:
  - `MODBUS_RTU_SILENCE_EXTRA_US = 200`
  - `MODBUS_RTU_INTERCHAR_EXTRA_US = 100`

Если будут жалобы на нестабильный приём, смотреть сначала сюда.

### Frame limits

Текущие лимиты:

- max quantity для read/write: `60`
- max frame buffer: `256` байт

Если увеличивать объём данных, нужно проверять одновременно:

- лимит в конкретном handler;
- размер буфера в `ModbusRtuRoutine.c`;
- `ByteCount` как 1-байтное поле;
- итоговый RTU frame size.

### Data encoding

- INT параметры передаются как raw `int32_t` в big-endian
- FLOAT параметры передаются как raw IEEE754 `float` через 4-байтовый образ в big-endian

Если внешний мастер декодирует значения неверно, сначала проверять именно endian/packing ожидания.

## Typical change points

### Add or expose a new parameter via Modbus

Обычно нужно проверить:

- enum / count в Unicorn2 command lists
- `AccessIntParam` или `AccessFloatParam` / `AccessTelemParam`
- достаточно ли текущего диапазона `_IPCount/_FPCount/_TelPCount`

В самом `Modbus/` часто ничего менять не нужно, если параметр уже встроен в соответствующий диапазон.

### Change read/write validation

Смотреть:

- `ReadHoldingRegisters.c`
- `ReadInputRegisters.c`
- `WriteSingleRegister.c`
- `WriteMultipleRegisters.c`
- `ErrorHandler.c`

### Add a new function code

Минимальный маршрут изменений:

- объявить FC в `modbusCommands.h` / связанных заголовках;
- добавить case в `CommandParcerModbus.c`;
- создать новый handler `.c/.h`;
- добавить новые файлы в `BUE-8_GD32F470Z.uvprojx`;
- при необходимости расширить `ModbusRtuFrame.c` и error mapping.

### Diagnose no response / broken response

Проверять по порядку:

- `gParamApp.ProtocolMode == PROTOCOL_MODBUS`
- инициализацию `ModbusRtuRoutine_Init()` в `main_application.c`
- адрес устройства `gParamSystem.rs485Modbus[uartNum].ModbusAddress`
- CRC и размер кадра
- gap/timing drop в `ModbusRtuRoutine.c`
- попадание в нужный handler
- коды ошибок из `Access*Param`

## Current practical assumption

Для текущей работы считать, что:

- bootloader не участвует;
- Unicorn2 интересует только как backend параметров;
- основной слой изменений и диагностики — `Modbus/` + связанные `Access*Param`.
