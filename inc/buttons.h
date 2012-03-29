#ifndef _BUTTONS_H_
#define _BUTTONS_H_

typedef struct {
    unsigned int state:1;   /* The debounced state */ 
    unsigned int toggled:1; /* Was the button toggled */
    unsigned int state_r:1; /* The raw state (no debouncing) */
    unsigned int state_p:1; /* The past raw state (no debouncing) */
    uint8_t debounce_val; /* the value the raw state needs to count up to before it is considered debounced. */
    uint8_t count;        /* The number of times the raw state was its current vlaue */
}button_t;

extern button_t button_joy_up;
extern button_t button_joy_down;
extern button_t button_joy_right;
extern button_t button_joy_left;
extern button_t button_joy_mid;
extern button_t button_hwb;

void buttons_init(uint8_t debounce_val);
void buttons_read();


#endif /* _BUTTONS_H_ */
