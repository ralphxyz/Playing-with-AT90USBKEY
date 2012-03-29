#ifndef _MY_UART_USB_LIB_H_
#define _MY_UART_USB_LIB_H_


void my_uart_usb_init(void);

int my_uart_usb_putchar(int data_to_send);
char my_uart_usb_getchar(void);
char my_uart_usb_getchar_wait(void);


#endif /* _MY_UART_USB_LIB_H_ */
