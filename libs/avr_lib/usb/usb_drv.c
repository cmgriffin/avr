#include <avr/interrupt.h>
#include <util/atomic.h>

#include "usb/usb_drv.h"

/**
 * @brief Contains the details of the most recent control request
 *
 */
volatile USB_Request_Header_t USB_ControlRequest;

volatile uint8_t configuration = 0;

uint8_t USB_getConfiguration() { return configuration; }

void USB_init()
{
    HW_CONFIG();
    USB_FREEZE(); // enable USB
    PLL_CONFIG(); // config PLL
    while (!(PLLCSR & (1 << PLOCK)))
        ;              // wait for PLL lock
    USB_CONFIG();      // start USB clock
    UDCON         = 0; // enable attach resistor
    configuration = 0;
    // enable end of reset interupt and start of frame interupt
    USB_deviceEnableInterupts((1 << EORSTE) | (1 << SOFE));
    sei();
}

uint8_t USB_endpointWaitUntilReady()
{
    uint16_t timeoutMsRemaining  = USB_STREAM_TIMEOUT_MS;

    uint16_t previousFrameNumber = USB_deviceGetFrameNumber();

    for (;;)
    {
        if (USB_endpointGetDirection() == USB_ENDPOINT_DIR_IN)
        {
            if (USB_endpointIsINReady())
                return USB_READYWAIT_NoError;
        }
        else
        {
            if (USB_endpointIsOUTReady())
                return USB_READYWAIT_NoError;
        }

        uint8_t configuration_LCL = configuration;
        if (configuration == 0)
            return USB_READYWAIT_DeviceDisconnected;
        else if (USB_endpointIsStalled())
            return USB_READYWAIT_EndpointStalled;

        uint16_t currentFrameNumber = USB_deviceGetFrameNumber();

        if (currentFrameNumber != previousFrameNumber)
        {
            previousFrameNumber = currentFrameNumber;
            if (!(timeoutMsRemaining--))
            {
                return USB_READYWAIT_Timeout;
            }
        }
    }
}

uint8_t USB_receiveBuffer(void *const Buffer, uint16_t Length,
                          uint16_t *const BytesProcessed)
{
    uint8_t *bBuffer         = (uint8_t *)Buffer;
    uint16_t bytesInTransfer = 0;
    uint8_t errorCode;

    if ((errorCode = USB_endpointWaitUntilReady()))
        return errorCode;

    if (BytesProcessed != NULL)
    {
        Length -= *BytesProcessed;
        bBuffer += *BytesProcessed;
    }

    while (Length)
    {
        // release the buffer if it is full
        if (!USB_endpointIsRWAllowed())
        {
            USB_endpointClearOUT();
            // if returning after each send
            if (BytesProcessed != NULL)
            {
                // update the state variable and return
                *BytesProcessed += bytesInTransfer;
                return USB_TRANSFER_TRANSFER_INCOMPLETE;
            }
            // otherwise wait for a new buffer to be available
            if ((errorCode = USB_endpointWaitUntilReady()))
                return errorCode;
        }
        else
        {
            *bBuffer++ = USB_endpointReadByte();
            Length--;
            bytesInTransfer++;
        }
    }
    return USB_TRANSFER_TRANSFER_COMPLETE;
}

uint8_t USB_transmitBuffer(const void *const Buffer, uint8_t Length,
                           uint8_t *BytesProcessed)
{
    uint8_t *bBuffer         = (uint8_t *)Buffer;
    uint16_t bytesInTransfer = 0;
    uint8_t errorCode;

    if ((errorCode = USB_endpointWaitUntilReady()))
        return errorCode;

    if (BytesProcessed != NULL)
    {
        Length -= *BytesProcessed;
        bBuffer += *BytesProcessed;
    }

    while (Length)
    {
        // release the buffer if it is full
        if (!USB_endpointIsRWAllowed())
        {
            USB_endpointClearIN();
            // if returning after each send
            if (BytesProcessed != NULL)
            {
                // update the state variable and return
                *BytesProcessed += bytesInTransfer;
                return USB_TRANSFER_TRANSFER_INCOMPLETE;
            }
            // otherwise wait for a new buffer to be available
            if ((errorCode = USB_endpointWaitUntilReady()))
                return errorCode;
        }
        else
        {
            *bBuffer++ = USB_endpointReadByte();
            Length--;
            bytesInTransfer++;
        }
    }
    return USB_TRANSFER_TRANSFER_COMPLETE;
}

