
#ifndef UNICORN_UART_SPEED_H
#define UNICORN_UART_SPEED_H

//#define _SpeedsNum    7
//#define _SpeedsNum    5
#define _speed9600    0
#define _speed19200   1
#define _speed38400   2
#define _speed57600   3
#define _speed115200  4
#define _speed345600  5
#define _speed1036800 6
#define _speed230400  7
#define _speed460800  8
#define _speed921600  9

#define _speedMaxNum  _speed921600

int unicorn_uart_speed_to_baudrate(int unicorn_uart_speed);

#endif // UNICORN_UART_SPEED_H
