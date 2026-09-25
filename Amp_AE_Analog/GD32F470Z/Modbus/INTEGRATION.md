# Интеграция переносимого Modbus RTU

Этот каталог разделён на три слоя. Внешний протокол и PDU обработчики не меняются: поддерживаются FC `0x03`, `0x04`, `0x06`, `0x10`, `0x14`, `0x15`, `0x2B/0x0E` и пользовательский `0x42`.

| Слой | Файлы | Зависимости | Задача |
|---|---|---|---|
| Core | `Core/ModbusCore.*` | C99 + `Backend/ModbusBackend.h` | CRC16, адресация RTU, broadcast, exception и упаковка ответа |
| Port | `Port/ModbusRtuPort.*` | Core + Backend | t1.5/t3.5, сборка кадра и отправка через абстракцию UART |
| Backend | `Backend/ModbusBackend.h` | нет платформенных | связывает Core с PDU и индикацией |
| GD32 adapter | `Backend/ModbusBackendGd32.*`, `ModbusRtuRoutine.c` | GD32, Unicorn2, RS485 | текущая реализация для этой платы |

## Быстрый перенос

В новый проект скопируйте `Core/`, `Port/` и `Backend/ModbusBackend.h`. Не копируйте `ModbusBackendGd32.*` и `ModbusRtuRoutine.c`: они привязаны к GD32. Добавьте эти исходники в сборку вместе с собственным backend-обработчиком PDU.

На каждый физический интерфейс RS-485 создаются по одному `ModbusRtu`, `ModbusRtuPort` и `ModbusBackend`. Контексты должны жить всё время работы устройства, например как `static` объекты.

```c
static ModbusRtu modbus1;
static ModbusRtuPort modbus1Port;
static ModbusBackend modbus1Backend;

void Modbus1_Init(void)
{
    modbus1Port.context = &uart1;
    modbus1Port.rxCount = Uart1_RxCount;
    modbus1Port.rxPop = Uart1_RxPop;
    modbus1Port.tickUs = Uart1_TickUs;
    modbus1Port.txFree = Uart1_TxFree;
    modbus1Port.txWrite = Uart1_TxWrite;

    modbus1Backend.context = &application;
    modbus1Backend.processPdu = Application_ProcessPdu;
    modbus1Backend.frameActivity = Application_FrameActivity; /* либо NULL */

    ModbusRtu_Init(&modbus1, &modbus1Port, &modbus1Backend,
                   1U, 115200U);
}

void Modbus1_Poll(void)
{
    ModbusRtu_Poll(&modbus1);
}
```

`Modbus1_Poll()` вызывается из основного цикла либо из задачи RTOS. Не вызывайте его из ISR: драйвер должен складывать принятые байты и их timestamp в FIFO.

## Контракт UART: `ModbusRtuPort`

`rxCount(context)` возвращает количество байтов в RX FIFO. `rxPop(context, &byte)` забирает один байт и возвращает `true`; поля `byte.byte` и `byte.timestamp` обязательны. `timestamp` — момент фактического приёма байта в микросекундном 16-битном счётчике, а не время вызова `Poll`.

`tickUs(context)` возвращает тот же 16-битный счётчик. Переполнение разрешено: разности времени вычисляются как `uint16_t`.

`txFree(context)` сообщает число свободных байтов TX FIFO. `txWrite(context, data, size)` кладёт данные в FIFO и возвращает действительно записанное число байтов. Направлением DE/RE RS-485 и физическим завершением передачи управляет драйвер UART.

Слой `Port` использует стандартные интервалы Modbus RTU: t1.5 и t3.5 вычисляются по скорости до 19 200 baud; выше используются 750 и 1750 мкс соответственно. В этой реализации добавлены запасы 100 и 200 мкс для джиттера main loop. Если ваш RX timestamp не аппаратный, эти запасы следует пересмотреть после измерений.

При смене baudrate вызовите `ModbusRtu_Reset()` и затем `ModbusRtu_Init()` с новой скоростью, когда UART уже переключён и TX FIFO пуст. Для смены slave address достаточно обновить `rtu.deviceAddress` между кадрами; GD32 adapter делает это в каждом `ModbusRtuRoutine()`.

## Контракт приложения: `ModbusBackend`

`processPdu(context, pdu, &pduSize)` получает PDU без slave address и CRC. Он изменяет буфер на ответ и устанавливает длину ответного PDU. Возвращаемое значение — Modbus exception code; успех обозначается `_NoError` (`0xFF`). Core сам формирует ответ `[address][PDU][CRC low][CRC high]`, exception и отсутствие ответа на broadcast.

`frameActivity` необязателен. Его вызывают только для валидного кадра, адресованного устройству или broadcast. В GD32 он вызывает `PingActivitiLED()`.

Простейший backend, полезный как основа для нового устройства:

```c
static uint8_t Application_ProcessPdu(void *context, uint8_t *pdu,
                                     uint32_t *pduSize)
{
    (void)context;
    switch (pdu[0]) {
    case 0x03U:
        return App_ReadHolding(pdu, pduSize);
    case 0x42U:
        return App_TableControl(pdu, pduSize);
    default:
        return 0x01U; /* IllegalFunction */
    }
}
```

Текущий `Backend/ModbusBackendGd32.c` адаптирует уже существующий `ModbusCommandProcess()` и поэтому сохраняет полную совместимость с Unicorn2. При переносе можно либо реализовать свой `processPdu`, либо перенести существующие PDU handlers вместе с их доступом к данным.

## Проверка интеграции

1. Подайте корректный RTU запрос и проверьте CRC ответа (CRC low byte передаётся первым).
2. Проверьте кадр с плохим CRC и с чужим адресом: ответа быть не должно.
3. Проверьте broadcast address `0`: данные записываются, но ответа нет.
4. На 9 600 и 115 200 baud проверьте границы t1.5/t3.5 аппаратным анализатором.
5. Запустите host-тесты: `make -C tests/modbus test` (либо соберите тот же список sources GCC вручную в Windows).

Для добавления нового FC меняйте только backend/PDU dispatcher и добавляйте host-тест. Core и Port менять не требуется.
