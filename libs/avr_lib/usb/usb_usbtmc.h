#ifndef __USB_USBTMC__
#define __USB_USBTMC__

#include <stdint.h>

void usb_init(void);          // initialize everything
uint8_t usb_configured(void); // is the USB port configured
int8_t usb_rawhid_recv(uint8_t *buffer,
                       uint8_t timeout); // receive a packet, with timeout
int8_t usb_rawhid_send(const uint8_t *buffer,
                       uint8_t timeout); // send a packet, with timeout

// Everything below this point is only intended for usb_serial.c
#ifdef USB_PRIVATE_INCLUDE
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avrlibdefs.h>
#include <string.h>

#define EP_TYPE_CONTROL         0x00
#define EP_TYPE_BULK_IN         0x81
#define EP_TYPE_BULK_OUT        0x80
#define EP_TYPE_INTERRUPT_IN    0xC1
#define EP_TYPE_INTERRUPT_OUT   0xC0
#define EP_TYPE_ISOCHRONOUS_IN  0x41
#define EP_TYPE_ISOCHRONOUS_OUT 0x40

#define EP_SINGLE_BUFFER        0x02
#define EP_DOUBLE_BUFFER        0x06

/* Type Defines */
typedef enum Usb_ErrorCodes
{
    USB_TRANSFER_TRANSFER_COMPLETE   = 0,
    USB_TRANSFER_CONFIGURATION_ERROR = 1,
    USB_TRANSFER_BUFFER_NOT_READY    = 2,
    USB_TRANSFER_TRANSFER_INCOMPLETE = 3,
} Usb_ErrorCodes_t;

typedef struct
{
    uint8_t Status;
    uint8_t Reserved;

    uint16_t TMCVersion;

    struct
    {
        unsigned ListenOnly             : 1; /* Bit 0 */
        unsigned TalkOnly               : 1;
        unsigned PulseIndicateSupported : 1;
        unsigned Reserved               : 5;
    } Interface;

    struct
    {
        unsigned SupportsAbortINOnMatch : 1;
        unsigned Reserved               : 7;
    } Device;
    // offset 7:
    uint8_t Reserved2[6];
    // Here subclass 488 extended capabilities:
    uint16_t bcdUSB488; /*0x0100 or greater */
    struct
    {
        unsigned SupportTrigger : 1; /* 1 if trigger is supported */
        unsigned
            SupportRenControl : 1; /* 1 if remote enable control is supported */
        unsigned Is488IF  : 1; /* 1 if this is a 488.2 interface (488.1 if 0)*/
        unsigned Reserved : 5;
    } USB488IfCap1;
    struct
    {
        unsigned DT1Capable    : 1; /* 1 if DT1 capable, otherwise 0 for DT0 */
        unsigned RL1Capable    : 1; /* 1 if RL1 capable, otherwise 0 for RL0  */
        unsigned SR1Capable    : 1; /* 1 if SR1 capable, otherwise 0 for SR0  */
        unsigned MandatorySCPI : 1; /* 1 if all 488.2 mandatory SCPI commands
                                       are supported */
        unsigned Reserved : 4;
    } USB488IfCap2;
    uint8_t Reserved3[8];
} TMC_Capabilities_t;

typedef struct
{
    uint8_t LastMessageTransaction;
    uint8_t TermChar;
    uint8_t Reserved[2];
} TMC_DevOUTMessageHeader_t;

typedef struct
{
    uint8_t LastMessageTransaction;
    uint8_t Reserved[3];
} TMC_DevINMessageHeader_t;

typedef struct
{
    uint8_t MessageID;
    uint8_t Tag;
    uint8_t InverseTag;
    uint8_t Reserved;
    uint32_t TransferSize;

    union
    {
        TMC_DevOUTMessageHeader_t DeviceOUT;
        TMC_DevINMessageHeader_t DeviceIN;
        uint32_t VendorSpecific;
    } MessageIDSpecific;
} TMC_MessageHeader_t;

#define Req_InitiateAbortBulkOut            0x01
#define Req_CheckAbortBulkOutStatus         0x02
#define Req_InitiateAbortBulkIn             0x03
#define Req_CheckAbortBulkInStatus          0x04
#define Req_InitiateClear                   0x05
#define Req_CheckClearStatus                0x06
#define Req_GetCapabilities                 0x07
#define Req_IndicatorPulse                  0x40

#define TMC_STATUS_SUCCESS                  0x01
#define TMC_STATUS_PENDING                  0x02
#define TMC_STATUS_FAILED                   0x80
#define TMC_STATUS_TRANSFER_NOT_IN_PROGRESS 0x81
#define TMC_STATUS_SPLIT_NOT_IN_PROGRESS    0x82
#define TMC_STATUS_SPLIT_IN_PROGRESS        0x83

#define TMC_MESSAGEID_DEV_DEP_MSG_OUT       0x01
#define TMC_MESSAGEID_DEV_DEP_MSG_IN        0x02
#define TMC_MESSAGEID_DEV_VENDOR_OUT        0x7E
#define TMC_MESSAGEID_DEV_VENDOR_IN         0x7F

#endif
#endif // __USB_RAWHID__
