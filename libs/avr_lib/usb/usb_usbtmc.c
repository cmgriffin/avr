/* Teensy USBTMC example
 * http://www.pjrc.com/teensy/USBTMC.html
 * Copyright (c) 2009 PJRC.COM, LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above description, website URL and copyright notice and this permission
 * notice shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// Version 1.0: Initial Release
// Version 1.1: fixed bug in analog

#define USB_PRIVATE_INCLUDE
#include "usb/usb_usbtmc.h"
#include <usb/usb_StdDescriptors.h>
#include <usb/usb_drv.h>
#include <util/atomic.h>

/**************************************************************************
 *
 *  Configurable Options
 *
 **************************************************************************/

// You can change these to give your code its own name.
#define STR_MANUFACTURER L"MfgName"
#define STR_PRODUCT      L"USBTMCtest"
#define STR_SERIAL       L"0123456789"

// These 4 numbers identify your device.  Set these to
// something that is (hopefully) not used by any others!
#define VENDOR_ID  0x16C0
#define PRODUCT_ID 0x0481

// These determine the bandwidth that will be allocated
// for your communication.  You do not need to use it
// all, but allocating more than necessary means reserved
// bandwidth is no longer available to other USB devices.
#define ENDPOINT0_SIZE           32
#define USBTMC_IO_SIZE           64 // transmit packet size
#define USBTMC_NOTIFICATION_SIZE 8  // receive packet size

/**************************************************************************
 *
 *  Endpoint Buffer Configuration
 *
 **************************************************************************/

#define USB_OUT_DIR_EP               0x00
#define USB_IN_DIR_EP                0x80

#define USBTMC_INTERFACE             0
#define USBTMC_NOTIFICATION_ENDPOINT 2
#define USBTMC_NOTIFICATION_INTERVAL 0xff
#define USBTMC_NOTIFICATION_BUFFER   EP_SINGLE_BUFFER
#define USBTMC_IO_INTERVAL           0x05
#define USBTMC_IO_BUFFER             EP_DOUBLE_BUFFER
#define USBTMC_IN_ENDPOINT           3
#define USBTMC_OUT_ENDPOINT          4

static const uint8_t PROGMEM endpoint_config_table[] = {
    0, // ep 1 unused
    0,
    //  EP_TYPE_INTERRUPT_IN,
    //  EP_SIZE(USBTMC_NOTIFICATION_SIZE) | USBTMC_NOTIFICATION_BUFFER,
    1,                                           //
    EP_TYPE_BULK_IN,                             //
    EP_SIZE(USBTMC_IO_SIZE) | USBTMC_IO_BUFFER,  //
    1,                                           //
    EP_TYPE_BULK_OUT,                            //
    EP_SIZE(USBTMC_IO_SIZE) | USBTMC_IO_BUFFER}; //

/**************************************************************************
 *
 *  Descriptor Data
 *
 **************************************************************************/

// Descriptors are the data that your computer reads when it auto-detects
// this USB device (called "enumeration" in USB lingo).  The most commonly
// changed items are editable at the top of this file.  Changing things
// in here should only be done by those who've read chapter 9 of the USB
// spec and relevant portions of any USB class specifications!

static const USB_Descriptor_Device_t PROGMEM device_descriptor = {
    .bLength            = 18,
    .bDescriptorType    = 1,
    .bcdUSB             = USB_VERSION_BCD(2, 0, 0),
    .bDeviceClass       = 0,
    .bDeviceSubClass    = 0,
    .bDeviceProtocol    = 0,
    .bMaxPacketSize0    = ENDPOINT0_SIZE,
    .idVendor           = VENDOR_ID,
    .idProduct          = PRODUCT_ID,
    .bcdDevice          = USB_VERSION_BCD(1, 0, 0),
    .iManufacturer      = 1,
    .iProduct           = 2,
    .iSerialNumber      = 3,
    .bNumConfigurations = 1};

typedef struct
{
    USB_Descriptor_Configuration_Header_t tmc_config;
    USB_Descriptor_Interface_t tmc_interface;
    USB_Descriptor_Endpoint_t tmc_outEndpoint;
    USB_Descriptor_Endpoint_t tmc_inEndpoint;
} tmc_config_descriptor_t;

