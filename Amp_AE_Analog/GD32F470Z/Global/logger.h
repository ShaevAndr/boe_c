/*! 
 * @file	logger.h
 * @brief
 * @author	Andrew Fomushkin (silicat@gmail.com)
 *
 */


#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <stdarg.h>
#include "global.h"

#ifndef GLOBAL_CLOG_LEVEL
#define GLOBAL_CLOG_LEVEL	10
//#define GLOBAL_CLOG_LEVEL	0
#endif

#define MYLOG_ERROR(log, format_str, args...)	\
	{ if (likely(log != NULL))  \
		log_print(log, ELLError, __FILE__, __func__, __LINE__, \
				format_str, ## args); }
#define MYLOG_WARNING(log, format_str, args...)	\
	{ if (likely(log != NULL)) \
		log_print(log, ELLWarning, __FILE__, __func__, __LINE__, \
				format_str, ## args); }
#define MYLOG_NOTICE(log, format_str, args...)	\
	{ if (likely(log != NULL)) \
		log_print(log, ELLNotice, __FILE__, __func__, __LINE__, \
				format_str, ## args); }
#define MYLOG_INFO(log, format_str, args...)	\
	{ if (likely(log != NULL)) \
		log_print(log, ELLInfo, __FILE__, __func__, __LINE__, \
				format_str, ## args); }
#define MYLOG_DEBUG(log, format_str, args...)	\
	{ if (likely(log != NULL)) \
		log_print(log, ELLDebug, __FILE__, __func__, __LINE__, \
				format_str, ## args); }

#if GLOBAL_CLOG_LEVEL >= 1
#define CLOG_ERROR(format_str, args...)	MYLOG_ERROR(&gLogger, format_str, ## args)
#else
#define CLOG_ERROR(format_str, args...)
#endif

#if GLOBAL_CLOG_LEVEL >= 2
#define CLOG_WARNING(format_str, args...)	MYLOG_WARNING(&gLogger, format_str, ## args)
#else
#define CLOG_WARNING(format_str, args...)
#endif

#if GLOBAL_CLOG_LEVEL >= 3
#define CLOG_NOTICE(format_str, args...)	MYLOG_NOTICE(&gLogger, format_str, ## args)
#else
#define CLOG_NOTICE(format_str, args...)
#endif

#if GLOBAL_CLOG_LEVEL >= 4
#define CLOG_INFO(format_str, args...)		MYLOG_INFO(&gLogger, format_str, ## args)
#else
#define CLOG_INFO(format_str, args...)
#endif

#if GLOBAL_CLOG_LEVEL >= 5
#define CLOG_DEBUG(format_str, args...)	MYLOG_DEBUG(&gLogger, format_str, ## args)
#else
#define CLOG_DEBUG(format_str, args...)
#endif

char* data_tostring(const void *data, int len, char *buf, int maxbufsize);
char* data_ascii(const void *data, int len, char *buf, int maxbufsize);
char ascii_printable(char c);

typedef enum LogLevel
{
	ELLError 	= 1,
	ELLWarning	= 2,
	ELLNotice	= 3,
	ELLInfo		= 4,
	ELLDebug	= 5,

	ELLMaxLevel
} ELogLevel;

#define SUBSYSTEM_MAXLEN  (10)

typedef struct Logger {
	char subsystem[SUBSYSTEM_MAXLEN];
	ELogLevel logLevel;
} Logger_t;

extern void log_init(Logger_t *logger, const char* subsystem, ELogLevel logLevel);
extern void log_initGlobal(const char* subsystem, ELogLevel logLevel);
extern void log_print(Logger_t *logger, ELogLevel logLevel, const char *fileName,
			const char *funcName, unsigned int line, const char *format, ...);
extern void log_vprint(Logger_t *logger, ELogLevel logLevel, const char *fileName,
			const char *funcName, unsigned int line, const char *format, va_list args);

extern Logger_t gLogger;

#endif //__LOGGER_HPP__
