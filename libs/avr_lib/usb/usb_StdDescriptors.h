#ifndef __USB_STD_DESCRIPTORS__
#define __USB_STD_DESCRIPTORS__
/**
 * @file usb_std_descriptors.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-05-18
 *
 * @copyright Copyright (c) 2022
 *
 */

/**
 * TODO List:
 * - add defines/enums for common descriptor values
 *
 */

#include <avrlibdefs.h>

#define USBDESC_bLength_DEVICE          18
#define USBDESC_bLength_CONFIG          9
#define USBDESC_bLength_INTF            9
#define USBDESC_bLength_ENDPT           7

#define USBDESC_bDescriptorType_DEVICE  1
#define USBDESC_bDescriptorType_CONFIG  2
#define USBDESC_bDescriptorType_STRING  3
#define USBDESC_bDescriptorType_INTF    4
#define USBDESC_bDescriptorType_ENDPT   5

#define USBDESC_Req_wValue_DEVICE       0x0100
#define USBDESC_Req_wIndex_DEVICE       0x0000

#define USBDESC_Req_wValue_CONFIG       0x0200
#define USBDESC_Req_wIndex_CONFIG       0x0000

#define USBDESC_Req_wValue_STRING(iStr) (0x0300 + (iStr))
#define USBDESC_Req_wIndex_STRING       0x0409

/**
 * @brief Struct used by the get_descriptor routine to find a matching
 * descriptor
 *
 */
struct USB_Descriptor_Request
{
    uint16_t wValue;  /* descriptor type and index from  */
    uint16_t wIndex;  /* 0x0000 or lang ID */
    const void *addr; /* address of descriptor data in flash*/
    uint8_t length;   /* length of the descriptor */
};

/**
 * @brief A weak definition for
 *
 */
ATTR_WEAK typedef struct
{
    uint8_t num_descriptors;
    struct USB_Descriptor_Request descriptors[1];
} USB_Descriptor_List_t;

/** \brief Standard USB Device Descriptor (USB-IF naming conventions).
 *
 *  Type define for a standard Device Descriptor. This structure uses the
 * relevant standard's given element names to ensure compatibility with the
 * standard.
 *
 *  \see \ref USB_Descriptor_Device_t for the version of this type with
 * non-standard LUFA specific element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as
 * little endian.
 */
typedef struct
{
    uint8_t bLength;         /**< Size of the descriptor, in bytes. */
    uint8_t bDescriptorType; /**< Type of the descriptor, either a value in \ref
                              * USB_DescriptorTypes_t or a value given by the
                              * specific class.
                              */
    uint16_t bcdUSB;         /**< BCD of the supported USB specification.
                              *
                              *   \see \ref VERSION_BCD() utility macro.
                              */
    uint8_t bDeviceClass;    /**< USB device class. */
    uint8_t bDeviceSubClass; /**< USB device subclass. */
    uint8_t bDeviceProtocol; /**< USB device protocol. */
    uint8_t bMaxPacketSize0; /**< Size of the control (address 0) endpoint's
                                bank in bytes. */
    uint16_t idVendor;       /**< Vendor ID for the USB product. */
    uint16_t idProduct;      /**< Unique product ID for the USB product. */
    uint16_t bcdDevice;      /**< Product release (version) number.
                              *
                              *   \see \ref VERSION_BCD() utility macro.
                              */
    uint8_t
        iManufacturer; /**< String index for the manufacturer's name. The
                        *   host will request this string via a separate
                        *   control request for the string descriptor.
                        *
                        *   \note If no string supplied, use \ref NO_DESCRIPTOR.
                        */
    uint8_t iProduct;  /**< String index for the product name/details.
                        *
                        *  \see ManufacturerStrIndex structure entry.
                        */
    uint8_t
        iSerialNumber; /**< String index for the product's globally unique
                        * hexadecimal serial number, in uppercase Unicode ASCII.
                        *
                        *  \note On some microcontroller models, there is an
                        * embedded serial number in the chip which can be used
                        * for the device serial number. To use this serial
                        * number, set this to \c USE_INTERNAL_SERIAL. On
                        * unsupported devices, this will evaluate to \ref
                        * NO_DESCRIPTOR and will cause the host to generate a
                        * pseudo-unique value for the device upon insertion.
                        *
                        *  \see \c ManufacturerStrIndex structure entry.
                        */
    uint8_t bNumConfigurations; /**< Total number of configurations supported by
                                 *   the device.
                                 */
} ATTR_PACKED USB_Descriptor_Device_t;

