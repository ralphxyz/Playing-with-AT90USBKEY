#ifndef _MY_INTERRUPT_H_
#define _MY_INTERRUPT_H_

typedef struct{
  uint8_t usb_endpoint;
}interrupt_state_t;

void my_interrupt_save_state(interrupt_state_t *state);
void my_interrupt_load_state(interrupt_state_t *state);

#endif
