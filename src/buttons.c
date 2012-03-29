#include <avr/io.h>
#include "lib_mcu/compiler.h"
#include "lib_board/usb_key/usb_key.h"
#include "buttons.h"

button_t button_joy_up;
button_t button_joy_down;
button_t button_joy_right;
button_t button_joy_left;
button_t button_joy_mid;
button_t button_hwb;

void button_init(button_t *button, uint8_t debounce_val){
    button->state = 0;
    button->toggled = 0;
    button->state_r = 0;
    button->state_p = 0;
    button->debounce_val = debounce_val;
    button->count = 0;
}

void buttons_init(uint8_t debounce_val){
    Joy_init();
    Hwb_button_init();
    
    button_init(&button_joy_up, debounce_val);
    button_init(&button_joy_down, debounce_val);
    button_init(&button_joy_right, debounce_val);
    button_init(&button_joy_left, debounce_val);
    button_init(&button_joy_mid, debounce_val);
    button_init(&button_hwb, debounce_val);
}

void button_read(button_t *button, uint8_t val){
    button->state_p = button->state_r;
    button->state_r = val;

    if(button->state_r == button->state_p)
        button->count = button->count + 1;
    else
        button->count = 0;

    button->toggled = 0;
    if(button->count >= button->debounce_val){
        button->toggled = button->state ^ button->state_r;
        button->state = button->state_r;
        button->count = 0;
    }
}

void buttons_read(){
  button_read(&button_joy_up, Is_joy_up());
  button_read(&button_joy_down, Is_joy_down());
  button_read(&button_joy_right, Is_joy_right());
  button_read(&button_joy_left, Is_joy_left());
  button_read(&button_joy_mid, Is_btn_middle());
  button_read(&button_hwb, Is_hwb());
}
