#define MULTI_LINE_STRING(...) #__VA_ARGS__
//static const char UD_JSON [] = MULTI_LINE_STRING(
const char UD_JSON [] = MULTI_LINE_STRING(
JSON\n

{
	"UUID": true,
	"Reset": true,
	"ChgModbusAddr": true,
	"ParamInt": [
		{
			"ID": "EnableLoadApp",
			"Num": 0,
			"Name": "Enable load application",
			"Type": "Bool",
			"Access": "rw"
		},
		{
			"ID": "MACAddrUpperThreeBytes",
			"Num": 1,
			"Name": "MAC address upper three bytes",
			"Type": "halfmac",
			"Access": "ro"
		},
		{
			"ID": "MACAddrLowThreeBytes",
			"Num": 2,
			"Name": "MAC address low three bytes",
			"Type": "halfmac",
			"Access": "ro"
		},
		{
			"ID": "TotalOperatingTime",
			"Num": 3,
			"Name": "Total operating time",
			"Type": "Regular",
			"Desc": "",
			"Unit": "hours",
			"FormatStr": "%d",
			"Access": "ro"
		},
		{
			"ID": "NumberOfBootloaderLaunches",
			"Num": 4,
			"Name": "Number of bootloader launches",
			"Type": "Regular",
			"Desc": "",
			"FormatStr": "%d",
			"Access": "ro"
		},
		{
			"ID": "NumberOfApplicationLaunches",
			"Num": 5,
			"Name": "Number of application launches",
			"Type": "Regular",
			"Desc": "",
			"FormatStr": "%d",
			"Access": "ro"
		},
		{
			"ID": "NumberErrorWriteDeviceUsStat",
			"Num": 6,
			"Name": "Number error write device us stat",
			"Type": "Regular",
			"FormatStr": "%d",
			"Access": "ro"
		},
		{
			"ID": "IPFactorySettingsLock",
			"Num": 7,
			"Name": "Factory settings change allowed",
			"Type": "Bool",
			"Desc": "",
			"Access": "ro"
		},
		{
			"ID": "IPFactorySettingsKey",
			"Num": 8,
			"Name": "Factory settings key",
			"Type": "Regular",
			"FormatStr": "0x%08X",
			"Desc": "",
			"Access": "rw"
		},
		{
			"ID": "DeviceDateOfManufacture",
			"Num": 9,
			"Name": "DeviceDateOfManufacture",
			"Type": "Regular",
			"FormatStr": "%T",
			"Access": "rw"
		},
		{
			"ID": "DeviceSerialNumber",
			"Num": 10,
			"Name": "DeviceSerialNumber",
			"Type": "Regular",
			"FormatStr": "%d",
			"Access": "rw"
		}
	],
	"Telemetry": [
		{
			"ID": "CoreUptime",
			"Num": 0,
			"Name": "Core uptime",
			"Type": "Time"
		}
	],
	"Flashes": [
		{
			"Name": "Flash memory for saving settings",
			"Num": 0,
			"PageSize": 128,
			"PagesCount": 256
		},{
			"Name": "Application",
			"Num": 1,
			"PageSize": 4096,
			"PagesCount": 480,
			"Offset": 134348800
		}
	],
	"UART": {
		"Values": [
			{
				"Name": "9600",
				"Value": 0
			},{
				"Name": "19200",
				"Value": 1
			},{
				"Name": "38400",
				"Value": 2
			},{
				"Name": "57600",
				"Value": 3
			},{
				"Name": "115200",
				"Value": 4
			},{
				"Name": "345600",
				"Value": 5
			}
		]
	}
}

);
