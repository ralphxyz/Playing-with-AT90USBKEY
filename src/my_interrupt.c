#include "lib_mcu/compiler.h"

#include "my_interrupt.h"
#include "lib_mcu/usb/usb_drv.h"
void my_interrupt_save_state(interrupt_state_t *state){
  state->usb_endpoint = Usb_get_selected_endpoint();
  return;
}
void my_interrupt_load_state(interrupt_state_t *state){
  Usb_select_endpoint(state->usb_endpoint);
  return;
}
