#include "lib_mcu/compiler.h"
#include "lib_board/usb_key/usb_key.h"
#include "circ_buffer.h"
#include "lib_mcu/usb/usb_drv.h"
#include "usb_descriptors.h"
#include "usb_specific_request.h"
#include "my_uart_usb_lib.h"
#include <stdio.h> // for EOF


// usb_specific_request.c
extern S_line_status line_status;      // for detection of serial state input lines
extern volatile U8 usb_request_break_generation;

#define RX_BUFFER_SIZE CIRC_BUFFER_SIZE
#define TX_BUFFER_SIZE CIRC_BUFFER_SIZE
circ_buffer_t my_uart_usb_rx_buffer;
circ_buffer_t my_uart_usb_tx_buffer;

void my_usb_ep0_int_action(void){
  Usb_select_endpoint(EP_CONTROL);
  if (Is_usb_receive_setup() && Is_usb_receive_setup_interrupt_enabled()){
    usb_process_request();
  }
}


uint8_t my_uart_usb_num_to_read(void){
  uint8_t num=0;

  if(!Is_device_enumerated())
     return 0;

  Usb_select_endpoint(RX_EP);
  if (Is_usb_receive_out()){
    num = Usb_byte_counter();
    if (num == 0)
      Usb_ack_receive_out();
  }
  return num;
}

void my_uart_usb_read_from_endpoint(void){
  register uint8_t data_rx;
  uint8_t num_to_read;

  num_to_read = my_uart_usb_num_to_read();
  // if nothing to do, then do nothing
  if(num_to_read == 0)
    return;

  Usb_select_endpoint(RX_EP);
  // read them all
  while(num_to_read != 0){
    // check if full
    if(my_uart_usb_rx_buffer.num == my_uart_usb_rx_buffer.size)
      break;

    // not full so read (this is the only thing that would make it more full
    data_rx = Usb_read_byte();
    circ_buffer_put_byte(&my_uart_usb_rx_buffer, data_rx);
    num_to_read --; 
  }
  // if we read the whole thing, ack
  if(num_to_read == 0){
    Usb_select_endpoint(RX_EP);
    Usb_ack_receive_out();
  }
}

void my_usb_ep2_int_action(void){
  //uint8_t int_state = Get_interrupt_state();
  //Disable_interrupt();

  Usb_select_endpoint(RX_EP);
  if(Is_usb_receive_out() &&  Is_usb_receive_out_interrupt_enabled()){
    my_uart_usb_read_from_endpoint();
  }
  //if(int_state != 0) Enable_interrupt();
}
// this function should be continuously called
void my_uart_usb_send_to_endpoint(void){
  static uint8_t zlp=FALSE; /* zero load packet */
  static uint8_t local_buffer[TX_EP_SIZE];
  uint16_t num_read;
  bit ret;
  if(!Is_device_enumerated())
    return;


  // make atomic, this will be fixed by interrupts
  Usb_select_endpoint(TX_EP);
  if(Is_usb_write_enabled()==FALSE) // Only if endpoint ready
    return;

  if(line_status.DTR == 0){
    Usb_ack_in_ready();
    return;
  }


  // we send in chunks of TX_EP_SIZE 
  num_read = TX_EP_SIZE;
  ret = circ_buffer_get_nbytes(&my_uart_usb_tx_buffer, local_buffer, &num_read);
  if(num_read == 0 && zlp == TRUE){
    Usb_ack_in_ready();
    zlp = FALSE;
    Usb_disable_in_ready_interrupt(); // Nothing left to send, disable this until next time
    return;
  }
  // if we got exactly TX_EP_SIZE bytes, we should send zlp the next time if we send 0
  if(ret == PASS){
    zlp = TRUE;
    //Usb_enable_in_ready_interrupt(); // We don't need to do this since this is inside an interrupt.
  }
  else{ // this is the second pass and we didn't get a full TX_EP_SIZE bytes so the buffer is not full
    zlp = FALSE;
  }

  // write to the endpoint
  uint16_t nb_data = 0;
  Usb_select_endpoint(TX_EP);
  while(nb_data < num_read)
  {
    Usb_write_byte(local_buffer[nb_data]);
    nb_data++;
  }
  Usb_ack_in_ready();

  cdc_update_serial_state(); // I really do not know what this does, it could have been an old artifact from usart usb

//  if(usb_request_break_generation == TRUE)
//   usb_request_break_generation = FALSE;
}
void my_usb_ep1_int_action(void){
 // uint8_t int_state = Get_interrupt_state();
  //Disable_interrupt();

  Usb_select_endpoint(TX_EP);
  if(Is_usb_in_ready() && Is_usb_in_ready_interrupt_enabled()){
    my_uart_usb_send_to_endpoint();
  }
//  if(int_state != 0) Enable_interrupt();
}

int my_uart_usb_putchar(int data_to_send){
  circ_buffer_put_byte(&my_uart_usb_tx_buffer, (uint8_t)data_to_send);
  return data_to_send;
}

char my_uart_usb_getchar(void){
  char c = EOF; // EOF
  circ_buffer_get_byte(&my_uart_usb_rx_buffer, (uint8_t *)&c);
  return c;
}


char my_uart_usb_getchar_wait(void){
  char c;
  while(circ_buffer_get_byte(&my_uart_usb_rx_buffer, (uint8_t *)&c));
  return c;
}


void my_uart_usb_init(void){
  circ_buffer_init(&my_uart_usb_rx_buffer);
  circ_buffer_init(&my_uart_usb_tx_buffer);
#ifdef __GNUC__
   fdevopen((int (*)(char, FILE*))(my_uart_usb_putchar),(int (*)(FILE*))my_uart_usb_getchar); //for printf redirection
#endif
}

