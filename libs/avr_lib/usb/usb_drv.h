/**
 * @file usb_drv.h
 * @brief Low level usb drivers
 * @version 0.1
 * @date 2022-05-11
 *
 * @copyright Copyright (c) 2022
 *
 */

/**
 * TODO list:
 * - add wait for endpoint ready with timeout using frame number
 * - finish adding receive/transmit functions
 *
 */

#ifndef USB_DRV_H
#define USB_DRV_H

#include "usb_conf.h"

#ifndef _AVR_IOXXX_H_
#define __AVR_ATmega32U4__
#endif

#include <avrlibdefs.h>
#include <usb/usb_StdDescriptors.h>

#define USB_bmRequestType_OUT       (0 << 7)
#define USB_bmRequestType_IN        (1 << 7)
#define USB_bmRequestType_Standard  (0x0 << 5)
#define USB_bmRequestType_Class     (0x1 << 5)
#define USB_bmRequestType_Vendor    (0x2 << 5)
#define USB_bmRequestType_Device    (0x0 << 0)
#define USB_bmRequestType_Interface (0x1 << 0)
#define USB_bmRequestType_Endpoint  (0x2 << 0)
#define USB_bmRequestType_Other     (0x3 << 0)

#define EP_SIZE(s) \
    ((s) > 32 ? 0x30 : ((s) > 16 ? 0x20 : ((s) > 8 ? 0x10 : 0x00)))
#define USB_EPNUM_MASK        0x0f
#define USB_ENDPOINT_DIR_MASK 0x80
#define USB_ENDPOINT_DIR_IN   0x80
#define USB_ENDPOINT_DIR_OUT  0x00

#define LSB(n)                (n & 255)
#define MSB(n)                ((n >> 8) & 255)
#define USB_VERSION_BCD(Major, Minor, Revision) \
    (((Major & 0xFF) << 8) | ((Minor & 0x0F) << 4) | (Revision & 0x0F))

#if defined(__AVR_AT90USB162__)
#define HW_CONFIG()
#define PLL_CONFIG() (PLLCSR = ((1 << PLLE) | (1 << PLLP0)))
#define USB_CONFIG() (USBCON = (1 << USBE))
#define USB_FREEZE() (USBCON = ((1 << USBE) | (1 << FRZCLK)))
#elif defined(__AVR_ATmega32U4__)
#define HW_CONFIG()  (UHWCON = 0x01)
#define PLL_CONFIG() (PLLCSR = 0x12)
#define USB_CONFIG() (USBCON = ((1 << USBE) | (1 << OTGPADE)))
#define USB_FREEZE() (USBCON = ((1 << USBE) | (1 << FRZCLK)))
#elif defined(__AVR_AT90USB646__)
#define HW_CONFIG()  (UHWCON = 0x81)
#define PLL_CONFIG() (PLLCSR = 0x1A)
#define USB_CONFIG() (USBCON = ((1 << USBE) | (1 << OTGPADE)))
#define USB_FREEZE() (USBCON = ((1 << USBE) | (1 << FRZCLK)))
#elif defined(__AVR_AT90USB1286__)
#define HW_CONFIG()  (UHWCON = 0x81)
#define PLL_CONFIG() (PLLCSR = 0x16)
#define USB_CONFIG() (USBCON = ((1 << USBE) | (1 << OTGPADE)))
#define USB_FREEZE() (USBCON = ((1 << USBE) | (1 << FRZCLK)))
#endif

// Typedefs

/**
 * @brief
 *
 */
typedef struct
{
    uint8_t bmRequestType; /**< Type of the request. */
    uint8_t bRequest;      /**< Request command code. */
    uint16_t wValue;       /**< wValue parameter of the request. */
    uint16_t wIndex;       /**< wIndex parameter of the request. */
    uint16_t wLength;      /**< Length of the data to transfer in bytes. */
} ATTR_PACKED USB_Request_Header_t;

enum USB_ControlRequest
{
    USB_REQ_GetStatus        = 0,
    USB_REQ_ClearFeature     = 1,
    USB_REQ_SetFeature       = 3,
    USB_REQ_SetAddress       = 5,
    USB_REQ_GetDescriptor    = 6,
    USB_REQ_GetConfiguration = 8,
    USB_REQ_SetConfiguration = 9,
    USB_REQ_GetInterface     = 10,
    USB_REQ_SetInterface     = 11,
};

