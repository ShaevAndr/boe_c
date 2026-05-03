
#include <stdio.h>

//#include "drv_TestPinKT.h"
#include "dbg_console.h"

#if DBG_CONSOLE == DBG_CONSOLE_USART
//#include "gd32f4xx.h"

//#define CONSOLE_BAND_RATE				128000U
#define CONSOLE_BAND_RATE				115200U
//#define CONSOLE_BAND_RATE				256000U
//#define CONSOLE_BAND_RATE				800000U

#define CONSOLE_USART                   USART0
#define CONSOLE_USART_CLK               RCU_USART0

#define CONSOLE_USART_TX_PIN            GPIO_PIN_9
#define CONSOLE_USART_RX_PIN            GPIO_PIN_10

#define CONSOLE_USART_GPIO_PORT         GPIOA
#define CONSOLE_USART_GPIO_CLK          RCU_GPIOA
#define CONSOLE_USART__AF               GPIO_AF_7

void console_usart_init()
{
	rcu_periph_clock_enable(CONSOLE_USART_GPIO_CLK);
	rcu_periph_clock_enable(CONSOLE_USART_CLK);

	gpio_af_set(CONSOLE_USART_GPIO_PORT, CONSOLE_USART__AF, CONSOLE_USART_TX_PIN);
	gpio_af_set(CONSOLE_USART_GPIO_PORT, CONSOLE_USART__AF, CONSOLE_USART_RX_PIN);

	/* configure USART0 TX as alternate function push-pull */
	gpio_mode_set(CONSOLE_USART_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, CONSOLE_USART_TX_PIN);
	gpio_output_options_set(CONSOLE_USART_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, CONSOLE_USART_TX_PIN);

	/* configure USART0 RX as alternate function push-pull */
	gpio_mode_set(CONSOLE_USART_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, CONSOLE_USART_RX_PIN);
	gpio_output_options_set(CONSOLE_USART_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, CONSOLE_USART_RX_PIN);

	/* USART configure */
	usart_deinit(CONSOLE_USART);
	usart_baudrate_set(CONSOLE_USART, CONSOLE_BAND_RATE);
	usart_receive_config(CONSOLE_USART, USART_RECEIVE_ENABLE);
	usart_transmit_config(CONSOLE_USART, USART_TRANSMIT_ENABLE);
	usart_enable(CONSOLE_USART);
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    (void)f;
    SetKT38();
    usart_data_transmit(CONSOLE_USART, (uint8_t)ch);
    while(RESET == usart_flag_get(CONSOLE_USART, USART_FLAG_TBE));
    ResetKT38();
    return ch;
}

//__asm(".global __use_no_semihosting");

/* Поскольку режим полухостинга не используется, некоторые функции в стандартной библиотеке C stdio.h, которые используют полухостинг, необходимо переписать */

//#pragma -Warmcc-pragma-import(__use_no_semihosting)  // Убедитесь, что функции полухостинга не используются из ссылки библиотеки C

// void _sys_exit(int x)    // Определить _sys_exit (), чтобы избежать использования режима полухостинга
//{ x = x; }   

/*void _sys_exit(int return_code)
{
    label: goto label;
}

void _ttywrch(int c)
{
  fputc(c, stdout); // stdout
  fflush(stdout);
}

int _sys_open(const char * sName, int OpenMode)
{
  (void)OpenMode;
  return (0);  // Not implemented
}
*/
/*
struct __FILE  // Поддержка функций, требуемых стандартной библиотекой

{ 

     int handle; 

}; 
*/
/* FILE is typedef’ d in stdio.h. */ 

//FILE __stdout;   

#elif DBG_CONSOLE == DBG_CONSOLE_RTT
//#include "RTT/SEGGER_RTT.h"
#include "../../RTT/SEGGER_SYSVIEW.h"
#elif DBG_CONSOLE == DBG_CONSOLE_SWO

#if __ARMCC_VERSION >= 6000000
__asm(".global __use_no_semihosting");
#elif __ARMCC_VERSION >= 5000000
#pragma import(__use_no_semihosting)
#else
#error Unsupported compiler
#endif

/*#define ITM_Port8(n)    (*((volatile unsigned char *)(0xE0000000+4*n)))
#define ITM_Port16(n)   (*((volatile unsigned short*)(0xE0000000+4*n)))
#define ITM_Port32(n)   (*((volatile unsigned long *)(0xE0000000+4*n)))
#define DEMCR           (*((volatile unsigned long *)(0xE000EDFC)))
#define TRCENA          0x01000000

//struct __FILE { int handle; };
//FILE __stdout;
//FILE __stdin;

int fputc(int ch, FILE *f)
{
    //SetKT38();
    if (DEMCR & TRCENA)
    {
        while (ITM_Port32(0) == 0);
        ITM_Port8(0) = ch;
    }
    //ResetKT38();
    return(ch);
}*/


#include "rt_sys.h"
#include "rt_misc.h"
#include "time.h"

const char __stdin_name[] = ":tt";
const char __stdout_name[] = ":tt";
const char __stderr_name[] = ":tt";

FILEHANDLE _sys_open(const char *name, int openmode)
{
  return 1;
}

int _sys_close(FILEHANDLE fh)
{
  return 0;
}

char *_sys_command_string(char *cmd, int len)
{
  return NULL;
}

int _sys_write(FILEHANDLE fh, const unsigned char *buf, unsigned len, int mode)
{
  return 0;
}

int _sys_read(FILEHANDLE fh, unsigned char *buf, unsigned len, int mode)
{
  return -1;
}

void _ttywrch(int ch)
{}

int _sys_istty(FILEHANDLE fh)
{
  return 0;
}

int _sys_seek(FILEHANDLE fh, long pos)
{
  return -1;
}

long _sys_flen(FILEHANDLE fh)
{
  return -1;
}

void _sys_exit(int return_code)\
{
  while (1)
  ;
}

clock_t clock(void)
{
  clock_t tmp;
  return tmp;
}

void _clock_init(void)
{}

time_t time(time_t *timer)
{
  time_t tmp;
  return tmp;
}

int system(const char *string)
{
  return 0;
}

char *getenv(const char *name)
{
  return NULL;
}

void _getenv_init(void)
{}

#endif

void dbg_console_init(void)
{
#if DBG_CONSOLE == DBG_CONSOLE_USART
    console_usart_init();
#elif DBG_CONSOLE == DBG_CONSOLE_RTT
    //SEGGER_RTT_Init();
    SEGGER_SYSVIEW_Conf();
    //SEGGER_RTT_WriteString(0, "Hello World from SEGGER!\r\n");
#elif DBG_CONSOLE == DBG_CONSOLE_SWO

#endif
}
