#include "lib_mcu/compiler.h"
#include "lib_board/usb_key/usb_key.h"

#include "buttons.h"
#include "timer.h"
#include "modules/scheduler/scheduler.h"

#include "lib_mcu/wdt/wdt_drv.h"
#include "lib_mcu/power/power_drv.h"

#include "modules/usb/usb_task.h"
#include "lib_mcu/usb/usb_drv.h"
#include "modules/usb/device_chap9/usb_standard_request.h"
//#include "cdc_task.h"
#include "circ_buffer.h"
#include "my_uart_usb_lib.h"
#include "device_mouse_task.h"

#include "my_interrupt.h"

#include <stdio.h>

uint16_t timer0_ovf_counter = 0;
uint32_t useconds = 0;
uint32_t seconds = 0;

ISR(TIMER0_OVF_vect){
  interrupt_state_t interrupt_state;
  my_interrupt_load_state(&interrupt_state);

  // runs at about 492Hz with the 8MHz X-Tal
  timer0_ovf_counter += 1;
  useconds += 2032 + (timer0_ovf_counter & 1); //2032.5 usecs per tick
  if(timer0_ovf_counter >= 492){
    seconds += 1;
    useconds = 0;
    timer0_ovf_counter = 0;
    Led1_toggle(); //toggle one of the led pins
    //printf("%u\r", (unsigned int) seconds);
  }
  scheduler(seconds, useconds);

  my_interrupt_load_state(&interrupt_state);
}

int buttons_read_task(uint32_t seconds, uint32_t useconds){
  (void) seconds;
  (void) useconds;

  buttons_read();

  return 0;
}

int main_task(uint32_t seconds, uint32_t useconds){
  char c;
  if(button_hwb.state == 1)
    Led2_on();
  else
    Led2_off();

  if(button_joy_left.toggled == 1 && button_joy_left.state == 1)   printf("Button Left\r");
  if(button_joy_right.toggled == 1 && button_joy_right.state == 1) printf("Button Right\r");
  if(button_joy_up.toggled == 1 && button_joy_up.state == 1) printf("Button Up\r");
  if(button_joy_down.toggled == 1 && button_joy_down.state == 1) printf("Button Down\r");
  if(button_joy_mid.toggled == 1 && button_joy_mid.state == 1) printf("Button Mid\r");
  if(button_hwb.toggled == 1 && button_hwb.state == 1) printf("Button HWB\r");

  c = my_uart_usb_getchar();
  if( c != EOF)
    my_uart_usb_putchar(c);


  // this interrupt happense about once every 2 ms
  //device_mouse_task_by_int();

  return 0;
}



void my_usb_task_init(){
   #if (USE_USB_PADS_REGULATOR==ENABLE)  // Otherwise assume USB PADs regulator is not used
   Usb_enable_regulator();
   #endif
   Usb_force_device_mode();

   Usb_disable();
   Usb_enable();
   Usb_select_device();
   Usb_enable_vbus_interrupt();
   Usb_enable_reset_interrupt();

}

void my_usb_reset_action(void){
  Usb_reset_endpoint(0);
  usb_configuration_nb=0;
  // after reset these get set to 0
  Usb_select_endpoint(EP_CONTROL);
  Usb_enable_receive_setup_interrupt();
}



int main(void)
{
  // disable watchdog timer
  wdtdrv_disable();
  // Clock is prescaled by 2^{-CLKPR}
  // see why this is important in chapter 6.9 of datasheet
  Set_cpu_prescaler(0);

  Leds_init();

  // buttons are looked at in a loop that runs at 500MHz
  // wait only 10 clicks before registering
  buttons_init(10);

  my_usb_task_init();
  my_uart_usb_init();
  device_mouse_task_init();

  scheduler_add_task(buttons_read_task);
  scheduler_add_task(main_task);

  // only enable the overflow interrupt
  timer0_init(1, 0, 0, 0, 0);

  Enable_interrupt();

  while(1){
    ;
  }
  Led3_on();

  return 0;
}