static const tmc_config_descriptor_t config1_descriptor PROGMEM = {
    {.bLength             = 9,
     .bDescriptorType     = 2,
     .wTotalLength        = sizeof(tmc_config_descriptor_t),
     .bNumInterfaces      = 1,
     .bConfigurationValue = 1,
     .iConfiguration      = 0,
     .bmAttributes        = 0xC0,
     .bMaxPower           = 50},
    {.bLength            = 9,
     .bDescriptorType    = 4,
     .bInterfaceNumber   = 0,
     .bAlternateSetting  = 0,
     .bNumEndpoints      = 2,
     .bInterfaceClass    = 0xFE,
     .bInterfaceSubClass = 0x03,
     .bInterfaceProtocol = 0x00,
     .iInterface         = 0},
    {.bLength          = 7,
     .bDescriptorType  = 5,
     .bEndpointAddress = (USBTMC_OUT_ENDPOINT | USB_OUT_DIR_EP),
     .bmAttributes     = 0x02,
     .wMaxPacketSize   = USBTMC_IO_SIZE,
     .bInterval        = USBTMC_IO_INTERVAL},
    {.bLength          = 7,
     .bDescriptorType  = 5,
     .bEndpointAddress = (USBTMC_IN_ENDPOINT | USB_IN_DIR_EP),
     .bmAttributes     = 0x02,
     .wMaxPacketSize   = USBTMC_IO_SIZE,
     .bInterval        = USBTMC_IO_INTERVAL}};

static const USB_Descriptor_String_t PROGMEM string0 = {4, 3, {0x0409}};
static const USB_Descriptor_String_t PROGMEM string1 = {
    sizeof(STR_MANUFACTURER), 3, STR_MANUFACTURER};
static const USB_Descriptor_String_t PROGMEM string2 = {sizeof(STR_PRODUCT), 3,
                                                        STR_PRODUCT};
static const USB_Descriptor_String_t PROGMEM string3 = {sizeof(STR_SERIAL), 3,
                                                        STR_SERIAL};

// This table defines which descriptor data is sent for each specific
// request from the host (in wValue and wIndex).
static const struct descriptor_list_struct
{
    uint16_t wValue;
    uint16_t wIndex;
    const void *addr;
    uint8_t length;
} PROGMEM descriptor_list[] = {
    {0x0100, 0x0000, &device_descriptor, sizeof(device_descriptor)},
    {0x0200, 0x0000, &config1_descriptor, sizeof(config1_descriptor)},
    {0x0300, 0x0000, &string0, 4},
    {0x0301, 0x0409, &string1, sizeof(STR_MANUFACTURER)},
    {0x0302, 0x0409, &string2, sizeof(STR_PRODUCT)},
    {0x0303, 0x0409, &string3, sizeof(STR_SERIAL)}};
#define NUM_DESC_LIST \
    (sizeof(descriptor_list) / sizeof(struct descriptor_list_struct))

/** Contains the (usually static) capabilities of the TMC device. This table is
 * requested by the host upon enumeration to give it information on what
 * features of the Test and Measurement USB Class the device supports.
 */
static const TMC_Capabilities_t Capabilities PROGMEM = {
    .Status     = TMC_STATUS_SUCCESS,
    .TMCVersion = USB_VERSION_BCD(1, 0, 0),

    .Interface =
        {
            .ListenOnly             = false,
            .TalkOnly               = false,
            .PulseIndicateSupported = true,
            .Reserved               = 0,
        },

    .Device =
        {
            .SupportsAbortINOnMatch = false, // false
        },
    .Reserved2 = {0, 0, 0, 0, 0, 0},
    .bcdUSB488 = USB_VERSION_BCD(1, 0, 0),
    .USB488IfCap1 =
        {
            .SupportTrigger    = 1, // 0
            .SupportRenControl = 1,
            .Is488IF           = 0,
            .Reserved          = 0,
        },
    .USB488IfCap2 =
        {
            .DT1Capable = 1, // 0 => Device trigger no capability / full
                             // capability		=> send *TRG command
            .RL1Capable =
                1, // 0 => Remote Local   no capability / full capability
            .SR1Capable    = 0, // 0 => service request
            .MandatorySCPI = 0, // 0
            .Reserved      = 0, // 0
        },
    .Reserved3 = {0, 0, 0, 0, 0, 0, 0, 0},
};