/** \brief Standard USB Configuration Descriptor (USB-IF naming conventions).
 *
 *  Type define for a standard Configuration Descriptor header. This structure
 * uses the relevant standard's given element names to ensure compatibility with
 * the standard.
 *
 *  \see \ref USB_Descriptor_Device_t for the version of this type with
 * non-standard LUFA specific element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as
 * little endian.
 */
typedef struct
{
    uint8_t bLength;         /**< Size of the descriptor, in bytes. */
    uint8_t bDescriptorType; /**< Type of the descriptor, either a value in \ref
                              * USB_DescriptorTypes_t or a value given by the
                              * specific class.
                              */
    uint16_t
        wTotalLength; /**< Size of the configuration descriptor header,
                       *   and all sub descriptors inside the configuration.
                       */
    uint8_t
        bNumInterfaces; /**< Total number of interfaces in the configuration. */
    uint8_t bConfigurationValue; /**< Configuration index of the current
                                    configuration. */
    uint8_t iConfiguration; /**< Index of a string descriptor describing the
                               configuration. */
    uint8_t
        bmAttributes;  /**< Configuration attributes, comprised of a mask of \c
                        * USB_CONFIG_ATTR_* masks.  On all devices, this should
                        * include USB_CONFIG_ATTR_RESERVED at a minimum.
                        */
    uint8_t bMaxPower; /**< Maximum power consumption of the device while in the
                        *   current configuration, calculated by the \ref
                        * USB_CONFIG_POWER_MA() macro.
                        */
} ATTR_PACKED USB_Descriptor_Configuration_Header_t;

/** \brief Standard USB Interface Descriptor (USB-IF naming conventions).
 *
 *  Type define for a standard Interface Descriptor. This structure uses the
 * relevant standard's given element names to ensure compatibility with the
 * standard.
 *
 *  \see \ref USB_Descriptor_Interface_t for the version of this type with
 * non-standard LUFA specific element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as
 * little endian.
 */
typedef struct
{
    uint8_t bLength;         /**< Size of the descriptor, in bytes. */
    uint8_t bDescriptorType; /**< Type of the descriptor, either a value in \ref
                              * USB_DescriptorTypes_t or a value given by the
                              * specific class.
                              */
    uint8_t bInterfaceNumber; /**< Index of the interface in the current
                                 configuration. */
    uint8_t
        bAlternateSetting; /**< Alternate setting for the interface number. The
                            * same interface number can have multiple alternate
                            * settings with different endpoint configurations,
                            * which can be selected by the host.
                            */
    uint8_t bNumEndpoints; /**< Total number of endpoints in the interface. */
    uint8_t bInterfaceClass;    /**< Interface class ID. */
    uint8_t bInterfaceSubClass; /**< Interface subclass ID. */
    uint8_t bInterfaceProtocol; /**< Interface protocol ID. */
    uint8_t iInterface; /**< Index of the string descriptor describing the
                         *   interface.
                         */
} ATTR_PACKED USB_Descriptor_Interface_t;

/** \brief Standard USB Interface Association Descriptor (USB-IF naming
 * conventions).
 *
 *  Type define for a standard Interface Association Descriptor. This structure
 * uses the relevant standard's given element names to ensure compatibility with
 * the standard.
 *
 *  This descriptor has been added as a supplement to the USB2.0 standard, in
 * the ECN located at
 *  <a>http://www.usb.org/developers/docs/InterfaceAssociationDescriptor_ecn.pdf</a>.
 * It allows composite devices with multiple interfaces related to the same
 * function to have the multiple interfaces bound together at the point of
 * enumeration, loading one generic driver for all the interfaces in the single
 *  function. Read the ECN for more information.
 *
 *  \see \ref USB_Descriptor_Interface_Association_t for the version of this
 * type with non-standard LUFA specific element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as
 * little endian.
 */
