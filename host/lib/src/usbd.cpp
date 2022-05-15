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

    int Usbduino::pinMode(uint32_t pin, uint32_t mode, uint32_t pull)
    {
        if (!device)
            return -ENODEV;

        packet = (PacketTypeDef *)buffer;

        packet->pid = PID_SET_GPIO;

        pin_config = (PinTypeDef *)packet->data;

        pin_config->pin = pin;
        pin_config->dir = mode;
        pin_config->pull = pull;

        write(device, buffer, 64);

        read(device, buffer, 64);

        return packet->pid;
    }

    int Usbduino::digitalWrite(uint32_t pin, uint32_t value)
    {
        if (!device)
            return -ENODEV;

        packet = (PacketTypeDef *)buffer;

        packet->pid = PID_SET_GPIO_VAL;

        pin_config = (PinTypeDef *)packet->data;

        pin_config->val = value;
        pin_config->pin = pin;

        write(device, buffer, 64);

        read(device, buffer, 64);

        return packet->pid;
    }

    int Usbduino::digitalRead(uint32_t pin)
    {
        if (!device)
            return -ENODEV;

        packet = (PacketTypeDef *)buffer;

        packet->pid = PID_GET_GPIO_VAL;

        pin_config = (PinTypeDef *)packet->data;

        pin_config->pin = pin;

        write(device, buffer, 64);

        read(device, buffer, 64);

        return pin_config->val;
    }

    int Usbduino::analogRead(uint32_t pin)
    {
        if (!device)
            return -ENODEV;

        packet = (PacketTypeDef *)buffer;

        packet->pid = PID_GET_ADC_SINGLE;

        pin_config = (PinTypeDef *)packet->data;

        pin_config->pin = pin;

        write(device, buffer, 64);

        read(device, buffer, 64);

        return pin_config->val;
    }

    int Usbduino::analogWrite(uint32_t pin, uint32_t value)
    {
        if (!device)
            return -ENODEV;

        packet = (PacketTypeDef *)buffer;

        packet->pid = PID_SET_DAC_SINGLE;

        pin_config = (PinTypeDef *)packet->data;

        pin_config->pin = pin;
        pin_config->val = value;

        write(device, buffer, 64);

        read(device, buffer, 64);

        return packet->pid;
    }

    int Usbduino::uartSetup(uint32_t baud,uint32_t tx_pin,uint32_t rx_pin,uint32_t device_id)
    {
        if (!device)
            return -ENODEV;

        packet = (PacketTypeDef *)buffer;

        packet->pid = PID_UART_SET_DEVICE;

        uart = (UARTTypeDef *)packet->data;

        uart->id = device_id;
        uart->active = 1;
        uart->baud = baud;
        uart->pin_tx = tx_pin;
        uart->pin_rx = rx_pin;

        write(device, buffer, 64);

        read(device, buffer, 64);

        return packet->pid;
    }

    int Usbduino::uartFree(uint32_t device_id)
    {
        if (!device)
            return -ENODEV;

        packet = (PacketTypeDef *)buffer;

        packet->pid = PID_UART_SET_DEVICE;

        uart = (UARTTypeDef *)packet->data;

        uart->id = device_id;
        uart->active = 0;

        write(device, buffer, 64);

        read(device, buffer, 64);

        return packet->pid;
    }

    int Usbduino::uartWrite(uint8_t *data, uint32_t length, uint32_t device_id)
    {
        uint32_t t = 0,i = 0;

        if (!device)
            return -ENODEV;

        packet = (PacketTypeDef *)buffer;

        packet->pid = PID_UART_TRANSMIT;
        packet->intr = device_id;

        if (length <= PID_MAX_LEN)
        {
            //if length of the data to transmit is less than 58 bytes.
            packet->length = length;

            for (uint32_t x = 0; x < length; x++)
                packet->data[x] = data[x];

            write(device, buffer, 64);

            read(device, buffer, 64);
        }
        else
        {
            t = length / PID_MAX_LEN;

            for(uint32_t x = 0; x < t - 1; x++)
            {
                packet->length = PID_MAX_LEN;

                for (uint32_t x = 0; x < PID_MAX_LEN; x++)
                {
                    packet->data[x] = data[i];
                    i++;
                }

                write(device, buffer, 64);

                read(device, buffer, 64);
            }

            packet->length = length - (t * PID_MAX_LEN);

            for (uint32_t x = 0; x < packet->length; x++)
            {
                packet->data[x] = data[i];
                i++;
            }

            write(device, buffer, 64);

            read(device, buffer, 64);
            
        }

        return packet->pid;
    }

    int Usbduino::uartRead(uint8_t *data, uint32_t *length, uint32_t device_id)
    {
        if (!device)
            return -ENODEV;

        packet = (PacketTypeDef *)buffer;

        packet->pid = PID_UART_RECIEVE;
        packet->intr = device_id;

        write(device, buffer, 64);

        read(device, buffer, 64);

        for(uint32_t x = 0;x < packet->length;x++)
            data[x] = packet->data[x];

        *length = packet->length;

        return packet->pid;
    }

    int Usbduino::i2cSetup(uint32_t clock,uint32_t sda_pin,uint32_t scl_pin,uint32_t device_id)
    {
        if (!device)
            return -ENODEV;

        packet = (PacketTypeDef *)buffer;

        packet->pid = PID_I2C_SET_DEVICE;

        i2c = (I2CTypeDef *)packet->data;

        i2c->id = device_id;
        i2c->active = 1;
        i2c->clock_freq = clock;
        i2c->pin_scl = scl_pin;
        i2c->pin_sda = sda_pin;

        write(device, buffer, 64);

        read(device, buffer, 64);

        return packet->pid;
    }

    int Usbduino::i2cFree(uint32_t device_id)
    {
        if (!device)
            return -ENODEV;

        packet = (PacketTypeDef *)buffer;

        packet->pid = PID_I2C_SET_DEVICE;

        i2c = (I2CTypeDef *)packet->data;

        i2c->id = device_id;
        i2c->active = 0;

        write(device, buffer, 64);

        read(device, buffer, 64);

        return packet->pid;
    }


    int Usbduino::i2cWrite(uint8_t address,uint8_t *data,uint32_t length,uint32_t device_id)
    {
        uint32_t t = 0,i = 0;

        if (!device)
            return -ENODEV;

        packet = (PacketTypeDef *)buffer;

        packet->pid = PID_I2C_TRANSMIT;
        packet->intr = (uint16_t)(device_id << 8) | address;

        if (length <= PID_MAX_LEN)
        {
            //if length of the data to transmit is less than 58 bytes.
            packet->length = length;

            for (uint32_t x = 0; x < length; x++)
                packet->data[x] = data[x];

            write(device, buffer, 64);

            read(device, buffer, 64);
        }
        else
        {
            return -EINVAL;
        }

        return packet->pid;
    }

    int Usbduino::i2cRead(uint8_t address,uint8_t *data,uint32_t *length,uint32_t device_id)
    {
        if (!device)
            return -ENODEV;

        packet = (PacketTypeDef *)buffer;

        packet->pid = PID_I2C_RECIEVE;
        packet->intr = (uint16_t)(device_id << 8) | address;

        write(device, buffer, 64);

        read(device, buffer, 64);

        for(uint32_t x = 0;x < packet->length;x++)
            data[x] = packet->data[x];

        *length = packet->length;

        return packet->pid;
    }


    int Usbduino::spiSetup(uint32_t clock,uint32_t tx_pin,uint32_t rx_pin,uint32_t clk_pin,uint32_t device_id)
    {
        if (!device)
            return -ENODEV;

        packet = (PacketTypeDef *)buffer;

        packet->pid = PID_SPI_SET_DEVICE;

        spi = (SPITypeDef *)packet->data;

        spi->id = device_id;
        spi->active = 1;
        spi->clock_freq = clock;
        spi->pin_tx = tx_pin;
        spi->pin_rx = rx_pin;
        spi->pin_clk = clk_pin;

        write(device, buffer, 64);

        read(device, buffer, 64);

        return packet->pid;
    }

    int Usbduino::spiFree(uint32_t device_id)
    {
        if (!device)
            return -ENODEV;

        packet = (PacketTypeDef *)buffer;

        packet->pid = PID_SPI_SET_DEVICE;

        spi = (SPITypeDef *)packet->data;

        spi->id = device_id;
        spi->active = 0;

        write(device, buffer, 64);

        read(device, buffer, 64);

        return packet->pid;
    }


    int Usbduino::spiWrite(uint8_t *data,uint32_t length,uint32_t device_id)
    {
        uint32_t t = 0,i = 0;

        if (!device)
            return -ENODEV;

        packet = (PacketTypeDef *)buffer;

        packet->pid = PID_SPI_TRANSMIT;
        packet->intr = device_id;

        if (length <= PID_MAX_LEN)
        {
            //if length of the data to transmit is less than 58 bytes.
            packet->length = length;

            for (uint32_t x = 0; x < length; x++)
                packet->data[x] = data[x];

            write(device, buffer, 64);

            read(device, buffer, 64);
        }
        else
        {
            return -EINVAL;
        }

        return packet->pid;
    }

    int Usbduino::spiRead(uint8_t *data,uint32_t *length,uint32_t device_id)
    {
        if (!device)
            return -ENODEV;

        packet = (PacketTypeDef *)buffer;

        packet->pid = PID_SPI_RECIEVE;
        packet->intr = device_id;

        write(device, buffer, 64);

        read(device, buffer, 64);

        for(uint32_t x = 0;x < packet->length;x++)
            data[x] = packet->data[x];

        *length = packet->length;

        return packet->pid;
    }

    void Usbduino::delay(uint32_t ms)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }

};