/**************************************************************************
 *
 *  Variables - these are the only non-stack RAM usage
 *
 **************************************************************************/

// zero when we are not configured, non-zero when enumerated
static volatile uint8_t usb_configuration = 0;

/** Current TMC control request that is being processed */
static uint8_t RequestInProgress = 0;

/** Stream callback abort flag for bulk IN data */
static bool IsTMCBulkINReset = false;

/** Stream callback abort flag for bulk OUT data */
static bool IsTMCBulkOUTReset = false;

/** Last used tag value for data transfers */
static uint8_t CurrentTransferTag = 0;

/** Length of last data transfer, for reporting to the host in case an
 * in-progress transfer is aborted */
static uint16_t LastTransferLength = 0;

/** Buffer to hold the next message to sent to the TMC host */
static uint8_t NextResponseBuffer[64];

/** Indicates the length of the next response to send */
static uint8_t NextResponseLen;

/**************************************************************************
 *
 *  Public Functions - these are the API intended for the user
 *
 **************************************************************************/

// receive packets until buffer is exhausted
Usb_ErrorCodes_t usb_USBTMC_recv(void *const Buffer, uint8_t Length,
                                 uint8_t *BytesProcessed)
{
    uint8_t intr_state, read_size;

    // offset the pointer to where the previous read ended
    uint8_t *bBuffer = (uint8_t *)Buffer + *BytesProcessed;

    if (!usb_configuration)
        return USB_TRANSFER_CONFIGURATION_ERROR;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        EndpointSelect(USBTMC_OUT_ENDPOINT);

        if (!USB_endpointIsRWAllowed())
            return USB_TRANSFER_BUFFER_NOT_READY;

        uint8_t remaining_bytes = Length - *BytesProcessed;
        if (remaining_bytes > USBTMC_IO_SIZE)
            read_size = USBTMC_IO_SIZE;
        else
            read_size = remaining_bytes;
        *BytesProcessed += read_size;

        for (; read_size; --read_size)
        {
            *bBuffer++ = USB_endpointReadByte();
        }
        USB_endpointClearOUT();
    }
    if (Length == *BytesProcessed)
        return USB_TRANSFER_TRANSFER_COMPLETE;
    else
        return USB_TRANSFER_TRANSFER_INCOMPLETE;
}

// send packets until buffer is exhausted, with timeout
Usb_ErrorCodes_t usb_USBTMC_send(const void *const Buffer, uint8_t Length,
                                 uint8_t *BytesProcessed)
{
    uint8_t intr_state, write_size, last_write_size;
    // offset the pointer to where the previous read ended
    uint8_t *bBuffer = (uint8_t *)Buffer + *BytesProcessed;

    if (!usb_configuration)
        return USB_TRANSFER_CONFIGURATION_ERROR;

    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        EndpointSelect(USBTMC_IN_ENDPOINT);
        if (!USB_endpointIsRWAllowed())
            return USB_TRANSFER_BUFFER_NOT_READY;

        uint8_t remaining_bytes = Length - *BytesProcessed;
        if (remaining_bytes > USBTMC_IO_SIZE)
            write_size = USBTMC_IO_SIZE;
        else
            write_size = remaining_bytes;
        last_write_size = write_size;
        *BytesProcessed += write_size;
        // write bytes from the FIFO
        // removed the long switch-case to improve readability since we don't
        // need the speed
        for (; write_size; --write_size)
        {
            USB_endpointWriteByte(*bBuffer++);
        }
        // transmit it now
        USB_endpointClearIN();
    }
    // check if the buffer is empty and the last write size is a full
    // package if so then we need to loop around again and send a ZLP
    if (Length == *BytesProcessed && !(last_write_size == USBTMC_IO_SIZE))
        return USB_TRANSFER_TRANSFER_COMPLETE;
    else
        return USB_TRANSFER_TRANSFER_INCOMPLETE;
}

