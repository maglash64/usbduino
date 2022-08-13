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
        {
            libusb_release_interface(device, 0x0);

            libusb_close(device);

            libusb_exit(NULL);
        }
    }

    int Usbduino::connect()
    {
        if (libusb_init(NULL) < 0)
        {
            return -1;
        }

        device = libusb_open_device_with_vid_pid(NULL, 0x0483, 0xFF11);

        if (!device)
        {
            libusb_exit(NULL);
            return -2;
        }

        if (libusb_claim_interface(device, 0x0) < 0)
        {
            libusb_close(device);
            libusb_exit(NULL);
            return -2;
        }

        is_open = 1;
        return 0;
    }

    int Usbduino::release()
    {
        if (is_open)
        {
            libusb_release_interface(device, 0x0);

            libusb_close(device);

            libusb_exit(NULL);
        }
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

        libusb_bulk_transfer(device,LIBUSB_ENDPOINT_OUT | 0x1,buffer,0x40,&len,1);

        libusb_bulk_transfer(device,LIBUSB_ENDPOINT_IN | 0x1,buffer,0x40,&len,1);

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

        libusb_bulk_transfer(device,LIBUSB_ENDPOINT_OUT | 0x1,buffer,0x40,&len,1);

        libusb_bulk_transfer(device,LIBUSB_ENDPOINT_IN | 0x1,buffer,0x40,&len,1);

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

        libusb_bulk_transfer(device,LIBUSB_ENDPOINT_OUT | 0x1,buffer,0x40,&len,1);

        libusb_bulk_transfer(device,LIBUSB_ENDPOINT_IN | 0x1,buffer,0x40,&len,1);

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

        libusb_bulk_transfer(device,LIBUSB_ENDPOINT_OUT | 0x1,buffer,0x40,&len,1);

        libusb_bulk_transfer(device,LIBUSB_ENDPOINT_IN | 0x1,buffer,0x40,&len,1);

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

        libusb_bulk_transfer(device,LIBUSB_ENDPOINT_OUT | 0x1,buffer,0x40,&len,1);

        libusb_bulk_transfer(device,LIBUSB_ENDPOINT_IN | 0x1,buffer,0x40,&len,1);

        return packet->pid;
    }

    int Usbduino::uartSetup(uint32_t baud, uint32_t tx_pin, uint32_t rx_pin, uint32_t device_id)
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

        libusb_bulk_transfer(device,LIBUSB_ENDPOINT_OUT | 0x1,buffer,0x40,&len,1);

        libusb_bulk_transfer(device,LIBUSB_ENDPOINT_IN | 0x1,buffer,0x40,&len,1);

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

        libusb_bulk_transfer(device,LIBUSB_ENDPOINT_OUT | 0x1,buffer,0x40,&len,1);

        libusb_bulk_transfer(device,LIBUSB_ENDPOINT_IN | 0x1,buffer,0x40,&len,1);

        return packet->pid;
    }

    int Usbduino::uartWrite(uint8_t *data, uint32_t length, uint32_t device_id)
    {
        uint32_t t = 0, i = 0;

        if (!device)
            return -ENODEV;

        packet = (PacketTypeDef *)buffer;

        packet->pid = PID_UART_TRANSMIT;
        packet->intr = device_id;

        if (length <= PID_MAX_LEN)
        {
            // if length of the data to transmit is less than 58 bytes.
            packet->length = length;

            for (uint32_t x = 0; x < length; x++)
                packet->data[x] = data[x];

            libusb_bulk_transfer(device,LIBUSB_ENDPOINT_OUT | 0x1,buffer,0x40,&len,1);

            libusb_bulk_transfer(device,LIBUSB_ENDPOINT_IN | 0x1,buffer,0x40,&len,1);
        }
        else
        {
            t = length / PID_MAX_LEN;

            for (uint32_t x = 0; x < t - 1; x++)
            {
                packet->length = PID_MAX_LEN;

                for (uint32_t x = 0; x < PID_MAX_LEN; x++)
                {
                    packet->data[x] = data[i];
                    i++;
                }

                libusb_bulk_transfer(device,LIBUSB_ENDPOINT_OUT | 0x1,buffer,0x40,&len,1);

                libusb_bulk_transfer(device,LIBUSB_ENDPOINT_IN | 0x1,buffer,0x40,&len,1);
            }

            packet->length = length - (t * PID_MAX_LEN);

            for (uint32_t x = 0; x < packet->length; x++)
            {
                packet->data[x] = data[i];
                i++;
            }

            libusb_bulk_transfer(device,LIBUSB_ENDPOINT_OUT | 0x1,buffer,0x40,&len,1);

            libusb_bulk_transfer(device,LIBUSB_ENDPOINT_IN | 0x1,buffer,0x40,&len,1);
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

        libusb_bulk_transfer(device,LIBUSB_ENDPOINT_OUT | 0x1,buffer,0x40,&len,1);

        libusb_bulk_transfer(device,LIBUSB_ENDPOINT_IN | 0x1,buffer,0x40,&len,1);

        for (uint32_t x = 0; x < packet->length; x++)
            data[x] = packet->data[x];

        *length = packet->length;

        return packet->pid;
    }

    int Usbduino::i2cSetup(uint32_t clock, uint32_t sda_pin, uint32_t scl_pin, uint32_t device_id)
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

        libusb_bulk_transfer(device,LIBUSB_ENDPOINT_OUT | 0x1,buffer,0x40,&len,1);

        libusb_bulk_transfer(device,LIBUSB_ENDPOINT_IN | 0x1,buffer,0x40,&len,1);

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

        libusb_bulk_transfer(device,LIBUSB_ENDPOINT_OUT | 0x1,buffer,0x40,&len,1);

        libusb_bulk_transfer(device,LIBUSB_ENDPOINT_IN | 0x1,buffer,0x40,&len,1);

        return packet->pid;
    }

    int Usbduino::i2cWrite(uint8_t address, uint8_t *data, uint32_t length, uint32_t device_id)
    {
        uint32_t t = 0, i = 0;

        if (!device)
            return -ENODEV;

        packet = (PacketTypeDef *)buffer;

        packet->pid = PID_I2C_TRANSMIT;
        packet->intr = (uint16_t)(device_id << 8) | address;

        if (length <= PID_MAX_LEN)
        {
            // if length of the data to transmit is less than 58 bytes.
            packet->length = length;

            for (uint32_t x = 0; x < length; x++)
                packet->data[x] = data[x];

            libusb_bulk_transfer(device,LIBUSB_ENDPOINT_OUT | 0x1,buffer,0x40,&len,1);

            libusb_bulk_transfer(device,LIBUSB_ENDPOINT_IN | 0x1,buffer,0x40,&len,1);
        }
        else
        {
            return -EINVAL;
        }

        return packet->pid;
    }

    int Usbduino::i2cRead(uint8_t address, uint8_t *data, uint32_t *length, uint32_t device_id)
    {
        if (!device)
            return -ENODEV;

        packet = (PacketTypeDef *)buffer;

        packet->pid = PID_I2C_RECIEVE;
        packet->intr = (uint16_t)(device_id << 8) | address;

        libusb_bulk_transfer(device,LIBUSB_ENDPOINT_OUT | 0x1,buffer,0x40,&len,1);

        libusb_bulk_transfer(device,LIBUSB_ENDPOINT_IN | 0x1,buffer,0x40,&len,1);

        for (uint32_t x = 0; x < packet->length; x++)
            data[x] = packet->data[x];

        *length = packet->length;

        return packet->pid;
    }

    int Usbduino::spiSetup(uint32_t clock, uint32_t tx_pin, uint32_t rx_pin, uint32_t clk_pin, uint32_t device_id)
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

        libusb_bulk_transfer(device,LIBUSB_ENDPOINT_OUT | 0x1,buffer,0x40,&len,1);

        libusb_bulk_transfer(device,LIBUSB_ENDPOINT_IN | 0x1,buffer,0x40,&len,1);

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

        libusb_bulk_transfer(device,LIBUSB_ENDPOINT_OUT | 0x1,buffer,0x40,&len,1);

        libusb_bulk_transfer(device,LIBUSB_ENDPOINT_IN | 0x1,buffer,0x40,&len,1);

        return packet->pid;
    }

    int Usbduino::spiWrite(uint8_t *data, uint32_t length, uint32_t device_id)
    {
        uint32_t t = 0, i = 0;

        if (!device)
            return -ENODEV;

        packet = (PacketTypeDef *)buffer;

        packet->pid = PID_SPI_TRANSMIT;
        packet->intr = device_id;

        if (length <= PID_MAX_LEN)
        {
            // if length of the data to transmit is less than 58 bytes.
            packet->length = length;

            for (uint32_t x = 0; x < length; x++)
                packet->data[x] = data[x];

            libusb_bulk_transfer(device,LIBUSB_ENDPOINT_OUT | 0x1,buffer,0x40,&len,1);

            libusb_bulk_transfer(device,LIBUSB_ENDPOINT_IN | 0x1,buffer,0x40,&len,1);
        }
        else
        {
            return -EINVAL;
        }

        return packet->pid;
    }

    int Usbduino::spiRead(uint8_t *data, uint32_t *length, uint32_t device_id)
    {
        if (!device)
            return -ENODEV;

        packet = (PacketTypeDef *)buffer;

        packet->pid = PID_SPI_RECIEVE;
        packet->intr = device_id;

        libusb_bulk_transfer(device,LIBUSB_ENDPOINT_OUT | 0x1,buffer,0x40,&len,1);

        libusb_bulk_transfer(device,LIBUSB_ENDPOINT_IN | 0x1,buffer,0x40,&len,1);

        for (uint32_t x = 0; x < packet->length; x++)
            data[x] = packet->data[x];

        *length = packet->length;

        return packet->pid;
    }

    void Usbduino::delay(uint32_t ms)
    {   
        if(ms)
            std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }

};