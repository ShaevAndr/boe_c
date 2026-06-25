# Agent: Modbus command documentation maintainer

## Purpose

Keep `Modbus/MASTER_PROTOCOL.md` synchronized with the firmware's externally visible Modbus command signatures so a master application can be generated from that document.

## When to run

Run this agent whenever a change touches any of these files:

- `Modbus/CommandParcerModbus.c`
- `Modbus/CommandParcerModbus.h`
- `Modbus/ReadHoldingRegisters.c/.h`
- `Modbus/ReadInputRegisters.c/.h`
- `Modbus/WriteSingleRegister.c/.h`
- `Modbus/WriteMultipleRegisters.c/.h`
- `Modbus/ReadFileRecord.c/.h`
- `Modbus/WriteFileRecord.c/.h`
- `Modbus/Diagnostics.c/.h`
- `Modbus/TabParamFiles.c/.h`
- `Modbus/ReadDeviceIdentification.c/.h`
- `Modbus/ModbusUtils.c/.h`
- `Modbus/ErrorHandler.c/.h`
- `Unicorn2/CommandList.h`
- `tests/modbus/test_modbus.c`

## Inputs

Primary source of truth is code, not prose:

1. Function-code constants and dispatch table: `Modbus/CommandParcerModbus.h` and `Modbus/CommandParcerModbus.c`.
2. PDU field parsing and validation: each command handler `.c` file.
3. Table diagnostics subfunctions: `Modbus/Diagnostics.h` and `Modbus/Diagnostics.c`.
4. File numbers and table backing data: `Modbus/ReadFileRecord.h`, `Modbus/TabParamFiles.h`, `Modbus/TabParamFiles.c`.
5. Address ranges and parameter counts: `Modbus/ModbusUtils.c` and `Unicorn2/CommandList.h`.
6. Expected behavior examples: `tests/modbus/test_modbus.c`.

## Update rules

Update `Modbus/MASTER_PROTOCOL.md` whenever any externally visible signature changes:

- function code added, removed, or rerouted;
- minimum request PDU size changes;
- request or response field order, size, endian, or unit changes;
- accepted quantity, byte count, file number, table index, or address range changes;
- diagnostics subfunction added, removed, or changed;
- exception-code mapping changes;
- current enum counts `_IPCount`, `_FPCount`, `_TelPCount`, `_TabPCount` change;
- tests document a new valid or invalid request shape.

Do not update `Modbus/MASTER_PROTOCOL.md` for internal refactors that preserve the wire contract.

## Required checks

After updating the document, run:

```sh
make -C tests/modbus test
make -C tests/modbus sanitize
```

If code did not change and only documentation was updated, these checks are still preferred but may be skipped with a note in the final report.

## Review checklist

- The function-code table in `Modbus/MASTER_PROTOCOL.md` matches `ModbusCommandProcess()`.
- Every supported function code has request PDU and response PDU byte layout.
- Every multi-byte field states endian and unit.
- Custom behavior is explicit: one register address equals one 4-byte parameter.
- File-record behavior distinguishes device description file `1` from table file `100`.
- Diagnostics subfunctions `0x0005..0x0009` match `Modbus/Diagnostics.h`.
- Exception codes match `ConvertUnicornErrorIntoModbusError()` and direct handler returns.
- Examples still match host tests.

## Output

When finished, report:

- whether `Modbus/MASTER_PROTOCOL.md` changed;
- which command signatures changed;
- which tests were run and their result.

