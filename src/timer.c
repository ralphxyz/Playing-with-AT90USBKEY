#include "lib_mcu/compiler.h"
#include "lib_board/usb_key/usb_key.h"
#include "timer.h"

void timer0_init(uint8_t toie0_en, uint8_t ocr0a, uint8_t ocie0a_en, uint8_t ocr0b, uint8_t ocie0b_en){

    TIMSK0 = 0; // Stop timer interrupts
    TCCR0B = 0; // Disable timer counter for now
    TCNT0 = 0; // Reset the timer

    /* Timer control register A
     * 7:6 COM0A event
     * 5:4 COM0B event
     * 1:0 WGM1, WGM0  generate waveform
     */

    /* Timer control register B
     * 7   Force output compare A
     * 6   Force output compare B
     * 3   WGM02 (see TCCRA)
     * 2:0 Clock select
     *      0: Timer stopped
     *      1: clk i/o (so scaling)
     *      2: clk/8
     *      3: clk/64
     *      4: clk/256
     *      5: clk/1024
     *      6: external clock, falling edge
     *      7: external clock, rising edge
     */
    TCCR0A = 0<<WGM00;
    TCCR0B = 3<<CS00; 
    //with Clock divide by 64, runs at about 492Hz with the 8MHz X-Tal

    OCR0A = ocr0a;
    OCR0B = ocr0b;

    TIMSK0 = (toie0_en & 1) << TOIE0 | 
       (ocie0a_en & 1) << OCIE0A |
       (ocie0b_en & 1) << OCIE0B;
}