typedef enum USB_Transfer_ErrorCodes
{
    USB_TRANSFER_TRANSFER_COMPLETE   = 0,
    USB_TRANSFER_CONFIGURATION_ERROR = 1,
    USB_TRANSFER_BUFFER_NOT_READY    = 2,
    USB_TRANSFER_TRANSFER_INCOMPLETE = 3,
} USB_Transfer_ErrorCodes_t;

typedef enum USB_ReadyWait_ErrorCodes
{
    USB_READYWAIT_NoError            = 0,
    USB_READYWAIT_EndpointStalled    = 1,
    USB_READYWAIT_DeviceDisconnected = 2,
    USB_READYWAIT_BusSuspended       = 3,
    USB_READYWAIT_Timeout            = 4,

} USB_ReadyWait_ErrorCodes_t;

// Inline Functions

ATTR_ALWAYS_INLINE static inline void
USB_deviceEnableInterupts(const uint8_t ints)
{
    UDIEN = ints;
}

ATTR_ALWAYS_INLINE static inline bool
USB_deviceIntIsReset(const uint8_t intbits)
{
    return (intbits & (1 << EORSTI)) ? true : false;
}

ATTR_ALWAYS_INLINE static inline bool
USB_deviceIntIsStartOfFrame(const uint8_t intbits)
{
    return (intbits & (1 << SOFI)) ? true : false;
}

ATTR_ALWAYS_INLINE static inline void USB_deviceSetAddress(const uint8_t addr)
{
    UDADDR = (UDADDR & (1 << ADDEN)) | (addr & 0x7F);
}

ATTR_ALWAYS_INLINE static inline uint16_t USB_deviceGetFrameNumber()
{
    return UDFNUM;
}

ATTR_ALWAYS_INLINE static inline void USB_endpointSelect(const uint8_t addr)
{
    UENUM = (addr & USB_EPNUM_MASK);
}

ATTR_ALWAYS_INLINE static inline uint8_t USB_endpointGetCurrent()
{
    return (UENUM & USB_EPNUM_MASK);
}

ATTR_ALWAYS_INLINE static inline uint8_t USB_endpointGetDirection()
{
    return (UECFG0X & (1 << EPDIR)) ? ENDPOINT_DIR_IN : ENDPOINT_DIR_OUT;
}

ATTR_ALWAYS_INLINE static inline void USB_endpointReset(const uint8_t addr)
{
    UERST = (1 << (addr & USB_EPNUM_MASK));
    UERST = 0;
}

ATTR_ALWAYS_INLINE static inline void USB_endpointEnable()
{
    UECONX &= ~(1 << EPEN););
}

ATTR_ALWAYS_INLINE static inline uint8_t USB_endpointIsEnabled()
{
    return ((UECONX & (1 << EPEN)) ? true : false);
}

ATTR_ALWAYS_INLINE static inline bool USB_endpointIsRWAllowed()
{
    return ((UEINTX & (1 << RWAL)) ? true : false);
}

ATTR_ALWAYS_INLINE static inline bool USB_endpointIsConfigured()
{
    return ((UESTA0X & (1 << CFGOK)) ? true : false);
}

ATTR_ALWAYS_INLINE static inline uint8_t USB_endpointGetInterupts()
{
    return UEINT;
}

ATTR_ALWAYS_INLINE static inline bool
USB_endpointHasEndpointInterupted(const uint8_t addr)
{
    return (
        (Endpoint_GetEndpointInterrupts() & (1 << (addr & ENDPOINT_EPNUM_MASK)))
            ? true
            : false);
}

// functions for checking if an endpoint has data ready
ATTR_ALWAYS_INLINE static inline bool USB_endpointIsINReady()
{
    return ((UEINTX & (1 << TXINI)) ? true : false);
}

ATTR_ALWAYS_INLINE static inline bool USB_endpointIsOUTReceived()
{
    return ((UEINTX & (1 << RXOUTI)) ? true : false);
}

ATTR_ALWAYS_INLINE static inline bool USB_endpointIsSETUPReceived()
{
    return ((UEINTX & (1 << RXSTPI)) ? true : false);
}

// functions for waiting until an endpoint has data ready
ATTR_ALWAYS_INLINE static inline void waitForINReady()
{
    while (!USB_endpointIsINReady())
    {
    }
}

