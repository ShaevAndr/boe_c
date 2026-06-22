# Modbus host tests

Тесты компилируют реальные исходники из `Modbus/` локальным GCC. Аппаратный backend
параметров заменён контролируемыми заглушками `AccessIntParam`, `AccessFloatParam`
и `AccessTelemParam`.

Запуск:

```sh
./tests/modbus/run_tests.sh
```

Либо напрямую:

```sh
make -C tests/modbus test
make -C tests/modbus sanitize
```

Проверяется:

- mapping INT/FLOAT/telemetry адресов;
- преобразование внутренних ошибок в Modbus exceptions;
- FC `0x03`, `0x04`, `0x06`, `0x10`, `0x14`;
- FC `0x2B` / MEI `0x0E` Read Device Identification;
- валидация размеров, количества и адресов;
- big-endian сериализация 32-битных значений;
- диспетчер функций;
- RTU CRC, адрес устройства, exception response и broadcast без ответа.
