#include "../include/usbd.h"

namespace USBD
{

    Usbduino::Usbduino()
    {
        device = 0;
        is_open = 0;
    }

    Usbduino::~Usbduino()
    {
        if (is_open)
            close(device);
    }

    int Usbduino::connect(char *dev)
    {
        device = open(dev, O_RDWR);
        if (device < 0)
        {
            device = 0;
            return -1;
        }

        is_open = 1;
        return 0;
    }

    int Usbduino::release()
    {
        if (is_open)
            close(device);
        return 0;
    }

    int Usbduino::pinMode(unsigned int port, unsigned int pin, unsigned int mode, unsigned int pull)
    {
        if (!device)
            return -ENODEV;

        packet = (PacketTypeDef *)buffer;

        packet->pid = PID_SET_GPIO;

        pin_config = (PinTypeDef *)packet->data;

        pin_config->port = port;
        pin_config->pin = pin;
        pin_config->dir = mode;
        pin_config->pull = pull;

        write(device, buffer, 64);

        read(device, buffer, 64);

        return packet->pid;
    }

    int Usbduino::digitalWrite(unsigned int port, unsigned int pin, unsigned int value)
    {
        if (!device)
            return -ENODEV;

        packet = (PacketTypeDef *)buffer;

        packet->pid = PID_SET_GPIO_VAL;

        pin_config = (PinTypeDef *)packet->data;

        pin_config->val = value;
        pin_config->pin = pin;
        pin_config->port = port;

        write(device, buffer, 64);

        read(device, buffer, 64);

        return packet->pid;
    }

    int Usbduino::digitalRead(unsigned int port, unsigned int pin)
    {
        if (!device)
            return -ENODEV;

        packet = (PacketTypeDef *)buffer;

        packet->pid = PID_GET_GPIO_VAL;

        pin_config = (PinTypeDef *)packet->data;

        pin_config->pin = pin;
        pin_config->port = port;

        write(device, buffer, 64);

        read(device, buffer, 64);

        return pin_config->val;
    }

    int Usbduino::analogRead(unsigned int port, unsigned int pin)
    {
        if (!device)
            return -ENODEV;

        packet = (PacketTypeDef *)buffer;

        packet->pid = PID_GET_ADC_SINGLE;

        pin_config = (PinTypeDef *)packet->data;

        pin_config->port = port;
        pin_config->pin = pin;

        write(device, buffer, 64);

        read(device, buffer, 64);

        return pin_config->val;
    }

    void Usbduino::delay(unsigned int ms)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }

};