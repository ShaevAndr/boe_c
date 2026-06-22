# GD32F470Z firmware workspace context

## Назначение

Репозиторий содержит bare-metal firmware для `GD32F470ZI` под Keil uVision / ARMClang:

- основное приложение;
- отдельный bootloader;
- общие модули, используемые обеими целями;
- сторонние/вендорные компоненты `LittleFS`, `Modbus`, `RTE`.

Этот файл нужен как короткая рабочая карта проекта, чтобы не тратить токены на повторный обзор структуры при следующих задачах.

## Активная область разработки

Текущий рабочий режим:

- bootloader исключён из активной разработки и из обычного контекста агента;
- основная целевая прошивка — application;
- основной функциональный фокус — `Modbus RTU` в application.

Практически это означает:

- по умолчанию не анализировать и не менять `BootLoader/`;
- по умолчанию начинать любые задачи с `Modbus/`, `main_application.c`, `ParamApp.*`, `Global/ParamSystem.*`;
- Unicorn2 и bootloader рассматривать только если задача явно этого требует.

## Точки входа и цели сборки

- Application:
  - проект: `BUE-8_GD32F470Z.uvprojx`
  - entrypoint: `main_application.c`
  - scatter: `BUE-8_GD32F470Z.sct`
  - define: `__Aplication__`

- Bootloader:
  - проект: `BootLoader/BUE-8_GD32F470Z_BootLoader.uvprojx`
  - entrypoint: `BootLoader/main_bootloader.c`
  - scatter: `BootLoader/BUE-8_GD32F470Z_BootLoader.sct`
  - define: `__BootLoader__`

Общее для обеих целей:

- MCU: `GD32F470ZI`
- toolchain: `ARMClang` в Keil uVision
- частота HXTAL в define: `8000000`

## Структура каталогов

- `/` — application-specific код и драйверы платы.
- `BootLoader/` — отдельный bootloader-проект.
- `Global/` — общие типы, адреса flash, параметры системы, FMC, lock/unlock, logger, debug console.
- `Unicorn2/` — основной протокол/командный стек для application.
- `BootLoader/Unicorn2/` — отдельная копия Unicorn2 для bootloader.
- `Modbus/` — RTU-стек Modbus для application.
- `LittleFS/` — файловая система и storage glue.
- `RTE/` и `BootLoader/RTE/` — CMSIS/device sources и generated pack content.
- `deviceInfo/` — описание устройства.

## Фактическая иерархия исполнения

### Application

`main_application.c`:

- настраивает SysTick на 10 мс;
- поднимает storage через SPI EEPROM;
- загружает `ParamSystem` и `ParamApp`;
- выбирает активный протокол:
  - `PROTOCOL_MODBUS` → `ModbusRtuRoutine()`
  - иначе → `Unicorn2Routine()`
- в основном цикле обслуживает:
  - ADC monitoring;
  - DS18B20;
  - LED state machine;
  - power / environment / BUE checks;
  - statistics;
  - отложенное сохранение параметров;
  - программный reset через `ResetFlag`.

### Bootloader

`BootLoader/main_bootloader.c`:

- поднимает SysTick на 10 мс;
- инициализирует EEPROM-backed storage;
- грузит `ParamSystem`, lock/unlock и командный парсер;
- использует `PacketParser_Init()` / `Unicorn2Routine()` как транспорт обновления;
- через таймер и флаг `EnableLoadApp` переходит в application;
- jump в приложение делает через:
  - установку `SCB->VTOR = _StartApplicationAddress`;
  - `BootJump(_StartApplicationAddress)`.

## Память и важное расхождение

`Global/global.h` задаёт ожидаемую раскладку:

- bootloader start: `0x08000000`
- bootloader size: `128 KiB`
- application start: `0x08020000`
- application size: `15 * 128 KiB`

Но scatter-файлы сейчас выглядят так:

- `BootLoader/BUE-8_GD32F470Z_BootLoader.sct` → `0x08000000`, размер `0x00020000`
- `BUE-8_GD32F470Z.sct` → тоже `0x08000000`, размер `0x001E0000`

В текущем режиме разработки это допустимо и осознанно:

- bootloader временно исключён из цикла разработки;
- application специально линкуется и запускается напрямую с `0x08000000`,
  чтобы не тратить время на загрузку через bootloader.

Если bootloader вернётся в рабочий контур, адресное пространство и linker layout нужно будет снова согласовать.

## Что обычно важно смотреть первым

Если задача про:

- протоколы связи:
  - в первую очередь `Modbus/`
  - затем `main_application.c`
  - при необходимости `ParamApp.*`

- параметры, persistence, flash:
  - `ParamApp.*`
  - `Global/ParamSystem.*`
  - `Global/drv_InternalFMC.*`
  - `LittleFS/lfs_storage.*`
  - `drv_EEPROM.*`

- старт, reset, переход bootloader ↔ app:
  - `Global/global.h`
  - оба `main_*.c`
  - оба `.sct`

- диагностика платы и периферия:
  - `drv_*`
  - `CheckEnviron.*`
  - `CheckBUE.*`
  - `Device*Statistics.*`

## Где вероятно дублирование

Есть отдельные копии модулей между application и bootloader, минимум:

- `Unicorn2/` ↔ `BootLoader/Unicorn2/`
- часть `Global/`
- storage / EEPROM / SPI / LEDs / watchdog

При изменениях в протоколе или общих структурах нужно каждый раз проверять, не требуется ли симметричное изменение во второй цели.
Но в текущем рабочем режиме это обычно несущественно, потому что bootloader исключён из активной области изменений.

## Практические правила для дальнейшей работы

- По умолчанию не трогать `RTE/Device/...` без явной причины: это в основном pack/vendor code.
- Для обзора проекта сначала читать `main_application.c`, `Modbus/`, `ParamApp.*`, `Global/ParamSystem.*`.
- По умолчанию не читать и не менять `BootLoader/`, так как он исключён через `.codexignore`.
- Generated output (`Objects`, `Listings`, JLink log, IDE session files) не использовать как первичный источник, кроме случаев анализа линковки или map-файла.
- При изменении адресов flash или boot sequence обязательно согласовывать:
  - `Global/global.h`
  - оба `.sct`
  - код jump в bootloader.
