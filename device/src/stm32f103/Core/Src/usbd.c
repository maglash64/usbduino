#include <usbd.h>
#include "usbd_ctlreq.h"


static uint8_t  USBD_DEV_Init(USBD_HandleTypeDef *pdev,uint8_t cfgidx);

static uint8_t  USBD_DEV_DeInit(USBD_HandleTypeDef *pdev,uint8_t cfgidx);

static uint8_t  USBD_DEV_Setup(USBD_HandleTypeDef *pdev,USBD_SetupReqTypedef *req);

static uint8_t  *USBD_DEV_GetFSCfgDesc(uint16_t *length);

static uint8_t  *USBD_DEV_GetDeviceQualifierDesc(uint16_t *length);

static uint8_t  USBD_DEV_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t  USBD_DEV_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum);

USBD_ClassTypeDef  USBD_DEV =
{
		USBD_DEV_Init,
		USBD_DEV_DeInit,
		USBD_DEV_Setup,
		NULL, /*EP0_TxSent*/
		NULL, /*EP0_RxReady*/
		USBD_DEV_DataIn, /*DataIn*/
		USBD_DEV_DataOut, /*DataOut*/
		NULL, /*SOF */
		NULL,
		NULL,
		USBD_DEV_GetFSCfgDesc,
		USBD_DEV_GetFSCfgDesc,
		USBD_DEV_GetFSCfgDesc,
		USBD_DEV_GetDeviceQualifierDesc,
};


/* USB HID device FS Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_DEV_CfgFSDesc[USB_HID_CONFIG_DESC_SIZ]  __ALIGN_END =
{
		0x09, /* bLength: Configuration Descriptor size */
		USB_DESC_TYPE_CONFIGURATION, /* bDescriptorType: Configuration */
		USB_HID_CONFIG_DESC_SIZ,  	/* wTotalLength: Bytes returned */
		0x00,
		0x01,         /*bNumInterfaces: 1 interface*/
		0x01,         /*bConfigurationValue: Configuration value*/
		0x00,         /*iConfiguration: Index of string descriptor describing the configuration*/
		0xE0,         /*bmAttributes: bus powered and Support Remote Wake-up */
		0x32,         /*MaxPower 100 mA: this current is used for detecting Vbus*/


		0x09,         /*bLength: Interface Descriptor size*/
		USB_DESC_TYPE_INTERFACE,/*bDescriptorType: Interface descriptor type*/
		0x00,         /*bInterfaceNumber: Number of Interface*/
		0x00,         /*bAlternateSetting: Alternate setting*/
		0x02,         /*bNumEndpoints*/
		0xFF,         /*bInterfaceClass: HID*/
		0x00,         /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
		0x00,         /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
		0,            /*iInterface: Index of string descriptor*/

		0x07,          /*bLength: Endpoint Descriptor size*/
		USB_DESC_TYPE_ENDPOINT, /*bDescriptorType:*/
		0x81,     /*bEndpointAddress: Endpoint Address (IN)*/
		0x02,          /*bmAttributes: BULK endpoint*/
		0x40, /*wMaxPacketSize: 308 Byte max */
		0x00,
		HID_FS_BINTERVAL,          /*bInterval: Polling Interval */

		0x07,          /*bLength: Endpoint Descriptor size*/
		USB_DESC_TYPE_ENDPOINT, /*bDescriptorType:*/
		0x01,     /*bEndpointAddress: Endpoint Address (OUT)*/
		0x02,          /*bmAttributes: BULK endpoint*/
		0x40, 			/*wMaxPacketSize: 64 Byte max */
		0x00,
		HID_FS_BINTERVAL,          /*bInterval: Polling Interval */
};




//		0x09,         /*bLength: Interface Descriptor size*/
//		USB_DESC_TYPE_INTERFACE,/*bDescriptorType: Interface descriptor type*/
//		0x01,         /*bInterfaceNumber: Number of Interface*/
//		0x00,         /*bAlternateSetting: Alternate setting*/
//		0x01,         /*bNumEndpoints*/
//		0x03,         /*bInterfaceClass: HID*/
//		0x00,         /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
//		0x00,         /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
//		0,

//		0x09,         /*bLength: HID Descriptor size*/
//		HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
//		0x11,         /*bcdHID: HID Class Spec release number*/
//		0x01,
//		0x00,         /*bCountryCode: Hardware target country*/
//		0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
//		0x22,         /*bDescriptorType*/
//		HID_REPORT_DESC_SIZE,/*wItemLength: Total length of Report descriptor*/
//		0x00,

//		0x07,          /*bLength: Endpoint Descriptor size*/
//		USB_DESC_TYPE_ENDPOINT, /*bDescriptorType:*/
//		HID_EPIN_ADDR,     /*bEndpointAddress: Endpoint Address (IN)*/
//		0x03,          /*bmAttributes: Interrupt endpoint*/
//		HID_EPIN_SIZE, /*wMaxPacketSize: 4 Byte max */
//		0x00,
//		HID_FS_BINTERVAL,          /*bInterval: Polling Interval */

//};


