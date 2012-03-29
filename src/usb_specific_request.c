/*This file has been prepared for Doxygen automatic documentation generation.*/
//! \file *********************************************************************
//!
//! \brief user call-back functions
//!
//!  This file contains the user call-back functions corresponding to the
//!  application:
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


//_____ I N C L U D E S ____________________________________________________

#include "config.h"
#include "conf_usb.h"
#include "lib_mcu/usb/usb_drv.h"
#include "usb_descriptors.h"
#include "modules/usb/device_chap9/usb_standard_request.h"
#include "usb_specific_request.h"
#include "lib_mcu/uart/uart_lib.h"
#if ((USB_DEVICE_SN_USE==ENABLE) && (USE_DEVICE_SN_UNIQUE==ENABLE))
#include "lib_mcu/flash/flash_drv.h"
#endif
#include "my_interrupt.h"

//_____ D E F I N I T I O N ________________________________________________

/*
#ifdef __GNUC__
extern PGM_VOID_P pbuffer;
#else
extern U8   code *pbuffer;
#endif
extern U8   data_to_transfer;
*/
S_line_coding   line_coding;
S_line_status line_status;      // for detection of serial state input lines


// We buffer the old state as it is wize only to send this interrupt message if
// sstate has changed.
S_serial_state serial_state;         // actual state
static S_serial_state serial_state_saved;   // buffered previously sent state
volatile U8 usb_request_break_generation=FALSE;


extern const S_usb_hid_report_descriptor_mouse usb_hid_report_descriptor_mouse;

uint8_t jump_bootloader = 0;

uint8_t g_u8_report_rate=0;


//_____ D E C L A R A T I O N ______________________________________________

void hid_get_report_descriptor(void);
void usb_hid_set_report_output(void);
void usb_hid_set_idle (U8 u8_report_id, U8 u8_duration );
void usb_hid_get_idle (U8 u8_report_id);
void hid_get_hid_descriptor(void);
void usb_hid_set_report_feature(void);
void hid_report_transfer(
  uint16_t data_to_transfer,
#ifdef __GNUC__
  PGM_VOID_P pbuffer
#else
  uint8_t    code *pbuffer
#endif
  );

//! @breif This function checks the specific request and if known then processes it
//!
//! @param type      corresponding at bmRequestType (see USB specification)
//! @param request   corresponding at bRequest (see USB specification)
//!
//! @return TRUE,  when the request is processed
//! @return FALSE, if the request is'nt know (STALL handshake is managed by the main standard request function).
//!
Bool usb_user_read_request(U8 type, U8 request)
{
   U16 wValue;

   LSB(wValue) = Usb_read_byte();
   MSB(wValue) = Usb_read_byte();

   //** Specific request from Class CDC
   if( USB_SETUP_SET_CLASS_INTER == type )
   {
      switch( request )
      {
         case SETUP_CDC_SET_LINE_CODING:
         cdc_set_line_coding();
         return TRUE;
         break;
   
         case SETUP_CDC_SET_CONTROL_LINE_STATE:
         cdc_set_control_line_state(wValue); // according cdc spec 1.1 chapter 6.2.14
         return TRUE;
         break;
   
         case SETUP_CDC_SEND_BREAK:
         cdc_send_break(wValue);             // wValue contains break lenght
         return TRUE;
         break;
      }
   }
   if( USB_SETUP_GET_CLASS_INTER == type )
   {
      switch( request )
      {
         case SETUP_CDC_GET_LINE_CODING:
         cdc_get_line_coding();
         return TRUE;
         break;
      }
   }

   //** Specific request from the Class HID (Mouse / Game pad)
   if( USB_SETUP_GET_STAND_INTERFACE == type )
   {
     switch( request )
     {
       case SETUP_GET_DESCRIPTOR :
       switch( MSB(wValue) ) // Descriptor ID
       {
         case DESCRIPTOR_HID :
           hid_get_hid_descriptor();
           return TRUE;
           break;
         case DESCRIPTOR_REPORT :
           hid_get_report_descriptor();
           return TRUE;

         case DESCRIPTOR_PHYSICAL:
           // TODO
           break;
       } 
       break;
     }

   }
   if( USB_SETUP_SET_CLASS_INTER == type )
   {
     switch( request )
     {
       case SETUP_HID_SET_REPORT :
         // The MSB wValue field specifies the Report Type
         // The LSB wValue field specifies the Rep
         switch (MSB(wValue))
         {
           case REPORT_TYPE_INPUT :
             // TODO
             break;
           case REPORT_TYPE_OUTPUT : 
             usb_hid_set_report_output();
             return TRUE;
             break;
           case REPORT_TYPE_FEATURE:
             usb_hid_set_report_feature();
             return TRUE;
             break;
         }
         break;
         
       case SETUP_HID_SET_IDLE:
         usb_hid_set_idle(LSB(wValue), MSB(wValue));
         return TRUE;

       case SETUP_HID_SET_PROTOCOL:
         // TODO
         break;
     }
   }

   if( USB_SETUP_GET_CLASS_INTER == type )
   {
     switch( request ) 
     {
       case SETUP_HID_GET_REPORT :
         // TODO
         break;
       case SETUP_HID_GET_IDLE :
         usb_hid_get_idle(LSB(wValue));
         break;
       case SETUP_HID_GET_PROTOCOL :
         // TODO
         break;
     }
   }

   return FALSE;  // No supported request
}


