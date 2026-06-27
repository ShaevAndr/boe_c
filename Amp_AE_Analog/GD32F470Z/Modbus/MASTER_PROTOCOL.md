# Modbus RTU master protocol

This document is the external contract for a Modbus RTU master application that talks to this device firmware. `Modbus/README.md` describes internal firmware flow; this file describes request/response bytes and command signatures.

## Transport

All requests and responses are Modbus RTU frames:

```text
Request:  [Addr][PDU...][CRC_L][CRC_H]
Response: [Addr][PDU...][CRC_L][CRC_H]
```

- `Addr`: slave address, 1..254. Address `0` is broadcast; the device executes supported write requests but sends no response.
- `CRC`: Modbus CRC16 over `[Addr][PDU...]`, low byte first.
- Multi-byte numeric fields inside PDU are big-endian unless explicitly stated otherwise.
- Normal response keeps the same function code. Exception response is `[Addr][FC | 0x80][ExceptionCode][CRC_L][CRC_H]`.

## Data model

This firmware uses a project-specific 4-byte parameter model for register commands. One Modbus register address is one complete 4-byte parameter, not a standard 16-bit Modbus register.

Current address spaces are defined by `Unicorn2/CommandList.h` and `Modbus/ModbusUtils.c`:

| Space | Address range | Backend type | Value format |
|---|---:|---|---|
| Holding | `0 .. _IPCount - 1` | INT params | raw `int32_t`, 4 bytes BE |
| Holding | `_IPCount .. _IPCount + _FPCount - 1` | FLOAT params | raw IEEE754 `float`, 4 bytes BE |
| Input | `0 .. _TelPCount - 1` | TELEMETRY params | raw IEEE754 `float`, 4 bytes BE |

Current enum counts in this firmware snapshot:

| Symbol | Value |
|---|---:|
| `_IPCount` | `35` |
| `_FPCount` | `53` |
| `_TelPCount` | `83` |
| `_TabPCount` | `1` |

Holding addresses therefore are:

| Address range | Meaning |
|---:|---|
| `0..34` | INT parameters |
| `35..87` | FLOAT parameters, float index = address - 35 |

Input addresses are `0..82` telemetry parameters.

## Limits

| Item | Limit |
|---|---:|
| PDU max, practical | `253` bytes |
| RTU frame buffer | `256` bytes |
| Read/write register quantity | `1..60` parameters |
| Register command byte count | `Quantity * 4` |

## Function codes

| FC | Name | Direction | Handler |
|---:|---|---|---|
| `0x03` | Read Holding Registers / parameters | read INT/FLOAT | `ReadHoldingRegisters()` |
| `0x04` | Read Input Registers / telemetry | read TELEMETRY | `ReadInputRegisters()` |
| `0x06` | Write Single Register / parameter | write INT/FLOAT | `WriteSingleRegister()` |
| `0x08` | Diagnostics extensions | table metadata/control | `Diagnostics()` |
| `0x10` | Write Multiple Registers / parameters | write INT/FLOAT | `WriteMultipleRegisters()` |
| `0x14` | Read File Record | device description and table bytes | `ReadFileRecord()` |
| `0x15` | Write File Record | table bytes | `WriteFileRecord()` |
| `0x2B / MEI 0x0E` | Read Device Identification | device metadata | `ReadDeviceIdentification()` |

## FC 0x03 - Read Holding Registers

Reads INT/FLOAT parameters from holding address space.

Request PDU:

```text
[03][StartAddr_H][StartAddr_L][Quantity_H][Quantity_L]
```

Response PDU:

```text
[03][ByteCount][Data...]
```

Fields:

| Field | Size | Meaning |
|---|---:|---|
| `StartAddr` | 2 | first holding parameter address |
| `Quantity` | 2 | number of 4-byte parameters, `1..60` |
| `ByteCount` | 1 | `Quantity * 4` |
| `Data` | `4 * Quantity` | each parameter as 4 bytes BE |

Errors:

- `0x02 IllegalDataAddress`: requested range is outside holding address space.
- `0x03 IllegalDataValue`: quantity is `0` or greater than `60`.

Example PDU, read one INT parameter at holding address `0`:

```text
Request PDU:  03 00 00 00 01
Response PDU: 03 04 VV VV VV VV
```

## FC 0x04 - Read Input Registers

Reads telemetry parameters from input address space.

Request PDU:

```text
[04][StartAddr_H][StartAddr_L][Quantity_H][Quantity_L]
```

Response PDU:

```text
[04][ByteCount][Data...]
```

Fields are the same as FC `0x03`, but addresses are input telemetry addresses `0.._TelPCount-1`.

Errors:

- `0x02 IllegalDataAddress`: requested range is outside input address space.
- `0x03 IllegalDataValue`: quantity is `0` or greater than `60`.