/* USB HID device Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_DEV_Desc[USB_HID_DESC_SIZ]  __ALIGN_END  =
{
		/* 18 */
		0x09,         /*bLength: HID Descriptor size*/
		HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
		0x11,         /*bcdHID: HID Class Spec release number*/
		0x01,
		0x00,         /*bCountryCode: Hardware target country*/
		0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
		0x22,         /*bDescriptorType*/
		HID_REPORT_DESC_SIZE,/*wItemLength: Total length of Report descriptor*/
		0x00,
};

/* USB Standard Device Descriptor */
__ALIGN_BEGIN static uint8_t USBD_DEV_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC]  __ALIGN_END =
{
		USB_LEN_DEV_QUALIFIER_DESC,
		USB_DESC_TYPE_DEVICE_QUALIFIER,
		0x00,
		0x02,
		0x00,
		0x00,
		0x00,
		0x40,
		0x01,
		0x00,
};


__ALIGN_BEGIN static uint8_t HID_ReportDesc[HID_REPORT_DESC_SIZE]  __ALIGN_END =
{
		0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
		0x15, 0x00,                    // LOGICAL_MINIMUM (0)
		0x09, 0x04,                    // USAGE (Joystick)
		0xa1, 0x01,                    // COLLECTION (Application)
		0x05, 0x02,                    //   USAGE_PAGE (Simulation Controls)
		0x09, 0xbb,                    //   USAGE (Throttle)
		0x15, 0x81,                    //   LOGICAL_MINIMUM (-127)
		0x25, 0x7f,                    //   LOGICAL_MAXIMUM (127)
		0x75, 0x08,                    //   REPORT_SIZE (8)
		0x95, 0x01,                    //   REPORT_COUNT (1)
		0x81, 0x02,                    //   INPUT (Data,Var,Abs)
		0x05, 0x01,                    //   USAGE_PAGE (Generic Desktop)
		0x09, 0x01,                    //   USAGE (Pointer)
		0xa1, 0x00,                    //   COLLECTION (Physical)
		0x09, 0x30,                    //     USAGE (X)
		0x09, 0x31,                    //     USAGE (Y)
		0x09, 0x32,                    //     USAGE (Z)
		0x95, 0x03,                    //     REPORT_COUNT (3)
		0x81, 0x02,                    //     INPUT (Data,Var,Abs)
		0xc0,                          //   END_COLLECTION
		0x05, 0x09,                    //   USAGE_PAGE (Button)
		0x19, 0x01,                    //   USAGE_MINIMUM (Button 1)
		0x29, 0x08,                    //   USAGE_MAXIMUM (Button 8)
		0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
		0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
		0x75, 0x01,                    //   REPORT_SIZE (1)
		0x95, 0x08,                    //   REPORT_COUNT (8)
		0x55, 0x00,                    //   UNIT_EXPONENT (0)
		0x65, 0x00,                    //   UNIT (None)
		0x81, 0x02,                    //   INPUT (Data,Var,Abs)
		0xc0                           // END_COLLECTION
};





static uint8_t  USBD_DEV_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
	/* Open EP IN */

	USBD_LL_OpenEP(pdev, 0x01, USBD_EP_TYPE_BULK, 0x40);
	pdev->ep_out[0x01 & 0xFU].is_used = 1U;

	USBD_LL_OpenEP(pdev, 0x81, USBD_EP_TYPE_BULK, 0x40);
	pdev->ep_in[0x81 & 0xFU].is_used = 1U;

	//USBD_LL_OpenEP(pdev, HID_EPIN_ADDR, USBD_EP_TYPE_INTR, HID_EPIN_SIZE);
	//pdev->ep_in[HID_EPIN_ADDR & 0xFU].is_used = 1U;

	pdev->pClassData = USBD_malloc(sizeof(USBD_DEV_HandleTypeDef));

	if (pdev->pClassData == NULL)
	{
		return USBD_FAIL;
	}

	((USBD_DEV_HandleTypeDef *)pdev->pClassData)->state = HID_IDLE;

	USBD_LL_PrepareReceive(pdev, 0x01, usb_buffer, 0x40);

	return USBD_OK;
}



static uint8_t  USBD_DEV_DeInit(USBD_HandleTypeDef *pdev,uint8_t cfgidx)
{
	/* Close HID EPs */
	//USBD_LL_CloseEP(pdev, HID_EPIN_ADDR);
	//pdev->ep_in[HID_EPIN_ADDR & 0xFU].is_used = 0U;

	/* FRee allocated memory */
	if (pdev->pClassData != NULL)
	{
		USBD_free(pdev->pClassData);
		pdev->pClassData = NULL;
	}

	return USBD_OK;
}



