
# Project name
PROJECT=mark-first

# CPU architecture : {avr0|...|avr6}
# Parts : {at90usb646|at90usb647|at90usb1286|at90usb1287|at90usb162|at90usb82}
MCU=at90usb1287

# Source files
# Add source files here
CSRCS=\
	src/main.c src/buttons.c src/timer.c \
	common/modules/scheduler/scheduler.c \
  common/lib_mcu/wdt/wdt_drv.c \
  common/lib_mcu/power/power_drv.c \
	common/modules/usb/usb_task.c \
	common/modules/usb/device_chap9/usb_device_task.c \
	common/modules/usb/device_chap9/usb_standard_request.c \
	src/usb_descriptors.c \
	src/usb_specific_request.c \
	common/lib_mcu/usb/usb_drv.c \
	common/lib_mcu/adc/adc_drv.c \
	common/lib_mcu/uart/uart_lib.c \
	common/lib_board/usb_key/usb_key.c \
	src/uart_usb_lib.c \
	src/circ_buffer.c \
	src/my_uart_usb_lib.c \
	src/cdc_task.c  \
	src/device_mouse_task.c \
	src/my_interrupt.c
	



# Assembler source files
ASSRCS=\

# Include Directories
#INCLUDES = -I"./.." -I"../conf" -I"../../.." -I"../../../../at90usb128" -I"../../../../common" 
INCLUDES= -I"./inc/" -I"conf" -I"common" -I"common/modules"



PROGRAMMER=dfu-programmer