uint8_t USB_controlTransmitBuffer_P(const void *const Buffer, uint8_t Length,
                                    uint8_t *BytesProcessed)
{
    uint8_t *bBuffer    = (uint8_t *)Buffer;
    bool lastPacketFull = false;

    if (Length > USB_ControlRequest.wLength)
        Length = USB_ControlRequest.wLength;
    else if (!(Length))
        USB_endpointClearIN();

    while (Length || lastPacketFull)
    {
        uint8_t configuration_LCL = configuration;
        if (configuration == 0)
            return USB_READYWAIT_DeviceDisconnected;
        else if (USB_endpointIsSETUPReceived())
            return USB_READYWAIT_EndpointStalled;
        else if (USB_endpointIsOUTReceived())
            break;

        // release the buffer if it is full
        if (USB_endpointIsINReady())
        {
            uint16_t bytesInEndpoint = USB_endpointCurrentNBytes();
            while (Length && (bytesInEndpoint < USB_CONTROL_ENDPOINT_SIZE))
            {
                USB_endpointWriteByte(pgm_read_byte(bBuffer++));
                Length--;
                bytesInEndpoint++;
            }
            lastPacketFull = (bytesInEndpoint == USB_CONTROL_ENDPOINT_SIZE);
            USB_endpointClearIN;
        }
    }
    while (!(USB_endpointIsOUTReceived()))
    {
        uint8_t configuration_LCL = configuration;
        if (configuration == 0)
            return USB_READYWAIT_DeviceDisconnected;
        else if (USB_endpointIsSETUPReceived())
            return USB_READYWAIT_EndpointStalled;
    }
    return USB_TRANSFER_TRANSFER_COMPLETE;
}

bool USB_EVENT_Stub() {}

// USB Device Interrupt - handle all device-level events
// the transmit buffer flushing is triggered by the start of frame
//
ISR(USB_GEN_vect)
{
    uint8_t intbits;

    intbits = UDINT;
    UDINT   = 0;
    if (USB_deviceIntIsReset(intbits))
    {
        USB_EVENT_Reset();
        configuration = 0;
    }
    if (USB_deviceIntIsStartOfFrame(intbits) && configuration)
    {
        USB_EVENT_StartOfFrame();
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

    USB_endpointSelect(0);

    // Load the global struct with control request values
    USB_ControlRequest.bmRequestType = USB_endpointReadByte();
    USB_ControlRequest.bRequest      = USB_endpointReadByte();
    USB_ControlRequest.wValue        = USB_endpointReadWord();
    USB_ControlRequest.wIndex        = USB_endpointReadWord();
    USB_ControlRequest.wLength       = USB_endpointReadWord();

    // call the user event first before handling standard requests
    if (USB_EVENT_ControlRequest())
        return;

    if (!USB_endpointIsSETUPReceived())
    {
        // this ISR only processes setup requests currently
        USB_endpointStallTransaction();
        return;
    }

    // UEINTX = ~((1 << RXSTPI) | (1 << RXOUTI) | (1 << TXINI));
    USB_endpointClearSETUP();

    switch (USB_ControlRequest.bRequest)
    {
    case USB_REQ_GetDescriptor:
        // call the user handler, returns a bool but not needed
        // since user will perform all needed actions
        USB_EVENT_GetDescriptor();
        return;

    case USB_REQ_SetAddress:
        USB_endpointClearIN();
        waitForINReady();
        USB_deviceSetAddress(USB_ControlRequest.wValue);
        return;

    case USB_REQ_SetConfiguration:
        // only handle standard SET CONFIGURATION requests here
        if (USB_ControlRequest.bmRequestType ==
            (USB_bmRequestType_OUT | USB_bmRequestType_Standard |
             USB_bmRequestType_Device))
        {
            configuration = USB_ControlRequest.wValue;
            USB_endpointClearIN();
            USB_EVENT_ConfigurationChanged();
            return;
        }

    case USB_REQ_GetConfiguration:
        // only handle standard SET CONFIGURATION requests here
        if (USB_ControlRequest.bmRequestType ==
            (USB_bmRequestType_IN | USB_bmRequestType_Standard |
             USB_bmRequestType_Device))
        {
            waitForINReady();
            USB_endpointWriteByte(configuration);
            USB_endpointClearIN();
            return;
        }

    case USB_REQ_GetStatus:
        waitForINReady();
        i = 0;
        if (USB_ControlRequest.bmRequestType ==
            (USB_bmRequestType_IN | USB_bmRequestType_Endpoint))
        {
            USB_endpointSelect(USB_ControlRequest.wIndex);
            if (USB_endpointIsStalled())
                i = 1;
            USB_endpointSelect(0);
        }
        USB_endpointWriteByte(i);
        USB_endpointWriteByte(0);
        USB_endpointClearIN();
        return;

    case USB_REQ_ClearFeature:
    case USB_REQ_SetFeature:
        if (USB_ControlRequest.bmRequestType == 0x02 &&
            USB_ControlRequest.wValue == 0)
        {
            // get the targeted endpoint addr
            i = USB_ControlRequest.wIndex & 0x7F;
            if (i >= 1 && i <= USB_MAX_ENDPOINT)
            {
                USB_endpointClearIN();
                USB_endpointSelect(i);
                if (USB_ControlRequest.bRequest == USB_REQ_SetFeature)
                {
                    USB_endpointStallTransaction();
                }
                else
                {
                    USB_endpointClearStall();
                    USB_endpointResetDataToggle();
                    USB_endpointReset(i);
                }
                return;
            }
        }

    default:
        break;
    }

    USB_endpointStallTransaction();
}