//! @brief This function configures the endpoints
//!
//! @param conf_nb configuration number choosed by USB host
//!
void usb_user_endpoint_init(U8 conf_nb)
{
  uint8_t int_state = Get_interrupt_state();
  Disable_interrupt();
  /****** Atomic *******/
  usb_configure_endpoint(INT_EP,        \
                         TYPE_INTERRUPT,\
                         DIRECTION_IN,  \
                         SIZE_32,       \
                         ONE_BANK,      \
                         NYET_ENABLED);

  usb_configure_endpoint(TX_EP,         \
                         TYPE_BULK,     \
                         DIRECTION_IN,  \
                         SIZE_64,       \
                         TWO_BANKS,     \
                         NYET_ENABLED);

  usb_configure_endpoint(RX_EP,         \
                         TYPE_BULK,     \
                         DIRECTION_OUT, \
                         SIZE_64,       \
                         TWO_BANKS,     \
                         NYET_ENABLED);

#if 0
  usb_configure_endpoint(MOUSE_EP,      \
                         TYPE_INTERRUPT,\
                         DIRECTION_IN,  \
                         MOUSE_EP_SIZE, \
                         ONE_BANK,      \
                         NYET_ENABLED);

#endif
  Usb_reset_endpoint(INT_EP);
  Usb_reset_endpoint(TX_EP);
  Usb_reset_endpoint(RX_EP);
  //Usb_reset_endpoint(MOUSE_EP);

  Usb_select_endpoint(RX_EP);
  Usb_enable_receive_out_interrupt();

  // It will only fire when the host requests something on the line
  // So it will basically fire once, then we ack it, until the modem connects
  Usb_select_endpoint(TX_EP);
  Usb_enable_in_ready_interrupt();

//  Usb_select_endpoint(MOUSE_EP);
 // Usb_enable_in_ready_interrupt();

  if(int_state != 0) Enable_interrupt();
}


//! @brief This function returns the interface alternate setting
//!
//! @param wInterface         Interface selected
//!
//! @return alternate setting configurated
//!
U8   usb_user_interface_get( U16 wInterface )
{
   return 0;  // Only one alternate setting possible for all interface
}


//! @brief This function selects (and resets) the interface alternate setting
//!
//! @param wInterface         Interface selected
//! @param alternate_setting  alternate setting selected
//!
void usb_user_interface_reset(U16 wInterface, U8 alternate_setting)
{  
   // default setting selected = reset data toggle
   if( INTERFACE0_NB == wInterface )
   {
      // Interface CDC ACM Com
      Usb_select_endpoint(INT_EP);
      Usb_disable_stall_handshake();
      Usb_reset_endpoint(INT_EP);
      Usb_reset_data_toggle();
   }
   if( INTERFACE1_NB == wInterface )
   {
      // Interface CDC ACM Data
      Usb_select_endpoint(TX_EP);
      Usb_disable_stall_handshake();
      Usb_reset_endpoint(TX_EP);
      Usb_reset_data_toggle();
      Usb_select_endpoint(RX_EP);
      Usb_disable_stall_handshake();
      Usb_reset_endpoint(RX_EP);
      Usb_reset_data_toggle();
   }
   if( INTERFACE_NB_MOUSE == wInterface )
   {
     // Interface Mouse 
     Usb_select_endpoint(MOUSE_EP);
     Usb_disable_stall_handshake();
     Usb_reset_endpoint(MOUSE_EP);
     Usb_reset_data_toggle();
   }
}


//! This function fills the global descriptor
//!
//! @param type      corresponding at MSB of wValue (see USB specification)
//! @param string    corresponding at LSB of wValue (see USB specification)
//!
//! @return FALSE, if the global descriptor no filled
//!
Bool usb_user_get_descriptor(U8 type, U8 string)
{ 
   return FALSE;
}

