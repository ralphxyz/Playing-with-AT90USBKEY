#include "config.h"
#include "conf_usb.h"
#include "my_cdc_task.h"
#include "lib_mcu/usb/usb_drv.h"
#include "usb_descriptors.h"
#include "modules/usb/device_chap9/us/standard_request.h"
#include "usb_specific_request.h"
#include "uart_usb_lib.h"
#include <stdio.h>

volatile        U8 cpt_sof;
extern          U8 rx_counter;
extern          U8 tx_counter;
extern volatile U8 usb_request_break_generation;

S_line_coding  line_coding;
S_line_status  line_status;
S_serial_state serial_state;
