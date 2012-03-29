/*This file has been prepared for Doxygen automatic documentation generation.*/
//! \file *********************************************************************
//!
//! \brief CDC USB Identifers.
//!
//!  This file contains the usb parameters that uniquely identify the
//!  CDC application through descriptor tables.
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

#ifndef _USB_DESCRIPTORS_H_
#define _USB_DESCRIPTORS_H_

//_____ I N C L U D E S ____________________________________________________

#include "config.h"
#include "modules/usb/device_chap9/usb_standard_request.h"
#include "conf_usb.h"

//_____ M A C R O S ________________________________________________________

#define Usb_get_dev_desc_pointer()        (&(usb_dev_desc.bLength))
#define Usb_get_dev_desc_length()         (sizeof (usb_dev_desc))
#define Usb_get_conf_desc_pointer()       (&(usb_conf_desc.cfg.bLength))
#define Usb_get_conf_desc_length()        (sizeof (usb_conf_desc))

//_____ U S B    D E F I N E _______________________________________________

                  // USB Device descriptor
#define USB_SPECIFICATION     0x0200
//#define DEVICE_CLASS          CDC_GLOB_CLASS  // CDC class
#define DEVICE_CLASS          0      // each configuration has its own class
#define DEVICE_SUB_CLASS      0      // each configuration has its own sub-class
#define DEVICE_PROTOCOL       0      // each configuration has its own protocol
#define EP_CONTROL_LENGTH     64
#define VENDOR_ID             VID_ATMEL
#define PRODUCT_ID            PID_MegaHIDMouse
#define RELEASE_NUMBER        0x1000
#define MAN_INDEX             0x00
#define PROD_INDEX            0x00
#if (USB_DEVICE_SN_USE==ENABLE)
#define SN_INDEX              0x03
#else
#define SN_INDEX              0x00  // No serial number field
#endif
#define NB_CONFIGURATION      1

               // CDC CONFIGURATION
//#define NB_INTERFACE       2           // Number of interfaces
#define NB_INTERFACE       3           // Number of interfaces
#define CONF_NB            1
#define CONF_INDEX         0
#define CONF_ATTRIBUTES    USB_CONFIG_BUSPOWERED
#define MAX_POWER          50          // 100 mA (increments of 2 mA)


             // Interface 1 descriptor
#define INTERFACE1_NB        1
#define ALTERNATE1           0
#define NB_ENDPOINT1         2
#define INTERFACE1_CLASS     CDC_DATA_CLASS    // CDC ACM Data
#define INTERFACE1_SUB_CLASS CDC_DATA_SUBCLASS
#define INTERFACE1_PROTOCOL  CDC_DATA_PROTOCOL
#define INTERFACE1_INDEX     0


             // USB Endpoint 1 descriptor
             // Bulk IN  TX endpoint
#define TX_EP_SIZE          0x40
#define ENDPOINT_NB_1       USB_ENDPOINT_IN | TX_EP
#define EP_ATTRIBUTES_1     0x02          // BULK = 0x02, INTERUPT = 0x03
#define EP_SIZE_1           TX_EP_SIZE
#define EP_INTERVAL_1       0x00

             // USB Endpoint 2 descriptor
             //Bulk OUT  RX endpoint
#define RX_EP_SIZE          0x40
#define ENDPOINT_NB_2       RX_EP
#define EP_ATTRIBUTES_2     0x02          // BULK = 0x02, INTERUPT = 0x03
#define EP_SIZE_2           RX_EP_SIZE
#define EP_INTERVAL_2       0x00

             // Interface 0 descriptor
#define INTERFACE0_NB        0
#define ALTERNATE0           0
#define NB_ENDPOINT0         1
#define INTERFACE0_CLASS     CDC_COMM_CLASS    // CDC ACM Com
#define INTERFACE0_SUB_CLASS CDC_COMM_SUBCLASS
#define INTERFACE0_PROTOCOL  CDC_COMM_PROTOCOL
#define INTERFACE0_INDEX     0

             // USB Endpoint 3 descriptor
             // Interrupt IN
#define INT_EP_SIZE          0x20
#define ENDPOINT_NB_3       USB_ENDPOINT_IN | INT_EP
#define EP_ATTRIBUTES_3     0x03          // BULK = 0x02, INTERUPT = 0x03
#define EP_SIZE_3           INT_EP_SIZE
#define EP_INTERVAL_3       0xFF //ms interrupt pooling from host