## FC 0x06 - Write Single Register

Writes one INT/FLOAT holding parameter.

Request PDU:

```text
[06][RegAddr_H][RegAddr_L][Value_B3][Value_B2][Value_B1][Value_B0]
```

Response PDU is an exact echo of the request PDU.

Fields:

| Field | Size | Meaning |
|---|---:|---|
| `RegAddr` | 2 | holding parameter address |
| `Value` | 4 | raw int32 or float bits, BE |

Errors:

- `0x02 IllegalDataAddress`: `RegAddr` is outside holding address space.
- `0x03 IllegalDataValue`: backend rejected write, for example read-only parameter.

## FC 0x10 - Write Multiple Registers

Writes consecutive INT/FLOAT holding parameters.

Request PDU:

```text
[10][StartAddr_H][StartAddr_L][Quantity_H][Quantity_L][ByteCount][Data...]
```

Response PDU:

```text
[10][StartAddr_H][StartAddr_L][Quantity_H][Quantity_L]
```

Fields:

| Field | Size | Meaning |
|---|---:|---|
| `StartAddr` | 2 | first holding parameter address |
| `Quantity` | 2 | number of 4-byte parameters, `1..60` |
| `ByteCount` | 1 | must equal `Quantity * 4` |
| `Data` | `4 * Quantity` | each parameter as 4 bytes BE |

Errors:

- `0x02 IllegalDataAddress`: requested range is outside holding address space.
- `0x03 IllegalDataValue`: invalid quantity, byte count, truncated request, or backend write rejection.

## FC 0x14 - Read File Record

Reads variable-length byte files. In this firmware it is used for device description and table parameter data.

Request PDU:

```text
[14][ByteCount][RefType][FileNumber_H][FileNumber_L][RecordNumber_H][RecordNumber_L][RecordLength_H][RecordLength_L]
```

Response PDU:

```text
[14][ResponseDataLength][FileResponseLength][RefType][RecordData...]
```

Fields:

| Field | Size | Meaning |
|---|---:|---|
| `ByteCount` | 1 | request sub-request length, must be `7` |
| `RefType` | 1 | file reference type, `6` |
| `FileNumber` | 2 | file identifier |
| `RecordNumber` | 2 | record index |
| `RecordLength` | 2 | retained for PDU compatibility; ignored when reading table files |
| `ResponseDataLength` | 1 | `2 + len(RecordData)` |
| `FileResponseLength` | 1 | `1 + len(RecordData)` |
| `RecordData` | variable | bytes returned |

Supported files:

| FileNumber | Name | Read behavior | Write behavior |
|---:|---|---|---|
| `1` | Device description | `RecordNumber * 245` byte offset, up to `245` bytes | not supported |
| `100` | Test table parameter | from `RecordNumber * 2`, up to `249` bytes per response | supported by FC `0x15` |

Important compatibility notes:

- For file `1`, `RecordLength` is validated only as non-zero; the response chunk size is driven by internal `CHUNK_SIZE = 245`.
- For table files, `RecordLength` does not limit the response; its value, including zero, is ignored.
- Only one 7-byte sub-request is supported: request PDU size must be `9`, `ByteCount` must be `7`, and `RefType` must be `6`.
- A table read requests up to `249` bytes from `AccessTabParam`, the maximum that fits after the 4-byte response overhead in a 253-byte PDU.
- `AccessTabParam` receives `offset = RecordNumber * 2`, `size = 249`, `wordSize = 2`, and `stride = 2`; it reduces `size` to the bytes actually available.
- If the offset is at or beyond EOF, `RecordData` is empty and the response is `[14][02][01][06]`.
- No table-level CRC is present: the Modbus RTU CRC already covers the complete request and response frame.

Example PDU, read table file `100` from its beginning. `RecordLength=4` is present in the request but does not limit the response:

```text
Request PDU:  14 07 06 00 64 00 00 00 04
Response PDU: 14 12 11 06 10 11 12 13 20 21 22 23 30 31 32 33 40 41 42 43
```

## FC 0x15 - Write File Record

Writes table parameter data. Currently only file `100` is writable.

Request PDU:

```text
[15][ByteCount][RefType][FileNumber_H][FileNumber_L][RecordNumber_H][RecordNumber_L][RecordLength_H][RecordLength_L][RecordData...]
```

Response PDU is an exact echo of the request PDU.

Fields:

| Field | Size | Meaning |
|---|---:|---|
| `ByteCount` | 1 | must equal `7 + len(RecordData)` |
| `RefType` | 1 | must be `6` |
| `FileNumber` | 2 | writable table file number, currently `100` |
| `RecordNumber` | 2 | byte offset is `RecordNumber * 2` |
| `RecordLength` | 2 | write length in 2-byte records, must be non-zero |
| `RecordData` | `RecordLength * 2` | bytes to write |

