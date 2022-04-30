
##########------------------------------------------------------##########
##########              Project-specific Details                ##########
##########    Check these every time you start a new project    ##########
##########------------------------------------------------------##########

MCU   = atmega32u4
#SERIAL_PORT = COM6
SERIAL_BAUD = 9600
#UPLOAD_BAUD = 57600 # arduino nano clone needs default overrided
PROGRAMMER_TYPE = arduino

## A directory for common include files and the simple USART library.
## If you move either the current folder or the Library folder, you'll 
##  need to change this path to match.
LIBDIR = ../../libs/avr_lib
## Extra sources to include (subfolders of the lib root dir)
LIBSRCS = usb/usb_hid_debug devices/hd44780

## Include the library makefile which has all project non-specific details
include $(LIBDIR)/include.mak