#define INTERFACE_NB_MOUSE          2
#define ALTERNATE_MOUSE             0
#define NB_ENDPOINT_MOUSE           1
#define INTERFACE_CLASS_MOUSE       HID_CLASS
#define INTERFACE_SUBCLASS_MOUSE    NO_SUBCLASS
#define INTERFACE_PROTOCOL_MOUSE    HID_PROTOCOL_MOUSE
#define INTERFACE_INDEX_MOUSE       0

#define MOUSE_EP_SIZE               8 
#define ENDPOINT_NB_4               (MOUSE_EP | USB_ENDPOINT_IN)
#define EP_ATTRIBUTES_4             0x03 // BULK = 0x02, INTERUPT = 0x03
#define EP_SIZE_4                   MOUSE_EP_SIZE
#define EP_INTERVAL_4               0x02 // Interrupt polling interval from host


#define DEVICE_STATUS         USB_DEVICE_STATUS_BUS_POWERED

#define LANG_ID               0x00




#define USB_MN_LENGTH         5
#define USB_MANUFACTURER_NAME \
{ Usb_unicode('A') \
, Usb_unicode('T') \
, Usb_unicode('M') \
, Usb_unicode('E') \
, Usb_unicode('L') \
}

#define USB_PN_LENGTH         16
#define USB_PRODUCT_NAME \
{ Usb_unicode('A') \
 ,Usb_unicode('V') \
 ,Usb_unicode('R') \
 ,Usb_unicode(' ') \
 ,Usb_unicode('U') \
 ,Usb_unicode('S') \
 ,Usb_unicode('B') \
 ,Usb_unicode(' ') \
 ,Usb_unicode('C') \
 ,Usb_unicode('D') \
 ,Usb_unicode('C') \
 ,Usb_unicode(' ') \
 ,Usb_unicode('D') \
 ,Usb_unicode('E') \
 ,Usb_unicode('M') \
 ,Usb_unicode('O') \
}

#define USB_SN_LENGTH         0x05
#define USB_SERIAL_NUMBER \
{ Usb_unicode('0') \
 ,Usb_unicode('0') \
 ,Usb_unicode('0') \
 ,Usb_unicode('0') \
 ,Usb_unicode('0') \
}

#define LANGUAGE_ID           0x0409


                  //! Usb Request
typedef struct
{
   U8      bmRequestType;        //!< Characteristics of the request
   U8      bRequest;             //!< Specific request
   U16     wValue;               //!< field that varies according to request
   U16     wIndex;               //!< field that varies according to request
   U16     wLength;              //!< Number of bytes to transfer if Data
}  S_UsbRequest;

                //! Usb Device Descriptor
typedef struct {
   U8      bLength;              //!< Size of this descriptor in bytes
   U8      bDescriptorType;      //!< DEVICE descriptor type
   U16     bscUSB;               //!< Binay Coded Decimal Spec. release
   U8      bDeviceClass;         //!< Class code assigned by the USB
   U8      bDeviceSubClass;      //!< Sub-class code assigned by the USB
   U8      bDeviceProtocol;      //!< Protocol code assigned by the USB
   U8      bMaxPacketSize0;      //!< Max packet size for EP0
   U16     idVendor;             //!< Vendor ID. ATMEL = 0x03EB
   U16     idProduct;            //!< Product ID assigned by the manufacturer
   U16     bcdDevice;            //!< Device release number
   U8      iManufacturer;        //!< Index of manu. string descriptor
   U8      iProduct;             //!< Index of prod. string descriptor
   U8      iSerialNumber;        //!< Index of S.N.  string descriptor
   U8      bNumConfigurations;   //!< Number of possible configurations
}  S_usb_device_descriptor;


          //! Usb Configuration Descriptor
typedef struct {
   U8      bLength;              //!< size of this descriptor in bytes
   U8      bDescriptorType;      //!< CONFIGURATION descriptor type
   U16     wTotalLength;         //!< total length of data returned
   U8      bNumInterfaces;       //!< number of interfaces for this conf.
   U8      bConfigurationValue;  //!< value for SetConfiguration resquest
   U8      iConfiguration;       //!< index of string descriptor
   U8      bmAttibutes;          //!< Configuration characteristics
   U8      MaxPower;             //!< maximum power consumption
}  S_usb_configuration_descriptor; /* size = 0x09 */


              //! Usb Interface Descriptor
