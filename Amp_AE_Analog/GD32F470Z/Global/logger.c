
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "drv_time.h"

#include "Time.h"
#include "logger.h"

//#define USE_UART_JTAG
#define USE_UART_CONSOLE
#define __USE_UART_CONSOLE_DMA__

#ifdef USE_UART_JTAG
#endif // USE_UART_JTAG

#ifdef USE_UART_CONSOLE
//#define USE_UART_NR			(0)
//#define USE_UART_BITRATE	(115200)
//#define USE_UART_DMA_NR		UART0TX_DMA
//#include "drv_uart_bf532.h"
//#include "BfDebugger.h"
#endif // USE_UART_CONSOLE

static const char ELogLevelChar[] =
{
	' ', 'E', 'W', 'N', 'I', 'D'
};

Logger_t gLogger;

void log_init(Logger_t *logger, const char* subsystem, ELogLevel logLevel)
{
	strncpy(logger->subsystem, subsystem, SUBSYSTEM_MAXLEN);
	logger->logLevel = logLevel;
#if defined(USE_UART_CONSOLE)
	//UartInitTerminal(USE_UART_NR,USE_UART_BITRATE);
	//UartDmaInitTx(USE_UART_NR,USE_UART_DMA_NR);
#endif
}

void log_initGlobal(const char* subsystem, ELogLevel logLevel)
{
	log_init(&gLogger, subsystem, logLevel);
}

void log_print(Logger_t *logger, ELogLevel logLevel, const char *fileName,
		const char *funcName, unsigned int line, const char *format, ...)
{
	va_list args;

	va_start(args, format);
	log_vprint(logger, logLevel, fileName, funcName, line, format, args);
	va_end(args);
}

void log_vprint(Logger_t *logger, ELogLevel logLevel, const char *fileName,
		const char *funcName, unsigned int line, const char *format, va_list args)
{
	char tmp_format[200];
	
    uint32_t ts = GetTimeMSec();
	snprintf(tmp_format, sizeof(tmp_format), "[%6u.%03u]%c %s:%s():%4u> %s\n",
			ts/1000U, ts%1000U, ELogLevelChar[logLevel], fileName+2,
			funcName, line, format);
	tmp_format[sizeof(tmp_format)-1] = '\n';
#ifdef USE_UART_JTAG
	vprintf(tmp_format, args);
#elif defined(USE_UART_CONSOLE)
	//udvprintf(USE_UART_NR, USE_UART_DMA_NR, tmp_format, args);
    vprintf(tmp_format, args);
#endif
	//fflush(stdout);
}

char *data_tostring(const void *data, int len, char *buf, int maxbufsize)
{
	int i, printLen;

	printLen = min(len, (maxbufsize-1)/2);
	for (i = 0; i < printLen; i++)
		sprintf(buf + i*2, "%02X", ((const unsigned char*)data)[i]);
	buf[i*2] = '\0';
	return buf;
}

char *data_ascii(const void *data, int len, char *buf, int maxbufsize)
{
	int i, printLen;
	char c;

	printLen = min(len, maxbufsize);
	for (i = 0; i < printLen; i++)
	{
		c = ((const char*)data)[i];
		if ((c >= 0x20) && (c <= 0x7E))
			buf[i] = c;
		else
			buf[i] = '.';
	}
	buf[i] = '\0';
	return buf;
}

char ascii_printable(char c)
{
	return ((c >= 0x20) && (c <= 0x7E) ? c : '.');
}
