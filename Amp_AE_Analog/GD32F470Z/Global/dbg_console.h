#ifndef __DBG_CONSOLE_H__
#define __DBG_CONSOLE_H__

#include <stdio.h>

#define DBG_CONSOLE_NONE    (0)
#define DBG_CONSOLE_USART   (1)
#define DBG_CONSOLE_SWO     (2)
#define DBG_CONSOLE_RTT     (3)

//#define DBG_CONSOLE   DBG_CONSOLE_USART
#define DBG_CONSOLE     DBG_CONSOLE_SWO
//#define DBG_CONSOLE   DBG_CONSOLE_RTT
#ifndef DBG_CONSOLE
#define DBG_CONSOLE	DBG_CONSOLE_NONE
#endif

void dbg_console_init(void);

#endif