void ProcessSentMessage(uint8_t *const Data, const uint8_t Length)
{
    if (strncmp((char *)Data, "*IDN?", 5) == 0)
        strcpy((char *)NextResponseBuffer, "LUFA TMC DEMO");

    NextResponseLen = strlen((char *)NextResponseBuffer);
}

uint8_t GetNextMessage(uint8_t *const Data)
{
    strcpy((char *)NextResponseBuffer, "LUFA TMC DEMO");

    NextResponseLen = strlen((char *)NextResponseBuffer);
    // ---
    uint8_t DataLen = MIN(NextResponseLen, 64);

    strlcpy((char *)Data, (char *)NextResponseBuffer, DataLen);

    return DataLen;
}

bool TMC_readHeader(TMC_MessageHeader_t *const MessageHeader)
{
    uint8_t bytesTransferred, errorCode;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        EndpointSelect(USBTMC_OUT_ENDPOINT);
        if (!EndpointBufferReady())
        {
            return false;
        }
    }
    bytesTransferred = 0;
    while ((errorCode = usb_USBTMC_recv(
                MessageHeader, sizeof(TMC_MessageHeader_t),
                &bytesTransferred)) == USB_TRANSFER_TRANSFER_INCOMPLETE)
    {
        if (IsTMCBulkOUTReset)
            break;
    }

    CurrentTransferTag = MessageHeader->Tag;
    return (!(IsTMCBulkOUTReset) &&
            (errorCode == USB_TRANSFER_TRANSFER_COMPLETE));
}

bool TMC_writeHeader(TMC_MessageHeader_t *const MessageHeader)
{
    MessageHeader->Tag        = CurrentTransferTag;
    MessageHeader->InverseTag = ~CurrentTransferTag;

    uint8_t bytesTransferred, errorCode;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        EndpointSelect(USBTMC_IN_ENDPOINT);
        if (!EndpointBufferReady())
        {
            return false;
        }
    }
    bytesTransferred = 0;
    while ((errorCode = usb_USBTMC_send(
                MessageHeader, sizeof(TMC_MessageHeader_t),
                &bytesTransferred)) == USB_TRANSFER_TRANSFER_INCOMPLETE)
    {
        if (IsTMCBulkINReset)
            break;
    }
    return (!(IsTMCBulkINReset) &&
            (errorCode == USB_TRANSFER_TRANSFER_COMPLETE));
}

bool TMC_LastMessageComplete = true;
bool TMC_eom;
bool TMC_InLastMessageComplete = true;

/* this is the high level task to be routinely called from main loop
 * pesudo code:
 * if no bytes remaining to be received:
 *      if a header is available:
 *           switch:
 *               case msg_trigger
 *                    do trigger
 *               case dev_dep_msg_out
 *                   recieve a msg from the endpt
 *               case dev_dep_msg_in
 *                   recieve a msg from the endpt
 *
 *
 */
void TMC_task()
{
    if (!usb_configuration)
        return;
    TMC_MessageHeader_t MessageHeader;
    uint8_t MessagePayload[128];

    if (TMC_readHeader(&MessageHeader))
    {
        switch (MessageHeader.MessageID)
        {
        case TMC_MESSAGEID_DEV_DEP_MSG_OUT:
            LastTransferLength = 0;

            while (usb_USBTMC_recv(
                       MessagePayload,
                       MIN(USBTMC_IO_SIZE - sizeof(TMC_MessageHeader_t),
                           MessageHeader.TransferSize),
                       &LastTransferLength) == USB_TRANSFER_TRANSFER_INCOMPLETE)
            {
                if (IsTMCBulkOUTReset)
                    break;
            }
            ProcessSentMessage(MessagePayload, LastTransferLength);
            break;

        case TMC_MESSAGEID_DEV_DEP_MSG_IN:
            MessageHeader.TransferSize = GetNextMessage(MessagePayload);
            MessageHeader.MessageIDSpecific.DeviceOUT.LastMessageTransaction =
                true;
            WriteTMCHeader(&MessageHeader);
            LastTransferLength = 0;

            while (usb_USBTMC_send(MessagePayload, MessageHeader.TransferSize,
                                   &LastTransferLength) ==
                   USB_TRANSFER_TRANSFER_INCOMPLETE)
            {
                if (IsTMCBulkINReset)
                    break;
            }

        default:
            USB_endpointStallTransaction();
            break;
        }
    }
}

