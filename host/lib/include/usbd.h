#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>

namespace USBD
{
    const uint32_t LOW = 0x0;
    const uint32_t HIGH = 0x01;

    //common Packet IDs
    const uint32_t PID_ERROR = 0x0;
    const uint32_t PID_OK = 0x1;
    const uint32_t PID_DEVICE = 0x2;
    const uint32_t PID_MAX_LEN = 0x3A;

    // GPIO specific packet id's
    const uint32_t PID_SET_GPIO = 0x10;
    const uint32_t PID_SET_GPIO_VAL = 0x11;
    const uint32_t PID_GET_GPIO_VAL = 0x12;

    // ADC specific packet id's
    const uint32_t PID_SET_ADC = 0x20;
    const uint32_t PID_GET_ADC_SINGLE = 0x21;
    const uint32_t PID_GET_ADC_STREAM = 0x22;

    // DAC specific packet id's
    const uint32_t PID_SET_DAC = 0x30;
    const uint32_t PID_SET_DAC_SINGLE = 0x31;
    const uint32_t PID_SET_DAC_STREAM = 0x32;

    // UART specific packet id's
    const uint32_t PID_UART_SET_DEVICE = 0x40;
    const uint32_t PID_UART_TRANSMIT = 0x41;
    const uint32_t PID_UART_RECIEVE = 0x42;

    // I2C specific packet id's
    const uint32_t PID_I2C_SET_DEVICE = 0x50;
    const uint32_t PID_I2C_TRANSMIT = 0x51;
    const uint32_t PID_I2C_RECIEVE = 0x52;

    // SPI specific packet id's
    const uint32_t PID_SPI_SET_DEVICE = 0x60;
    const uint32_t PID_SPI_TRANSMIT = 0x61;
    const uint32_t PID_SPI_RECIEVE = 0x62;

    const uint32_t GPIO_MODE_INPUT = 0x1;
    const uint32_t GPIO_MODE_OUTPUT_PP = 0x2;
    const uint32_t GPIO_MODE_OUTPUT_OD = 0x3;
    const uint32_t GPIO_MODE_OUTPUT = GPIO_MODE_OUTPUT_PP;

    const uint32_t GPIO_MODE_ANALOG = 0x6;

    const uint32_t GPIO_MODE_IT_RISING = 0x7;
    const uint32_t GPIO_MODE_IT_FALLING = 0x8;
    const uint32_t GPIO_MODE_IT_RISING_FALLING = 0x9;

    const uint32_t GPIO_NOPULL = 0x0;
    const uint32_t GPIO_PULLUP = 0x1;
    const uint32_t GPIO_PULLDOWN = 0x2;

    struct __attribute__((packed)) PacketTypeDef
    {
        uint16_t pid;
        uint16_t length;
        uint16_t intr;
        uint8_t data[58];
    };

    struct __attribute__((packed)) PinTypeDef
    {
        uint32_t pin;
        uint32_t dir;
        uint32_t pull;
        uint32_t val;
    };

    struct __attribute__((packed)) ErrorTypeDef
    {
        uint16_t type;
        uint8_t str[56];
    };

    struct __attribute__((packed)) UARTTypeDef
    {
        uint32_t id;
        uint32_t active;
        uint32_t baud;
        uint32_t pin_tx;
        uint32_t pin_rx;
    };

    struct __attribute__((packed)) I2CTypeDef
    {
        uint32_t id;
        uint32_t active;
        uint32_t clock_freq;
        uint32_t pin_scl;
        uint32_t pin_sda;
    };

    struct __attribute__((packed)) SPITypeDef
    {
        uint32_t id;
	    uint32_t active;
	    uint32_t clock_freq;
	    uint32_t pin_tx;
	    uint32_t pin_rx;
	    uint32_t pin_clk;
    };

    class Usbduino
    {
    private:
        int device;
        int is_open;
        int ret;
        uint8_t buffer[64];

    private:
        PacketTypeDef *packet;
        PinTypeDef *pin_config;
        ErrorTypeDef *error;
        UARTTypeDef *uart;
        I2CTypeDef *i2c;
        SPITypeDef *spi;

    public:
        Usbduino();
        
        ~Usbduino();
        
        int connect(char *dev);
        
        int release();
        
        int pinMode(uint32_t pin, uint32_t mode, uint32_t pull = GPIO_NOPULL);
        
        int digitalWrite(uint32_t pin, uint32_t value);
        int digitalRead(uint32_t pin);

        int analogRead(uint32_t pin);
        int analogWrite(uint32_t pin, uint32_t value);

        int uartSetup(uint32_t baud, uint32_t tx_pin = 0,uint32_t rx_pin = 0,uint32_t device_id = 0);
        int uartFree(uint32_t device_id = 0);
        int uartWrite(uint8_t *data, uint32_t length, uint32_t device_id = 0);
        int uartRead(uint8_t *data, uint32_t *length, uint32_t device_id = 0);

        int i2cSetup(uint32_t clock,uint32_t sda_pin = 0,uint32_t scl_pin = 0,uint32_t device_id = 0);
        int i2cFree(uint32_t device_id = 0);
        int i2cWrite(uint8_t address,uint8_t *data,uint32_t length,uint32_t device_id = 0);
        int i2cRead(uint8_t address,uint8_t *data,uint32_t *length,uint32_t device_id = 0);

        int spiSetup(uint32_t clock,uint32_t tx_pin = 0,uint32_t rx_pin = 0,uint32_t clk_pin = 0,uint32_t device_id = 0);
        int spiFree(uint32_t device_id = 0);
        int spiWrite(uint8_t *data,uint32_t length,uint32_t device_id = 0);
        int spiRead(uint8_t *data,uint32_t *length,uint32_t device_id = 0);

        void delay(uint32_t ms);
    };

};