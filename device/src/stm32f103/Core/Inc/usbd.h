
#ifndef __USB_HID_H
#define __USB_HID_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include  "usbd_ioreq.h"

#define HID_EPIN_ADDR                 0x82U
#define HID_EPIN_SIZE                 5U

#define USB_HID_CONFIG_DESC_SIZ       32U
#define USB_HID_DESC_SIZ              9U
#define HID_REPORT_DESC_SIZE    	  60U

#define HID_DESCRIPTOR_TYPE           0x21U
#define HID_REPORT_DESC               0x22U

#define HID_FS_BINTERVAL            	0x1

#define HID_REQ_SET_PROTOCOL          0x0BU
#define HID_REQ_GET_PROTOCOL          0x03U

#define HID_REQ_SET_IDLE              0x0AU
#define HID_REQ_GET_IDLE              0x02U

#define HID_REQ_SET_REPORT            0x09U
#define HID_REQ_GET_REPORT            0x01U


typedef enum
{
  HID_IDLE = 0,
  HID_BUSY,
}
HID_StateTypeDef;


typedef struct
{
  uint32_t             Protocol;
  uint32_t             IdleState;
  uint32_t             AltSetting;
  HID_StateTypeDef     state;
}
USBD_DEV_HandleTypeDef;

extern uint8_t 	usb_buffer[64];
extern uint8_t	usb_rx_ready;
extern uint8_t	usb_tx_ready;

extern USBD_ClassTypeDef  USBD_DEV;
#define USBD_DEV_CLASS    &USBD_DEV



uint8_t USBD_HID_SendReport(USBD_HandleTypeDef *pdev,
                            uint8_t *report,
                            uint16_t len);

uint32_t USBD_HID_GetPollingInterval(USBD_HandleTypeDef *pdev);

uint8_t USBD_Transmit(USBD_HandleTypeDef  *pdev,uint8_t *data,uint16_t len);

#ifdef __cplusplus
}
#endif

#endif  /* __USB_HID_H */

