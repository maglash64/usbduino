#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>

#define PID_ERROR 0x0
#define PID_OK 0x1
#define PID_DEVICE 0x2

//GPIO specific packet id's
#define PID_SET_GPIO 0x3
#define PID_SET_GPIO_VAL 0x4
#define PID_GET_GPIO_VAL 0x5

//ADC specific packet id's
#define PID_SET_ADC 0x6
#define PID_GET_ADC_SINGLE 0x7
#define PID_GET_ADC_STREAM 0x8

//DAC specific packet id's
#define PID_SET_DAC 0x9
#define PID_SET_DAC_SINGLE 0xA
#define PID_SET_DAC_STREAM 0xB

//UART specific packet id's

#define PID_UART_SET_DEVICE 0x7
#define PID_UART_GET_DEVICE 0x8

#define PID_UART_TRANSMIT 0x9
#define PID_UART_RECIEVE 0xA

//I2C specific packet id's
#define PID_I2C_SET_DEVICE 0xB
#define PID_I2C_GET_DEVICE 0xC

#define PID_I2C_TRANSMIT
#define PID_I2C_RECIEVE

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

#define GPIO_PIN_0 0x1
#define GPIO_PIN_1 0x2
#define GPIO_PIN_2 0x3
#define GPIO_PIN_3 0x4
#define GPIO_PIN_4 0x5
#define GPIO_PIN_5 0x6
#define GPIO_PIN_6 0x7
#define GPIO_PIN_7 0x8
#define GPIO_PIN_8 0x9
#define GPIO_PIN_9 0xA
#define GPIO_PIN_10 0xB
#define GPIO_PIN_11 0xC
#define GPIO_PIN_12 0xD
#define GPIO_PIN_13 0xE
#define GPIO_PIN_14 0xF
#define GPIO_PIN_15 0x10
#define GPIO_PIN_ALL 0x11

#define PORTA 0x1
#define PORTB 0x2
#define PORTC 0x3
#define PORTD 0x4
#define PORTE 0x5

namespace USBD
{

    struct __attribute__((packed)) PacketTypeDef
    {
        uint16_t pid;
        uint16_t length;
        uint16_t intr;
        uint8_t data[58];
    };

    struct __attribute__((packed)) PinTypeDef
    {
        uint32_t port;
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

    public:
        Usbduino();
        ~Usbduino();
        int connect(char *dev);
        int release();
        int pinMode(unsigned int port, unsigned int pin, unsigned int mode, unsigned int pull = GPIO_NOPULL);
        int digitalWrite(unsigned int port, unsigned int pin, unsigned int value);
        int digitalRead(unsigned int port, unsigned int pin);
        int analogRead(unsigned int port, unsigned int pin);
        int analogWrite(unsigned int port, unsigned int pin);

        void delay(unsigned int ms);
    };

};