/**************************************************************************
 *
 *  Private Functions - not intended for general user consumption....
 *
 **************************************************************************/

#if (GCC_VERSION >= 40300) && (GCC_VERSION < 40302)
#error "Buggy GCC 4.3.0 compiler, please upgrade!"
#endif

// USB Device Interrupt - handle all device-level events
// the transmit buffer flushing is triggered by the start of frame
//
ISR(USB_GEN_vect)
{
    uint8_t intbits, t;

    intbits = UDINT;
    UDINT   = 0;
    if (USB_deviceIntIsReset(intbits))
    {
        ConfigureEndpoint0();
        usb_configuration = 0;
    }
    if (USB_deviceIntIsStartOfFrame(intbits) && usb_configuration)
    {
    }
}

void USBTMC_specificControlRequest(USB_Request_Header_t *USB_ControlRequest)
{
    uint8_t TMCRequestStatus = TMC_STATUS_SUCCESS;

    /* Process TMC specific control requests */
    switch (USB_ControlRequest->bRequest)
    {
    case Req_InitiateAbortBulkOut:
        if (USB_ControlRequest->bmRequestType ==
            (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_ENDPOINT))
        {
            /* Check that no split transaction is already in progress and the
             * data transfer tag is valid */
            if (RequestInProgress != 0)
            {
                TMCRequestStatus = TMC_STATUS_SPLIT_IN_PROGRESS;
            }
            else if (USB_ControlRequest->wValue != CurrentTransferTag)
            {
                TMCRequestStatus = TMC_STATUS_TRANSFER_NOT_IN_PROGRESS;
            }
            else
            {
                /* Indicate that all in-progress/pending data OUT requests
                 * should be aborted */
                IsTMCBulkOUTReset = true;

                /* Save the split request for later checking when a new request
                 * is received */
                RequestInProgress = Req_InitiateAbortBulkOut;
            }

            USB_endpointClearSETUP();

            /* Write the request response byte */
            USB_endpointWriteByte(TMCRequestStatus);

            USB_endpointClearIN();
            Endpoint_ClearStatusStage();
        }

        break;
    case Req_CheckAbortBulkOutStatus:
        if (USB_ControlRequest->bmRequestType ==
            (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_ENDPOINT))
        {
            /* Check that an ABORT BULK OUT transaction has been requested and
             * that the request has completed */
            if (RequestInProgress != Req_InitiateAbortBulkOut)
                TMCRequestStatus = TMC_STATUS_SPLIT_NOT_IN_PROGRESS;
            else if (IsTMCBulkOUTReset)
                TMCRequestStatus = TMC_STATUS_PENDING;
            else
                RequestInProgress = 0;

            Endpoint_ClearSETUP();

            /* Write the request response bytes */
            Endpoint_Write_8(TMCRequestStatus);
            Endpoint_Write_16_LE(0);
            Endpoint_Write_32_LE(LastTransferLength);

            Endpoint_ClearIN();
            Endpoint_ClearStatusStage();
        }

        break;
    case Req_InitiateAbortBulkIn:
        if (USB_ControlRequest->bmRequestType ==
            (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_ENDPOINT))
        {
            /* Check that no split transaction is already in progress and the
             * data transfer tag is valid */
            if (RequestInProgress != 0)
            {
                TMCRequestStatus = TMC_STATUS_SPLIT_IN_PROGRESS;
            }
            else if (USB_ControlRequest->wValue != CurrentTransferTag)
            {
                TMCRequestStatus = TMC_STATUS_TRANSFER_NOT_IN_PROGRESS;
            }
            else
            {
                /* Indicate that all in-progress/pending data IN requests should
                 * be aborted */
                IsTMCBulkINReset = true;

                /* Save the split request for later checking when a new request
                 * is received */
                RequestInProgress = Req_InitiateAbortBulkIn;
            }

            Endpoint_ClearSETUP();

            /* Write the request response bytes */
            Endpoint_Write_8(TMCRequestStatus);
            Endpoint_Write_8(CurrentTransferTag);

            Endpoint_ClearIN();
            Endpoint_ClearStatusStage();
        }

        break;
    case Req_CheckAbortBulkInStatus:
        if (USB_ControlRequest->bmRequestType ==
            (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_ENDPOINT))
        {
            /* Check that an ABORT BULK IN transaction has been requested and
             * that the request has completed */
            if (RequestInProgress != Req_InitiateAbortBulkIn)
                TMCRequestStatus = TMC_STATUS_SPLIT_NOT_IN_PROGRESS;
            else if (IsTMCBulkINReset)
                TMCRequestStatus = TMC_STATUS_PENDING;
            else
                RequestInProgress = 0;

            Endpoint_ClearSETUP();

            /* Write the request response bytes */
            Endpoint_Write_8(TMCRequestStatus);
            Endpoint_Write_16_LE(0);
            Endpoint_Write_32_LE(LastTransferLength);

            Endpoint_ClearIN();
            Endpoint_ClearStatusStage();
        }

        break;
    case Req_InitiateClear:
        if (USB_ControlRequest->bmRequestType ==
            (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
        {
            /* Check that no split transaction is already in progress */
            if (RequestInProgress != 0)
            {
                Endpoint_Write_8(TMC_STATUS_SPLIT_IN_PROGRESS);
            }
            else
            {
                /* Indicate that all in-progress/pending data IN and OUT
                 * requests should be aborted */
                IsTMCBulkINReset  = true;
                IsTMCBulkOUTReset = true;

                /* Save the split request for later checking when a new request
                 * is received */
                RequestInProgress = Req_InitiateClear;
            }

            Endpoint_ClearSETUP();

            /* Write the request response byte */
            Endpoint_Write_8(TMCRequestStatus);

            Endpoint_ClearIN();
            Endpoint_ClearStatusStage();
        }

        break;
    case Req_CheckClearStatus:
        if (USB_ControlRequest->bmRequestType ==
            (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
        {
            /* Check that a CLEAR transaction has been requested and that the
             * request has completed */
            if (RequestInProgress != Req_InitiateClear)
                TMCRequestStatus = TMC_STATUS_SPLIT_NOT_IN_PROGRESS;
            else if (IsTMCBulkINReset || IsTMCBulkOUTReset)
                TMCRequestStatus = TMC_STATUS_PENDING;
            else
                RequestInProgress = 0;

            Endpoint_ClearSETUP();

            /* Write the request response bytes */
            Endpoint_Write_8(TMCRequestStatus);
            Endpoint_Write_8(0);

            Endpoint_ClearIN();
            Endpoint_ClearStatusStage();
        }

        break;
    case Req_GetCapabilities:
        if (USB_ControlRequest->bmRequestType ==
            (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
        {
            Endpoint_ClearSETUP();

            /* Write the device capabilities to the control endpoint */
            Endpoint_Write_Control_Stream_LE(&Capabilities,
                                             sizeof(TMC_Capabilities_t));
            Endpoint_ClearOUT();
        }

        break;
    }
}

ATTR_ALWAYS_INLINE static inline void waitForINReady()
{
    while (!USB_endpointIsINReady())
    {
    }
}

// USB Endpoint Interrupt - endpoint 0 is handled here.  The
// other endpoints are manipulated by the user-callable
// functions, and the start-of-frame interrupt.
//
ISR(USB_COM_vect)
{
    const uint8_t *list;
    const uint8_t *cfg;
    uint8_t i, n, len, en;
    uint16_t desc_val;
    const uint8_t *desc_addr;
    uint8_t desc_length;

    USB_Request_Header_t req;

    USB_endpointSelect(0);
    if (!USB_endpointIsSETUPReceived())
    {
        // this ISR only processes setup requests currently
        USB_endpointStallTransaction();
        return;
    }
    req.bmRequestType = USB_endpointReadByte();
    req.bRequest      = USB_endpointReadByte();
    req.wValue        = USB_endpointReadWord();
    req.wIndex        = USB_endpointReadWord();
    req.wLength       = USB_endpointReadWord();

    // UEINTX = ~((1 << RXSTPI) | (1 << RXOUTI) | (1 << TXINI));
    USB_endpointClearSETUP();
    switch (req.bRequest)
    {
    case GET_DESCRIPTOR:
        list = (const uint8_t *)descriptor_list;
        for (i = 0;; i++)
        {
            if (i >= NUM_DESC_LIST)
            {
                USB_endpointStallTransaction();
                return;
            }
            desc_val = pgm_read_word(list);
            if (desc_val != req.wValue)
            {
                list += sizeof(struct descriptor_list_struct);
                continue;
            }
            list += 2;
            desc_val = pgm_read_word(list);
            if (desc_val != req.wIndex)
            {
                list += sizeof(struct descriptor_list_struct) - 2;
                continue;
            }
            list += 2;
            desc_addr = (const uint8_t *)pgm_read_word(list);
            list += 2;
            desc_length = pgm_read_byte(list);
            break;
        }
        len = (req.wLength < 256) ? req.wLength : 255;
        if (len > desc_length)
            len = desc_length;
        do
        {
            // wait for host ready for IN packet
            waitForINReady();
            if (USB_endpointIsOUTReceived())
                return; // abort
            // send IN packet
            n = len < ENDPOINT0_SIZE ? len : ENDPOINT0_SIZE;
            for (i = n; i; i--)
            {
                USB_endpointWriteByte(pgm_read_byte(desc_addr++));
            }
            len -= n;
            USB_endpointClearIN();
        } while (len || n == ENDPOINT0_SIZE);
        return;
        break;
    case SET_ADDRESS:
        USB_endpointClearIN();
        waitForINReady();
        UDADDR = req.wValue | (1 << ADDEN);
        return;
    case SET_CONFIGURATION:
        usb_configuration = req.wValue;
        USB_endpointClearIN();
        cfg = endpoint_config_table;
        for (i = 1; i < 5; i++)
        {
            USB_endpointSelect(i);
            en = pgm_read_byte(cfg++);
            USB_endpointEnable(en);
            if (en)
            {
                UECFG0X = pgm_read_byte(cfg++);
                UECFG1X = pgm_read_byte(cfg++);
            }
            USB_endpointReset(i);
        }

        // UERST = 0x1E;
        // UERST = 0;

        return;
    case GET_CONFIGURATION:
        waitForINReady();
        USB_endpointWriteByte(usb_configuration);
        USB_endpointClearIN();
        return;
    case GET_STATUS:
        waitForINReady();
        i = 0;
        if (req.bmRequestType == 0x82)
        {
            USB_endpointSelect(req.wIndex);
            if (USB_endpointIsStalled())
                i = 1;
            UENUM = 0;
        }
        UEDATX = i;
        UEDATX = 0;
        usb_send_in();
        return;
    case CLEAR_FEATURE:
    case SET_FEATURE:
        if (req.bmRequestType == 0x02 && req.wValue == 0)
            i = req.wIndex & 0x7F;
        if (i >= 1 && i <= MAX_ENDPOINT)
        {
            usb_send_in();
            UENUM = i;
            if (req.bRequest == SET_FEATURE)
            {
                UECONX = (1 << STALLRQ) | (1 << EPEN);
            }
            else
            {
                UECONX = (1 << STALLRQC) | (1 << RSTDT) | (1 << EPEN);
                UERST  = (1 << i);
                UERST  = 0;
            }
            return;
        }
    default:
        // call to handles USBTMC specific requests
        USBTMC_specificControlRequest(&req);
        break;
    }

    USB_endpointStallTransaction();
}