typedef struct
{
    uint8_t bLength;         /**< Size of the descriptor, in bytes. */
    uint8_t bDescriptorType; /**< Type of the descriptor, either a value in \ref
                              * USB_DescriptorTypes_t or a value given by the
                              * specific class.
                              */
    uint8_t bFirstInterface; /**< Index of the first associated interface. */
    uint8_t bInterfaceCount; /**< Total number of associated interfaces. */
    uint8_t bFunctionClass;  /**< Interface class ID. */
    uint8_t bFunctionSubClass; /**< Interface subclass ID. */
    uint8_t bFunctionProtocol; /**< Interface protocol ID. */
    uint8_t iFunction; /**< Index of the string descriptor describing the
                        *   interface association.
                        */
} ATTR_PACKED USB_Descriptor_Interface_Association_t;

/** \brief Standard USB Endpoint Descriptor (USB-IF naming conventions).
 *
 *  Type define for a standard Endpoint Descriptor. This structure uses the
 * relevant standard's given element names to ensure compatibility with the
 * standard.
 *
 *  \see \ref USB_Descriptor_Endpoint_t for the version of this type with
 * non-standard LUFA specific element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as
 * little endian.
 */
typedef struct
{
    uint8_t bLength;         /**< Size of the descriptor, in bytes. */
    uint8_t bDescriptorType; /**< Type of the descriptor, either a value in \ref
                              * USB_DescriptorTypes_t or a value given by the
                              * specific class.
                              */
    uint8_t bEndpointAddress; /**< Logical address of the endpoint within the
                               * device for the current configuration, including
                               * direction mask.
                               */
    uint8_t bmAttributes; /**< Endpoint attributes, comprised of a mask of the
                           * endpoint type (EP_TYPE_*) and attributes
                           * (ENDPOINT_ATTR_*) masks.
                           */
    uint16_t wMaxPacketSize; /**< Size of the endpoint bank, in bytes. This
                              * indicates the maximum packet size that the
                              * endpoint can receive at a time.
                              */
    uint8_t bInterval; /**< Polling interval in milliseconds for the endpoint if
                        * it is an INTERRUPT or ISOCHRONOUS type.
                        */
} ATTR_PACKED USB_Descriptor_Endpoint_t;

/** \brief Standard USB String Descriptor (USB-IF naming conventions).
 *
 *  Type define for a standard string descriptor. Unlike other standard
 * descriptors, the length of the descriptor for placement in the descriptor
 * header must be determined by the \ref USB_STRING_LEN() macro rather than by
 * the size of the descriptor structure, as the length is not fixed.
 *
 *  This structure should also be used for string index 0, which contains the
 * supported language IDs for the device as an array.
 *
 *  This structure uses the relevant standard's given element names to ensure
 * compatibility with the standard.
 *
 *  \see \ref USB_Descriptor_String_t for the version of this type with with
 * non-standard LUFA specific element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as
 * little endian.
 */
typedef struct
{
    uint8_t bLength;         /**< Size of the descriptor, in bytes. */
    uint8_t bDescriptorType; /**< Type of the descriptor, either a value in \ref
                              * USB_DescriptorTypes_t or a value given by the
                              * specific class.
                              */
    uint16_t bString[]; /**< String data, as unicode characters (alternatively,
                         * string language IDs). If normal ASCII characters are
                         * to be used, they must be added as an array of
                         * characters rather than a normal C string so that they
                         * are widened to Unicode size.
                         *
                         *   Under GCC, strings prefixed with the "L" character
                         * (before the opening string quotation mark) are
                         * considered to be Unicode strings, and may be used
                         * instead of an explicit array of ASCII characters.
                         */
} ATTR_PACKED USB_Descriptor_String_t;

#endif // __USB_STD_DESCRIPTORS__