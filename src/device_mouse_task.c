#include "config.h"
#include "conf_usb.h"

#include "device_mouse_task.h"
#include "usb_descriptors.h"

#include "lib_mcu/usb/usb_drv.h"

#include "modules/usb/device_chap9/usb_standard_request.h"

// MACROS


#define Hid_mouse_report_up()             (g_hid_mouse_report[2] -= MOUSE_SPEED)
#define Hid_mouse_report_down()           (g_hid_mouse_report[2] += MOUSE_SPEED)
#define Hid_mouse_report_left()           (g_hid_mouse_report[1] -= MOUSE_SPEED)
#define Hid_mouse_report_right()          (g_hid_mouse_report[1] += MOUSE_SPEED)

#define Hid_mouse_report_scroll_up()      (g_hid_mouse_report[3] += MOUSE_SPEED)
#define Hid_mouse_report_scroll_down()    (g_hid_mouse_report[3] -= MOUSE_SPEED)

#define  Hid_mouse_report_click_left()    (g_hid_mouse_report[0] |= 0x01)
#define  Hid_mouse_report_click_right()   (g_hid_mouse_report[0] |= 0x02)
#define  Hid_mouse_report_click_middle()  (g_hid_mouse_report[0] |= 0x04)

#define  Hid_mouse_report_reset()         (g_hid_mouse_report[0]=0,g_hid_mouse_report[1]=0,g_hid_mouse_report[2]=0,g_hid_mouse_report[3]=0)


#define Is_mouse_left_event()             Is_joy_left()
#define Is_mouse_right_event()            Is_joy_right()
#define Is_mouse_up_event()               Is_joy_up()
#define Is_mouse_down_event()             Is_joy_down()



#define Is_mouse_scroll_up_event()        FALSE
#define Is_mouse_scroll_down_event()      FALSE



#define Is_mouse_click_left_event()       Is_btn_left()
#define Is_mouse_click_middle_event()     FALSE
#define Is_mouse_click_right_event()      Is_btn_middle()

static bit   is_device_mouse_event(void);

//! Used to flag a report ready to send
volatile Bool  g_b_send_report;
//! Used to flag a ack report ready to send
volatile Bool  g_b_send_ack_report;
//! Used to store the HID mouse report
volatile U8    g_hid_mouse_report[4];

void device_mouse_task_init(void){
  g_b_send_report = FALSE;
  g_b_send_ack_report = TRUE;
}

//! Task which links mouse events with the USB HID mouse device
//!
//! @verbatim
//! This task is interrupt handled : the function is called periodically from the ISR of Start of Frame reception
//! That makes possible moving the mouse while the mass storage is used (file copy for example)
//! @endverbatim
//!
void device_mouse_task_by_int(void){
  if(!Is_device_enumerated()){
    // Should not be in here
    Led3_on();
    return; // device not ready
  }

#if 0
  if(g_b_send_report == FALSE){
    // No report sending on going then check mouse event to eventually fill a new report
    if(is_device_mouse_event()){
      g_b_send_report = TRUE;
    }
  }

  if((!g_b_send_report) && (!g_b_send_ack_report))
    return; // No report and ack to send

#endif
  //** A report or ack must be sent
  Usb_select_endpoint(MOUSE_EP);
  if(!Is_usb_write_enabled()){
    return; // Endpoint not free
  }

  is_device_mouse_event(); // get the events

#if 0
  if( g_b_send_report == TRUE ){
    g_b_send_report = FALSE;
    // Send an ack after a "clic" report only
    g_b_send_ack_report = (0 != g_hid_mouse_report[0]);
  }
  else{
    Hid_mouse_report_reset(); // Reset report to have a ack report
    // I guess an ack report has to be full of 0s
    g_b_send_ack_report = FALSE;
  }
#endif

  // send report
  Usb_write_byte(g_hid_mouse_report[0]);
  Usb_write_byte(g_hid_mouse_report[1]);
  Usb_write_byte(g_hid_mouse_report[2]);
  Usb_write_byte(g_hid_mouse_report[3]);
  Usb_ack_in_ready();
}

void my_usb_ep4_int_action(void){
  Usb_select_endpoint(MOUSE_EP);
  if(Is_usb_in_ready() && Is_usb_in_ready_interrupt_enabled()){
    device_mouse_task_by_int();
  }
}

static bit is_device_mouse_event(void){
  Hid_mouse_report_reset();

  // Check click events
  if(Is_mouse_click_left_event()){
    Hid_mouse_report_click_left();
  }
  if(Is_mouse_click_middle_event()) {
    Hid_mouse_report_click_middle();
  }
  if(Is_mouse_click_right_event()) {
    Hid_mouse_report_click_right();
  }
  // Check Move events
  if (Is_mouse_left_event()) {
    Hid_mouse_report_left();
  }
  if (Is_mouse_right_event()) {
    Hid_mouse_report_right();
  }
  if (Is_mouse_up_event()) {
    Hid_mouse_report_up();
  }
  if (Is_mouse_down_event()) {
    Hid_mouse_report_down();
  }

  // Check report
  if((0==g_hid_mouse_report[0])
      && (0==g_hid_mouse_report[1])
      && (0==g_hid_mouse_report[2])
      && (0==g_hid_mouse_report[3]))
  {
    return FALSE;  // No event
  }
  return TRUE; // some events
}