static uint8_t  USBD_DEV_Setup(USBD_HandleTypeDef *pdev,USBD_SetupReqTypedef *req)
{
	USBD_DEV_HandleTypeDef *hhid = (USBD_DEV_HandleTypeDef *) pdev->pClassData;
	uint16_t len = 0U;
	uint8_t *pbuf = NULL;
	uint16_t status_info = 0U;
	USBD_StatusTypeDef ret = USBD_OK;

	switch (req->bmRequest & USB_REQ_TYPE_MASK)
	{
	case USB_REQ_TYPE_CLASS :
		switch (req->bRequest)
		{
		case HID_REQ_SET_PROTOCOL:
			hhid->Protocol = (uint8_t)(req->wValue);
			break;

		case HID_REQ_GET_PROTOCOL:
			USBD_CtlSendData(pdev, (uint8_t *)(void *)&hhid->Protocol, 1U);
			break;

		case HID_REQ_SET_IDLE:
			hhid->IdleState = (uint8_t)(req->wValue >> 8);
			break;

		case HID_REQ_GET_IDLE:
			USBD_CtlSendData(pdev, (uint8_t *)(void *)&hhid->IdleState, 1U);
			break;

		default:
			USBD_CtlError(pdev, req);
			ret = USBD_FAIL;
			break;
		}
		break;
		case USB_REQ_TYPE_STANDARD:
			switch (req->bRequest)
			{
			case USB_REQ_GET_STATUS:
				if (pdev->dev_state == USBD_STATE_CONFIGURED)
				{
					USBD_CtlSendData(pdev, (uint8_t *)(void *)&status_info, 2U);
				}
				else
				{
					USBD_CtlError(pdev, req);
					ret = USBD_FAIL;
				}
				break;

			case USB_REQ_GET_DESCRIPTOR:
				if (req->wValue >> 8 == HID_REPORT_DESC)
				{
					len = MIN(HID_REPORT_DESC_SIZE, req->wLength);
					pbuf = HID_ReportDesc;
				}
				else if (req->wValue >> 8 == HID_DESCRIPTOR_TYPE)
				{
					pbuf = USBD_DEV_Desc;
					len = MIN(USB_HID_DESC_SIZ, req->wLength);
				}
				else
				{
					USBD_CtlError(pdev, req);
					ret = USBD_FAIL;
					break;
				}
				USBD_CtlSendData(pdev, pbuf, len);
				break;

			case USB_REQ_GET_INTERFACE :

				if (pdev->dev_state == USBD_STATE_CONFIGURED)
				{
					USBD_CtlSendData(pdev, (uint8_t *)(void *)&hhid->AltSetting, 1U);
				}
				else
				{
					USBD_CtlError(pdev, req);
					ret = USBD_FAIL;
				}
				break;

			case USB_REQ_SET_INTERFACE :

				if (pdev->dev_state == USBD_STATE_CONFIGURED)
				{
					hhid->AltSetting = (uint8_t)(req->wValue);
				}
				else
				{
					USBD_CtlError(pdev, req);
					ret = USBD_FAIL;
				}

				break;
			default:
				USBD_CtlError(pdev, req);
				ret = USBD_FAIL;
				break;
			}
			break;

			default:
				USBD_CtlError(pdev, req);
				ret = USBD_FAIL;
				break;
	}

	return ret;
}




uint8_t USBD_DEV_SendReport(USBD_HandleTypeDef  *pdev,uint8_t *report,uint16_t len)
{
	USBD_DEV_HandleTypeDef     *hhid = (USBD_DEV_HandleTypeDef *)pdev->pClassData;

	if (pdev->dev_state == USBD_STATE_CONFIGURED)
	{
		if (hhid->state == HID_IDLE)
		{
			hhid->state = HID_BUSY;
			USBD_LL_Transmit(pdev,
					0x81,
					report,
					len);
		}
	}
	return USBD_OK;
}


uint8_t USBD_Transmit(USBD_HandleTypeDef  *pdev,uint8_t *data,uint16_t len)
{
	if (pdev->dev_state == USBD_STATE_CONFIGURED)
		{
			if (usb_tx_ready)
			{
				usb_tx_ready = 0;
				USBD_LL_Transmit(pdev,0x81,data,len);
			}
		}
		return USBD_OK;
}



uint32_t USBD_DEV_GetPollingInterval(USBD_HandleTypeDef *pdev)
{
	return (uint32_t)HID_FS_BINTERVAL;
}



static uint8_t  *USBD_DEV_GetFSCfgDesc(uint16_t *length)
{
	*length = sizeof(USBD_DEV_CfgFSDesc);
	return USBD_DEV_CfgFSDesc;
}



static uint8_t  USBD_DEV_DataIn(USBD_HandleTypeDef *pdev,uint8_t epnum)
{
	usb_tx_ready = 1;
	((USBD_DEV_HandleTypeDef *)pdev->pClassData)->state = HID_IDLE;
	return USBD_OK;
}

static uint8_t  USBD_DEV_DataOut(USBD_HandleTypeDef *pdev,uint8_t epnum)
{
	usb_rx_ready = 1;
	USBD_LL_PrepareReceive(pdev, 0x01, usb_buffer, 0x40);
	return USBD_OK;
}


static uint8_t  *USBD_DEV_GetDeviceQualifierDesc(uint16_t *length)
{
	*length = sizeof(USBD_DEV_DeviceQualifierDesc);
	return USBD_DEV_DeviceQualifierDesc;
}


