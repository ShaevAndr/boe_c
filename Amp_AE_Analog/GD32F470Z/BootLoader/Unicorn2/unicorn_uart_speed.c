
#include "unicorn_uart_speed.h"

int unicorn_uart_speed_to_baudrate(int unicorn_uart_speed)
{
  switch (unicorn_uart_speed)
  {
  case _speed9600:    return 9600;
  case _speed19200:   return 19200;
  case _speed38400:   return 38400;
  case _speed57600:   return 57600;
  case _speed115200:  return 115200;
  case _speed345600:  return 345600;
  case _speed1036800: return 1036800;
  case _speed230400:  return 230400;
  case _speed460800:  return 460800;
  case _speed921600:  return 921600;
  default: break;
  }
  return 0;
}
