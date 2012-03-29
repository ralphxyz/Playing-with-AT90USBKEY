/*This file has been prepared for Doxygen automatic documentation generation.*/
//! \file *********************************************************************
//!
//! \brief This file manages the CDC task.
//!
//! - Compiler:           IAR EWAVR and GNU GCC for AVR
//! - Supported devices:  AT90USB1287, AT90USB1286, AT90USB647, AT90USB646
//!
//! \author               Atmel Corporation: http://www.atmel.com \n
//!                       Support and FAQ: http://support.atmel.no/
//!
//! ***************************************************************************

/* Copyright (c) 2007, Atmel Corporation All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of ATMEL may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//_____  I N C L U D E S ___________________________________________________

#include "config.h"
#include "conf_usb.h"
#include "cdc_task.h"
#include "lib_mcu/usb/usb_drv.h"
#include "lib_mcu/adc/adc_drv.h"
#include "usb_descriptors.h"
#include "modules/usb/device_chap9/usb_standard_request.h"
#include "usb_specific_request.h"
#include "lib_mcu/uart/uart_lib.h"
#include "uart_usb_lib.h"
#include <stdio.h>

extern S_line_status line_status;      // for detection of serial state input lines


//_____ M A C R O S ________________________________________________________



//_____ D E F I N I T I O N S ______________________________________________



//_____ D E C L A R A T I O N S ____________________________________________


volatile        U8 cpt_sof;
extern          U8    rx_counter;
extern          U8    tx_counter;
extern volatile U8 usb_request_break_generation;



//! @brief This function initializes the hardware ressources required for CDC demo.
//!
//!
//! @param none
//!
//! @return none
//!
//!/
void cdc_task_init(void)
{
   Usb_enable_sof_interrupt();
#ifdef USE_ADC
	init_adc();
#endif
#ifdef __GNUC__
   fdevopen((int (*)(char, FILE*))(uart_usb_putchar),(int (*)(FILE*))uart_usb_getchar); //for printf redirection
#endif
}



//! @brief Entry point of the uart cdc management
//!
//! This function links the uart and the USB bus.
//!
//! @param none
//!
//! @return none
void cdc_task(void)
{
   
  if(Is_device_enumerated() && line_status.DTR) //Enumeration processs OK and COM port openned ?
  {
    if (uart_usb_test_hit())   // Something received from the USB ?
    {
      while (rx_counter)
      {
        U8 temp = uart_usb_getchar();
        if(temp == 't'){
          S16 tmpC = Read_temperature();
          printf("%d\r\n",tmpC);
       }
        uart_usb_putchar(temp);
        Led1_toggle();
      }
    }
    if ( cpt_sof>=REPEAT_KEY_PRESSED)   //Debounce joystick events
    {
      if (Is_btn_middle()) printf ("Select Pressed !\r");
      if (Is_joy_right())  printf ("Right Pressed !\r");
      if (Is_joy_left())   printf ("Left Pressed !\r");
      if (Is_joy_down())   printf ("Down Pressed !\r");
      if (Is_joy_up())     printf ("Up Pressed !\r");
      if(Is_btn_left())    printf("Hello from AT90USBXXX !\r");
    }
    /*
    cdc_update_serial_state();

    if(usb_request_break_generation==TRUE)
    {
      usb_request_break_generation=FALSE;
      Led2_toggle();
    }
    */
  }
}

//! @brief sof_action
//!
//! This function increments the cpt_sof counter each times
//! the USB Start Of Frame interrupt subroutine is executed (1ms)
//! Usefull to manage time delays
//!
//! @warning Code:?? bytes (function code length)
//!
//! @param none
//!
//! @return none
void sof_action()
{
   cpt_sof++;
}