//! cdc_get_line_coding.
//!
//! @brief This function manages reception of line coding parameters (baudrate...).
//!
//! @param none
//!
//! @return none
//!
void cdc_get_line_coding(void)
{
     Usb_ack_receive_setup();
     Usb_write_byte(LSB0(line_coding.dwDTERate));
     Usb_write_byte(LSB1(line_coding.dwDTERate));
     Usb_write_byte(LSB2(line_coding.dwDTERate));
     Usb_write_byte(LSB3(line_coding.dwDTERate));
     Usb_write_byte(line_coding.bCharFormat);
     Usb_write_byte(line_coding.bParityType);
     Usb_write_byte(line_coding.bDataBits);

     Usb_send_control_in();
     while(!(Is_usb_read_control_enabled()));
     //Usb_clear_tx_complete();

   while(!Is_usb_receive_out());
   Usb_ack_receive_out();
}


//! cdc_set_line_coding.
//!
//! @brief This function manages reception of line coding parameters (baudrate...).
//!
//! @param none
//!
//! @return none
//!
void cdc_set_line_coding (void)
{
   Usb_ack_receive_setup();
   while (!(Is_usb_receive_out()));
   LSB0(line_coding.dwDTERate) = Usb_read_byte();
   LSB1(line_coding.dwDTERate) = Usb_read_byte();
   LSB2(line_coding.dwDTERate) = Usb_read_byte();
   LSB3(line_coding.dwDTERate) = Usb_read_byte();
   line_coding.bCharFormat = Usb_read_byte();
   line_coding.bParityType = Usb_read_byte();
   line_coding.bDataBits = Usb_read_byte();
     Usb_ack_receive_out();

     Usb_send_control_in();                // send a ZLP for STATUS phase
     while(!(Is_usb_read_control_enabled()));
#ifdef UART_U2
   Uart_set_baudrate((line_coding.dwDTERate)/2);
#else
   Uart_set_baudrate(line_coding.dwDTERate);
#endif
}


//! cdc_set_control_line_state.
//!
//! @brief This function manages the SET_CONTROL_LINE_LINE_STATE CDC request.
//!
//! @todo Manages here hardware flow control...
//!
//! @param none
//!
//! @return none
//!
void cdc_set_control_line_state (U16 state)
{
     Usb_ack_receive_setup();
   Usb_send_control_in();
   line_status.all = state;
   
   while(!(Is_usb_read_control_enabled()));

}

//! cdc_update_serial_state.
//!
//! @brief This function checks if serial state has changed and updates host with that information.
//!
//! @todo Return TRUE only if update was accepted by host, to detect need for resending
//!
//! @param none
//!
//! @return TRUE if updated state was sent otherwise FALSE
//!
//! @comment upr: Added for hardware handshake support according cdc spec 1.1 chapter 6.3.5
//!
Bool cdc_update_serial_state()
{
   if( serial_state_saved.all != serial_state.all)
   {
      serial_state_saved.all = serial_state.all;
      
      Usb_select_endpoint(INT_EP);
      if (Is_usb_write_enabled())
      {
         Usb_write_byte(USB_SETUP_GET_CLASS_INTER);   // bmRequestType
         Usb_write_byte(SETUP_CDC_BN_SERIAL_STATE);   // bNotification
         
         Usb_write_byte(0x00);   // wValue (zero)
         Usb_write_byte(0x00);
         
         Usb_write_byte(0x00);   // wIndex (Interface)
         Usb_write_byte(0x00);
         
         Usb_write_byte(0x02);   // wLength (data count = 2)
         Usb_write_byte(0x00);
         
         Usb_write_byte(LSB(serial_state.all));   // data 0: LSB first of serial state
         Usb_write_byte(MSB(serial_state.all));   // data 1: MSB follows
         Usb_ack_in_ready();
      }
      return TRUE;
   }
   return FALSE;
}

//! @brief This function manages the SEND_BREAK CDC request.
//!
//! @todo Manages here hardware flow control...
//!
//! @param break lenght
//!
void cdc_send_break(U16 break_duration)
{
   Usb_ack_receive_setup();
    Usb_send_control_in();
   usb_request_break_generation=TRUE;
   while(!(Is_usb_read_control_enabled()));
}

void hid_get_report_descriptor(void){
  uint16_t data_to_transfer;
#ifdef __GNUC__
  PGM_VOID_P pbuffer;
#else
  uint8_t    code *pbuffer;
#endif
  uint16_t wInterface;

  LSB(wInterface) = Usb_read_byte();
  MSB(wInterface) = Usb_read_byte();

  //if(wInterface == INTERFACE_NB_MOUSE)
  //{
    data_to_transfer = sizeof(usb_hid_report_descriptor_mouse);
    pbuffer = &(usb_hid_report_descriptor_mouse.report[0]);
  //}
  // else some other HID device


  hid_report_transfer(data_to_transfer, pbuffer);
}

void usb_hid_set_report_output(void){
  Usb_ack_receive_setup();
  Usb_send_control_in();

  while(!Is_usb_receive_out());
  Usb_ack_receive_out();
  Usb_send_control_in();
}