Validation:

- PDU size must equal `2 + ByteCount`.
- `ByteCount` must equal `7 + RecordLength * 2`.
- `RefType` must be `6`.
- `RecordLength` must be in the range `1..122`, keeping the request within the 253-byte PDU limit.
- `AccessTabParam` receives `offset = RecordNumber * 2`, `size = RecordLength * 2`, and `stride = 2`.
- Write range must fit inside the table file.

Example PDU, write 4 bytes at byte offset `4` of table file `100`:

```text
Request PDU:  15 0B 06 00 64 00 02 00 02 AA BB CC DD
Response PDU: 15 0B 06 00 64 00 02 00 02 AA BB CC DD
```

## FC 0x08 - Diagnostics table extensions

Diagnostics is used here for table metadata/control. Subfunction is bytes `[1..2]` of the PDU, big-endian.

### Subfunction 0x0005 - table count

Request PDU:

```text
[08][00][05]
```

Response PDU:

```text
[08][00][05][Count_B3][Count_B2][Count_B1][Count_B0]
```

Current `Count = 1`.

### Subfunction 0x0006 - table description

Request PDU:

```text
[08][00][06][TableIndex_B3][TableIndex_B2][TableIndex_B1][TableIndex_B0]
```

Response PDU:

```text
[08][00][06][TableIndex:4][DescriptionSize:4][ASCII Description...]
```

Current table index `0` description is `Test table parameter; file=100; bytes=16`.

### Subfunction 0x0007 - prepare table

Request PDU:

```text
[08][00][07][TableIndex:4][Column:4]
```

Response PDU echoes `[08][00][07][TableIndex:4][Column:4]`.

`Column` is signed int32 BE. This subfunction always means read preparation (`mode = 0` in the Unicorn table protocol), so no separate mode field is transmitted. Current stub accepts the value and returns success for table index `0`.

### Subfunction 0x0008 - prepare progress

Request PDU:

```text
[08][00][08][TableIndex:4]
```

Response PDU:

```text
[08][00][08][TableIndex:4][CurrentStep:4][StepsCount:4][Rows:4][Columns:4]
```

Current stub values for table index `0`: `CurrentStep=1`, `StepsCount=1`, `Rows=4`, `Columns=4`.

### Subfunction 0x0009 - release table

Request PDU:

```text
[08][00][09][TableIndex:4]
```

Response PDU echoes `[08][00][09][TableIndex:4]`.

## FC 0x2B / MEI 0x0E - Read Device Identification

Request PDU:

```text
[2B][0E][ReadDevIdCode][ObjectId]
```

Response PDU:

```text
[2B][0E][ReadDevIdCode][Conformity][MoreFollows][NextObjectId][ObjectCount]
[ObjectId][Length][ASCII Value]...
```

Supported `ReadDevIdCode`:

| Code | Meaning |
|---:|---|
| `0x01` | Basic objects `0x00..0x02` |
| `0x02` | Regular objects, includes `0x04` |
| `0x03` | Regular objects, includes `0x04` |
| `0x04` | Individual object by `ObjectId` |

Objects:

| ObjectId | Name | Value source |
|---:|---|---|
| `0x00` | VendorName | `Unicorn` |
| `0x01` | ProductCode | `_TypeDev` formatted as `0xXXXX` |
| `0x02` | MajorMinorRevision | `_VerDev` formatted as `major.minor` |
| `0x04` | ProductName | runtime `UnitDescription` |

Conformity is `0x82`. Segmentation is not implemented; requested objects must fit into one response PDU.

## Exception codes

The Modbus layer returns standard exception codes:

| Code | Name | Typical source |
|---:|---|---|
| `0x01` | IllegalFunction | unsupported function code |
| `0x02` | IllegalDataAddress | unknown parameter, file, table, or out-of-range address |
| `0x03` | IllegalDataValue | invalid size/count/byte count/ref type or read-only write rejection |
| `0x04` | SlaveDeviceFailure | internal error or impossible command |
| `0x06` | SlaveDeviceBusy | data not ready or timeout |

## Master implementation checklist

1. Build PDU according to the command section.
2. Wrap PDU into RTU frame: prepend slave address and append CRC16 low byte first.
3. For normal responses, validate address, function code, PDU length, and CRC.
4. If response function has bit `0x80`, parse byte 2 as exception code.
5. Decode all 4-byte values big-endian. For floats, reinterpret the 32-bit payload as IEEE754 single precision.
6. Treat FC `0x03/0x04/0x06/0x10` register addresses as parameter indexes, not 16-bit register word offsets.