typedef struct {
   U8      bLength;               //!< size of this descriptor in bytes
   U8      bDescriptorType;       //!< INTERFACE descriptor type
   U8      bInterfaceNumber;      //!< Number of interface
   U8      bAlternateSetting;     //!< value to select alternate setting
   U8      bNumEndpoints;         //!< Number of EP except EP 0
   U8      bInterfaceClass;       //!< Class code assigned by the USB
   U8      bInterfaceSubClass;    //!< Sub-class code assigned by the USB
   U8      bInterfaceProtocol;    //!< Protocol code assigned by the USB
   U8      iInterface;            //!< Index of string descriptor
}  S_usb_interface_descriptor; /* Size = 0x09 */


               //! Usb Endpoint Descriptor
typedef struct {
   U8      bLength;               //!< Size of this descriptor in bytes
   U8      bDescriptorType;       //!< ENDPOINT descriptor type
   U8      bEndpointAddress;      //!< Address of the endpoint
   U8      bmAttributes;          //!< Endpoint's attributes
   U16     wMaxPacketSize;        //!< Maximum packet size for this EP
   U8      bInterval;             //!< Interval for polling EP in ms
} S_usb_endpoint_descriptor; /* 0x07 */


               //! Usb Language Descriptor
typedef struct {
   U8      bLength;               //!< size of this descriptor in bytes
   U8      bDescriptorType;       //!< STRING descriptor type
   U16     wlangid;               //!< language id
}  S_usb_language_id;


//_____ U S B   M A N U F A C T U R E R   D E S C R I P T O R _______________


//struct usb_st_manufacturer
typedef struct {
   U8  bLength;               // size of this descriptor in bytes
   U8  bDescriptorType;       // STRING descriptor type
   U16 wstring[USB_MN_LENGTH];// unicode characters
} S_usb_manufacturer_string_descriptor;


//_____ U S B   P R O D U C T   D E S C R I P T O R _________________________


//struct usb_st_product
typedef struct {
   U8  bLength;               // size of this descriptor in bytes
   U8  bDescriptorType;       // STRING descriptor type
   U16 wstring[USB_PN_LENGTH];// unicode characters
} S_usb_product_string_descriptor;


//_____ U S B   S E R I A L   N U M B E R   D E S C R I P T O R _____________


#if (USB_DEVICE_SN_USE==ENABLE)
//struct usb_st_serial_number
typedef struct {
   U8  bLength;               // size of this descriptor in bytes
   U8  bDescriptorType;       // STRING descriptor type
#if (USE_DEVICE_SN_UNIQUE==ENABLE)

#else   
   U16 wstring[USB_SN_LENGTH];// unicode characters
#endif
} S_usb_serial_number;
#endif

/*_____ U S B   H I D   D E S C R I P T O R __________________________________*/

typedef struct {
  U8  bLength;               /* Size of this descriptor in bytes */
  U8  bDescriptorType;       /* HID descriptor type */
  U16 bscHID;                /* Binay Coded Decimal Spec. release */
  U8  bCountryCode;          /* Hardware target country */
  U8  bNumDescriptors;       /* Number of HID class descriptors to follow */
  U8  bRDescriptorType;      /* Report descriptor type */
  U16 wDescriptorLength;     /* Total length of Report descriptor */
} S_usb_hid_descriptor;

/*_____ U S B   C D C  D E S C R I P T O R __________________________________*/

typedef struct
{
   S_usb_configuration_descriptor cfg;
   S_usb_interface_descriptor     ifc0;
   U8 CS_INTERFACE[19];
   S_usb_endpoint_descriptor      ep3;
   S_usb_interface_descriptor     ifc1;
   S_usb_endpoint_descriptor      ep1;
   S_usb_endpoint_descriptor      ep2;
   S_usb_interface_descriptor     ifc_mouse;
   S_usb_hid_descriptor           hid_mouse;
   S_usb_endpoint_descriptor      ep_hid_mouse;
} S_usb_user_configuration_descriptor;

/*______ U S B   G A M E P A D   D E S C R I P T O R _________________________*/





#endif

