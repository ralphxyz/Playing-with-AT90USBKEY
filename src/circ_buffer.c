#include "lib_mcu/compiler.h"
#include "circ_buffer.h"

void circ_buffer_init(circ_buffer_t *b){
  b->start = 0;
  b->end = 0;
  b->num = 0;
  b->size = CIRC_BUFFER_SIZE;
}

bit circ_buffer_put_byte(circ_buffer_t *b, uint8_t byte){
  bit ret = PASS;
  uint8_t int_state = Get_interrupt_state();
  Disable_interrupt();

  if(b->num == b->size){
    ret=FAIL;
    goto end;
  }
  b->buf[b->end] = byte;
  b->end = b->end + 1;
  if(b->end == b->size)
    b->end = 0;
  b->num = b->num + 1;

end:
  if(int_state != 0)
    Enable_interrupt();
  return ret;
}

bit circ_buffer_get_byte(circ_buffer_t *b, uint8_t *byte){
  bit ret = PASS;
  uint8_t int_state = Get_interrupt_state();
  Disable_interrupt();

  if(b->num == 0){
    ret = FAIL;
    goto end;
  }

  *byte = b->buf[b->start];
  b->start = b->start + 1;
  if(b->start == b->size)
    b->start = 0;
  b->num = b->num - 1;

end:
  if(int_state != 0)
    Enable_interrupt();
  return ret;
}

bit circ_buffer_put_nbytes(circ_buffer_t *b, uint8_t *bytes, uint16_t *n){
  uint16_t n_actual = 0;
  bit ret = PASS;
  uint8_t int_state = Get_interrupt_state();
  Disable_interrupt();

  while(b->num != b->size &&
          n_actual < *n){
    b->buf[b->end] = bytes[n_actual];
    b->end = b->end + 1;
    if(b->end == b->size)
      b->end = 0;
    b->num = b->num + 1;

    n_actual++;
  }

  if(n_actual < *n){
    ret = FAIL;
  }

  *n = n_actual;
  if(int_state != 0)
    Enable_interrupt();
  return ret;
}
bit circ_buffer_get_nbytes(circ_buffer_t *b, uint8_t *bytes, uint16_t *n){
  uint16_t n_actual = 0;
  bit ret = PASS;
  uint8_t int_state = Get_interrupt_state();
  Disable_interrupt();

  while(b->num != 0 &&
          n_actual < *n){
    bytes[n_actual] = b->buf[b->start];
    b->start = b->start + 1;
    if(b->start == b->size)
      b->start = 0;
    b->num = b->num - 1;
    n_actual++;
  }
  if(n_actual < *n){
    ret = FAIL;
  }

  *n = n_actual;
  if(int_state != 0)
    Enable_interrupt();
  return ret;
}