ATTR_ALWAYS_INLINE static inline void waitForOUTReceived()
{
    while (!USB_endpointIsOUTReceived())
    {
    }
}

// Functions for clearing (sending/receiving) data
ATTR_ALWAYS_INLINE static inline void USB_endpointClearSETUP()
{
    UEINTX &= ~(1 << RXSTPI);
}

ATTR_ALWAYS_INLINE static inline void USB_endpointClearIN()
{
    UEINTX &= ~((1 << TXINI) | (1 << FIFOCON));
}

ATTR_ALWAYS_INLINE static inline void USB_endpointClearOUT()
{
    UEINTX &= ~((1 << RXOUTI) | (1 << FIFOCON));
}

ATTR_ALWAYS_INLINE static inline void USB_endpointStallTransaction()
{
    UECONX |= (1 << STALLRQ);
}

ATTR_ALWAYS_INLINE static inline void USB_endpointResetDataToggle()
{
    UECONX |= (1 << RSTDT);
}

ATTR_ALWAYS_INLINE static inline void USB_endpointClearStall()
{
    UECONX |= (1 << STALLRQC);
}

ATTR_ALWAYS_INLINE static inline bool USB_endpointIsStalled()
{
    return ((UECONX & (1 << STALLRQ)) ? true : false);
}

ATTR_ALWAYS_INLINE static inline uint8_t USB_endpointReadByte()
{
    return UEDATX;
}

ATTR_ALWAYS_INLINE static inline USB_endpointWriteByte(const uint8_t data)
{
    UEDATX = data;
}

ATTR_ALWAYS_INLINE static inline void USB_endpointDiscardByte()
{
    uint8_t dummy = UEDATX;
    (void)dummy;
}

ATTR_ALWAYS_INLINE static inline uint16_t USB_endpointReadWord()
{
    union
    {
        uint16_t Value;
        uint8_t Bytes[2];
    } Data;

    Data.Bytes[0] = UEDATX;
    Data.Bytes[1] = UEDATX;

    return Data.Value;
}

ATTR_ALWAYS_INLINE static inline USB_endpointWriteWord(const uint16_t data)
{
    UEDATX = (data & 0xFF);
    UEDATX = (data >> 8);
}

uint8_t USB_getConfiguration();

void USB_init();

uint8_t USB_endpointWaitUntilReady();

uint8_t USB_receiveBuffer(void *const Buffer, uint8_t Length,
                          uint8_t *BytesProcessed);

uint16_t USB_transmitBuffer(void const *const Buffer, uint8_t Length,
                            uint8_t *BytesProcessed);

// Externally Defined Event Callbacks

bool USB_EVENT_Stub();

/**
 * @brief Event callback for a control request
 *
 */
bool USB_EVENT_ControlRequest() ATTR_WEAK ATTR_ALIAS(USB_Event_Stub);

/**
 * @brief Event callback when the configuration is changed
 *
 * User routine should do the following:
 * - configure all neccessary endpoints as needed for requested configuration
 * - issue reset for all configured endpoints (not control endpoint)
 *
 */
bool USB_EVENT_ConfigurationChanged() ATTR_WEAK ATTR_ALIAS(USB_Event_Stub);

/**
 * @brief Event callback when a GET_DESCRIPTOR request is received
 *
 * User routine should do the following:
 * - issue stall if descriptor requested does not exist
 * - send the descriptor broken up by the packet data size
 * - for each packet
 *   - wait for the IN buffer to be ready
 *   - abort if a new OUT packet has been received
 */
bool USB_EVENT_GetDescriptor() ATTR_WEAK ATTR_ALIAS(USB_Event_Stub);

/**
 * @brief Event callback for when a USB reset has occured
 *
 */
bool USB_EVENT_Reset() ATTR_WEAK ATTR_ALIAS(USB_Event_Stub);

/**
 * @brief Event callback for when a start of frame packet is received
 *
 */
bool USB_EVENT_StartOfFrame() ATTR_WEAK ATTR_ALIAS(USB_Event_Stub);

// External Variables
extern volatile USB_Request_Header_t USB_ControlRequest;
extern volatile USB_Descriptor_List_t USB_DescriptorList;

#endif /* USB_DRV_H */