void usb_hid_set_idle (U8 u8_report_id, U8 u8_duration ){
  uint16_t wInterface;

  LSB(wInterface) = Usb_read_byte();
  MSB(wInterface) = Usb_read_byte();
  Usb_ack_receive_setup();

  g_u8_report_rate = u8_duration;

  Usb_send_control_in();
  while(!Is_usb_in_ready());
}

void usb_hid_get_idle (U8 u8_report_id){
  U16 wLength;
  U16 wInterface;

  // Get interface number to put in idle mode
  LSB(wInterface)= Usb_read_byte();
  MSB(wInterface)= Usb_read_byte();
  LSB(wLength)   = Usb_read_byte();
  MSB(wLength)   = Usb_read_byte();
  Usb_ack_receive_setup();

  if( wLength != 0){
    Usb_write_byte(g_u8_report_rate);
    Usb_send_control_in();
  }

  while(!Is_usb_receive_out());
  Usb_ack_receive_out();
}

void hid_get_hid_descriptor(void){
  uint16_t data_to_transfer;
#ifdef __GNUC__
  PGM_VOID_P pbuffer;
#else
  uint8_t    code *pbuffer;
#endif
  uint16_t wInterface;

  LSB(wInterface) = Usb_read_byte();
  MSB(wInterface) = Usb_read_byte();

  if(wInterface == INTERFACE_NB_MOUSE)
  {
//    data_to_transfer = sizeof(usb_conf_desc.ep_hid_mouse);
    //pbuffer = &(usb_conf_desc.ep_hid_mouse.bLength);
  }
  // Else some other HID device you have
  hid_report_transfer(data_to_transfer, pbuffer);
}
void hid_report_transfer(
  uint16_t data_to_transfer,
#ifdef __GNUC__
  PGM_VOID_P pbuffer
#else
  uint8_t    code *pbuffer
#endif
  ){
  uint16_t wLength;
  uint8_t  nb_byte;
  bit      zlp=FALSE;

  LSB(wLength) = Usb_read_byte();
  MSB(wLength) = Usb_read_byte();
  Usb_ack_receive_setup();

  if(wLength > data_to_transfer)
  {
    if((data_to_transfer % EP_CONTROL_LENGTH) == 0) { zlp = TRUE;}
    else { zlp = FALSE; } // no need for zero length packet
  }
  else
  {
    data_to_transfer = (U8)wLength; // send only requested number of data
  }

  while((data_to_transfer != 0) && (!Is_usb_receive_out())){
    while(!Is_usb_read_control_enabled());

    nb_byte = 0;
    while(data_to_transfer != 0){
      if(nb_byte++ == EP_CONTROL_LENGTH) // check endpoint 0 size
        break;

#ifndef __GNUC__
      Usb_write_byte(*pbuffer++);
#else // AVRGCC does not support point to PGM space
      //warning with AVRGCC assumes devices descriptors are stored in the lower 64Kbytes of on-chip flash memory
      Usb_write_byte(pgm_read_byte_near((unsigned int)pbuffer++));
#endif
      data_to_transfer--;
    }
    Usb_send_control_in();
  }

  if(Is_usb_receive_out())
  {
    // abort from Host
    Usb_ack_receive_out();
    return;
  }
  if(zlp == TRUE){
    while(!Is_usb_read_control_enabled());
    Usb_send_control_in();
  }

  while(!Is_usb_receive_out());
  Usb_ack_receive_out();
}

// go to bootloader ???
void usb_hid_set_report_feature(void){
  Usb_ack_receive_setup();
  Usb_send_control_in();

  while(!Is_usb_receive_out());

  if(Usb_read_byte() == 0x55)
    if(Usb_read_byte() == 0xAA)
      if(Usb_read_byte() == 0x55)
        if(Usb_read_byte() == 0xAA)
          jump_bootloader = 1;

  Usb_ack_receive_out();
  Usb_send_control_in();
  while(!Is_usb_in_ready());
}

ISR(USB_COM_vect){
  uint8_t mask;
  uint8_t ep;
  //interrupt_state_t interrupt_state;
  //my_interrupt_load_state(&interrupt_state);
  mask = Usb_interrupt_flags();
  ep = NB_ENDPOINTS;
  // wow no idea why doing a countdown works, but some comment says that
  // the Setup request may reset endpoints so we need to take care of that one last
  while(ep-->0){
    if((mask & (1 << ep)) != 0){
      Usb_select_endpoint(ep);
      switch(ep){
        case 0: Usb_ep0_int_action(); break;
        case 1: Usb_ep1_int_action(); break;
        case 2: Usb_ep2_int_action(); break;
        case 3: Usb_ep3_int_action(); break;
        case 4: Usb_ep4_int_action(); break;
        case 5: Usb_ep5_int_action(); break;
        case 6: Usb_ep6_int_action(); break;
        default: break;
      }
    }
  }
  //my_interrupt_load_state(&interrupt_state);
}
