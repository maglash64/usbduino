#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hardware/gpio.h"
#include "hardware/uart.h"
#include "hardware/i2c.h"
#include "hardware/spi.h"
#include "hardware/adc.h"
#include "hardware/irq.h"

#include "bsp/board.h"
#include "tusb.h"

#include "class/vendor/vendor_device.h"
#include "usb_descriptors.h"

#define PID_ERROR 0x0
#define PID_OK 0x1
#define PID_DEVICE 0x2
#define PID_MAX_LEN 0x3A

// GPIO specific packet id's
#define PID_SET_GPIO 0x10
#define PID_SET_GPIO_VAL 0x11
#define PID_GET_GPIO_VAL 0x12

// ADC specific packet id's
#define PID_SET_ADC 0x20
#define PID_GET_ADC_SINGLE 0x21
#define PID_GET_ADC_STREAM 0x22

// DAC specific packet id's
#define PID_SET_DAC 0x30
#define PID_SET_DAC_SINGLE 0x31
#define PID_SET_DAC_STREAM 0x32

// UART specific packet id's

#define PID_UART_SET_DEVICE 0x40
#define PID_UART_TRANSMIT 0x41
#define PID_UART_RECIEVE 0x42

// I2C specific packet id's
#define PID_I2C_SET_DEVICE 0x50
#define PID_I2C_TRANSMIT 0x51
#define PID_I2C_RECIEVE 0x52

// SPI specific packet id's
#define PID_SPI_SET_DEVICE 0x60
#define PID_SPI_TRANSMIT 0x61
#define PID_SPI_RECIEVE 0x62


#define GPIO_MODE_INPUT 0x1
#define GPIO_MODE_OUTPUT_PP 0x2
#define GPIO_MODE_OUTPUT_OD 0x3
#define GPIO_MODE_OUTPUT GPIO_MODE_OUTPUT_PP
#define GPIO_MODE_AF_PP 0x4
#define GPIO_MODE_AF_OD 0x5
#define GPIO_MODE_AF_INPUT GPIO_MODE_INPUT

#define GPIO_MODE_ANALOG 0x6

#define GPIO_MODE_IT_RISING 0x7
#define GPIO_MODE_IT_FALLING 0x8
#define GPIO_MODE_IT_RISING_FALLING 0x9

#define GPIO_NOPULL 0x0
#define GPIO_PULLUP 0x1
#define GPIO_PULLDOWN 0x2

struct PacketTypeDef
{
	uint16_t pid;
	uint16_t length;
	uint16_t intr;
	uint8_t data[58];
} __attribute__((packed)) * packet;

struct PinTypeDef
{
	uint32_t pin;
	uint32_t dir;
	uint32_t pull;
	uint32_t val;
} __attribute__((packed)) * pin_config;

struct ErrorTypeDef
{
	uint16_t type;
	uint8_t str[56];
} __attribute__((packed)) * error;

struct UARTTypeDef
{
	uint32_t id;
	uint32_t active;
	uint32_t baud;
	uint32_t pin_tx;
	uint32_t pin_rx;
} __attribute__((packed)) * uart_config;

struct I2CTypeDef
{
	uint32_t id;
	uint32_t active;
	uint32_t clock_freq;
	uint32_t pin_scl;
	uint32_t pin_sda;
} __attribute__((packed)) * i2c_config;

struct SPITypeDef
{
	uint32_t id;
	uint32_t active;
	uint32_t clock_freq;
	uint32_t pin_tx;
	uint32_t pin_rx;
	uint32_t pin_clk;
} __attribute__((packed)) * spi_config;
