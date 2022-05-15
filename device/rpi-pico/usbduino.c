#include "usbduino.h"

int ret;

uint8_t usb_buffer[64];
uint8_t usb_rx_ready;

uint8_t uart0_rx_buffer[58];
uint8_t uart0_rx_count;

uint8_t uart1_rx_buffer[58];
uint8_t uart1_rx_count;

void on_uart0_rx()
{
    while (uart_is_readable(uart0))
    {
        if (uart0_rx_count < 58)
        {
            uart0_rx_buffer[uart0_rx_count] = uart_getc(uart0);
            uart0_rx_count++;
        }
        else
        {
            uart0_rx_count = 0;
        }
    }
}

void on_uart1_rx()
{
    while (uart_is_readable(uart1))
    {
        if (uart1_rx_count < 58)
        {
            uart1_rx_buffer[uart1_rx_count] = uart_getc(uart1);
            uart1_rx_count++;
        }
        else
        {
            uart1_rx_count = 0;
        }
    }
}

int main(void)
{
    board_init();
    tusb_init();
    
    adc_init();
    
    while (1)
    {
        tud_task(); // tinyusb device task

        if (tud_suspended())
        {
            // Wake up host if we are in suspend mode
            // and REMOTE_WAKEUP feature is enabled by host
            tud_remote_wakeup();
        }

        if (usb_rx_ready)
        {
            packet = (struct PacketTypeDef *)usb_buffer;

            switch (packet->pid)
            {
            case PID_DEVICE:
            {
                //THE DEVICE IS RPI-PICO
                packet->length = 2;
                packet->data[0] = 0xFF;
                packet->data[1] = 0x11;
            }break;

            case PID_SET_GPIO:
            {
                pin_config = (struct PinTypeDef *)packet->data;

                gpio_init(pin_config->pin);

                switch (pin_config->dir)
                {
                case GPIO_MODE_INPUT:
                    gpio_set_dir(pin_config->pin, GPIO_IN);
                    break;
                case GPIO_MODE_OUTPUT:
                    gpio_set_dir(pin_config->pin, GPIO_OUT);
                    break;
                case GPIO_MODE_ANALOG:
                    if(pin_config->pin >= 26 && pin_config->pin <= 28)
                        adc_gpio_init(pin_config->pin);
                    break;
                default:
                    break;
                }

                if (pin_config->pull)
                {
                    if (pin_config->pull == 1)
                        gpio_pull_up(pin_config->pin);
                    if (pin_config->pull == 2)
                        gpio_pull_down(pin_config->pin);
                }else
                    gpio_set_pulls(pin_config->pin,false,false);

                packet->pid = PID_OK;

                tud_vendor_write(usb_buffer, 0x40);
            }break;

            case PID_SET_GPIO_VAL:
            {
                pin_config = (struct PinTypeDef *)packet->data;

                gpio_put(pin_config->pin, pin_config->val);
                packet->pid = PID_OK;

                tud_vendor_write(usb_buffer, 0x40);
            }break;

            case PID_GET_GPIO_VAL:
            {
                pin_config = (struct PinTypeDef *)packet->data;

                // read from a digital pin..
                pin_config->val = gpio_get(pin_config->pin);
                packet->pid = PID_OK;

                tud_vendor_write(usb_buffer, 0x40);
            }break;

            case PID_GET_ADC_SINGLE:
            {
                pin_config = (struct PinTypeDef *)packet->data;

                if(pin_config->pin >= 26 && pin_config->pin <= 28)
                {
                    adc_select_input(pin_config->pin - 26);
                    
                    pin_config->val = adc_read();

                    packet->pid = PID_OK;
                }else
                {
                    packet->pid = PID_ERROR;
                }
                
                tud_vendor_write(usb_buffer, 0x40);
            }break;

            case PID_UART_SET_DEVICE:
            {
                uart_config = (struct UARTTypeDef *)packet->data;

                if (uart_config->pin_tx == 0 && uart_config->pin_rx == 0)
                {
                    if (uart_config->id == 0)
                    {
                        gpio_set_function(17, GPIO_FUNC_UART);
                        gpio_set_function(16, GPIO_FUNC_UART);
                    }
                    else
                    {
                        gpio_set_function(4, GPIO_FUNC_UART);
                        gpio_set_function(5, GPIO_FUNC_UART);
                    }
                }
                else
                {
                    gpio_set_function(uart_config->pin_tx, GPIO_FUNC_UART);
                    gpio_set_function(uart_config->pin_rx, GPIO_FUNC_UART);
                }

                //if device is active the init it or else deinit..
                if (uart_config->active)
                {
                    if (uart_config->id == 0)
                    {
                        uart_init(uart0, uart_config->baud);

                        uart_set_hw_flow(uart0, false, false);

                        uart_set_format(uart0, 8, 1, UART_PARITY_NONE);

                        uart_set_fifo_enabled(uart0, false);

                        irq_set_exclusive_handler(UART0_IRQ, on_uart0_rx);

                        irq_set_enabled(UART0_IRQ, true);

                        uart_set_irq_enables(uart0, true, false);

                        packet->pid = PID_OK;
                    }
                    else if (uart_config->id == 1)
                    {
                        uart_init(uart1, uart_config->baud);

                        uart_set_hw_flow(uart1, false, false);

                        uart_set_format(uart1, 8, 1, UART_PARITY_NONE);

                        uart_set_fifo_enabled(uart1, false);

                        irq_set_exclusive_handler(UART1_IRQ, on_uart0_rx);

                        irq_set_enabled(UART1_IRQ, true);

                        uart_set_irq_enables(uart1, true, false);

                        packet->pid = PID_OK;
                    }
                    else
                    {
                        packet->pid = PID_ERROR;
                    }
                }
                else
                {
                    if (uart_config->id == 0)
                    {
                        uart_deinit(uart0);

                        irq_set_enabled(UART0_IRQ, false);

                        uart_set_irq_enables(uart0, false, false);

                        packet->pid = PID_OK;
                    }
                    else if (uart_config->id == 1)
                    {
                        uart_deinit(uart1);

                        irq_set_enabled(UART1_IRQ, false);

                        uart_set_irq_enables(uart1, false, false);

                        packet->pid = PID_OK;
                    }
                    else
                    {
                        packet->pid = PID_ERROR;
                    }

                    gpio_set_function(uart_config->pin_tx, GPIO_FUNC_NULL);
                    gpio_set_function(uart_config->pin_rx, GPIO_FUNC_NULL);
                }

                tud_vendor_write(usb_buffer, 0x40);
            }break;

            case PID_UART_TRANSMIT:
            {
                packet->pid = PID_OK;

                if (packet->length > 58)
                {
                    packet->pid = PID_ERROR;
                    tud_vendor_write(usb_buffer, 0x40);
                    break;
                }

                if (packet->intr == 0)
                {
                    uart_puts(uart0, packet->data);
                }
                else if (packet->intr == 1)
                {
                    uart_puts(uart1, packet->data);
                }
                else
                {
                    packet->pid = PID_ERROR;
                }

                tud_vendor_write(usb_buffer, 0x40);
            }break;

            case PID_UART_RECIEVE:
            {
                if (packet->intr == 0)
                {
                    memset(packet->data,0,PID_MAX_LEN);
                    memcpy(packet->data, uart0_rx_buffer, uart0_rx_count);
                    packet->length = uart0_rx_count;

                    // reset the rx count
                    uart0_rx_count = 0;

                    packet->pid = PID_OK;
                }
                else if (packet->intr == 1)
                {
                    memset(packet->data,0,PID_MAX_LEN);
                    memcpy(packet->data, uart1_rx_buffer, uart1_rx_count);
                    packet->length = uart1_rx_count;

                    // reset the rx count
                    uart1_rx_count = 0;

                    packet->pid = PID_OK;
                }
                else
                {
                    packet->pid = PID_ERROR;
                }

                tud_vendor_write(usb_buffer, 0x40);
            }break;

            case PID_I2C_SET_DEVICE:
            {
                i2c_config = (struct I2CTypeDef *)packet->data;

                if (i2c_config->pin_scl == 0 && i2c_config->pin_sda == 0)
                {
                    if (i2c_config->id == 0)
                    {
                        gpio_set_function(6, GPIO_FUNC_I2C);
                        gpio_set_function(7, GPIO_FUNC_I2C);
                        gpio_pull_up(6);
                        gpio_pull_up(7);
                    }
                    else
                    {
                        gpio_set_function(4, GPIO_FUNC_I2C);
                        gpio_set_function(5, GPIO_FUNC_I2C);
                        gpio_pull_up(4);
                        gpio_pull_up(5);
                    }
                }
                else
                {
                    gpio_set_function(i2c_config->pin_sda, GPIO_FUNC_I2C);
                    gpio_set_function(i2c_config->pin_scl, GPIO_FUNC_I2C);
                    gpio_pull_up(i2c_config->pin_sda);
                    gpio_pull_up(i2c_config->pin_scl);
                }

                //if active we want to initialise the device..else deinit.
                if (i2c_config->active)
                {
                    if (i2c_config->id == 0)
                    {
                        i2c_init(i2c0, i2c_config->clock_freq);

                        packet->pid = PID_OK;
                    }
                    else if (i2c_config->id == 1)
                    {
                        i2c_init(i2c1, i2c_config->clock_freq);

                        packet->pid = PID_OK;
                    }
                    else
                    {
                        packet->pid = PID_ERROR;
                    }
                }
                else
                {
                    if (i2c_config->id == 0)
                    {
                        i2c_deinit(i2c0);

                        //need top free pull ups on i2c bus here

                        packet->pid = PID_OK;
                    }
                    else if (i2c_config->id == 1)
                    {
                        i2c_deinit(i2c1);

                        //need top free pull ups on i2c bus here

                        packet->pid = PID_OK;
                    }
                    else
                    {
                        packet->pid = PID_ERROR;
                    }

                    gpio_set_pulls(i2c_config->pin_scl, false,false);
                    gpio_set_pulls(i2c_config->pin_sda, false,false);

                    gpio_set_function(i2c_config->pin_scl, GPIO_FUNC_NULL);
                    gpio_set_function(i2c_config->pin_sda, GPIO_FUNC_NULL);
                }

                tud_vendor_write(usb_buffer, 0x40);
            }break;

            case PID_I2C_TRANSMIT:
            {
                packet->pid = PID_OK;

                if (packet->length > 58)
                {
                    packet->pid = PID_ERROR;
                    tud_vendor_write(usb_buffer, 0x40);
                    break;
                }

                if ((packet->intr >> 8) == 0)
                {
                    ret = i2c_write_blocking(i2c0,(uint8_t)(packet->intr),packet->data,packet->length,false);
                }
                else if ((packet->intr >> 8)  == 1)
                {
                    ret = i2c_write_blocking(i2c1,(uint8_t)(packet->intr),packet->data,packet->length,false);
                }
                else
                {
                    packet->pid = PID_ERROR;
                }

                if(ret == PICO_ERROR_GENERIC || ret == PICO_ERROR_TIMEOUT)
                {
                    packet->pid = PID_ERROR;
                }

                tud_vendor_write(usb_buffer, 0x40);
            }break;

            case PID_I2C_RECIEVE:
            {
                packet->pid = PID_OK;

                if (packet->length > 58)
                {
                    packet->pid = PID_ERROR;
                    tud_vendor_write(usb_buffer, 0x40);
                    break;
                }

                if ((packet->intr >> 8) == 0)
                {
                    ret = i2c_read_blocking(i2c0,(uint8_t)(packet->intr),packet->data,packet->length,false);
                }
                else if ((packet->intr >> 8)  == 1)
                {
                    ret = i2c_read_blocking(i2c1,(uint8_t)(packet->intr),packet->data,packet->length,false);
                }
                else
                {
                    packet->pid = PID_ERROR;
                }
                
                if(ret == PICO_ERROR_GENERIC || ret == PICO_ERROR_TIMEOUT)
                {
                    packet->pid = PID_ERROR;
                }

                tud_vendor_write(usb_buffer, 0x40);
            }break;

            case PID_SPI_SET_DEVICE:
            {
                spi_config = (struct SPITypeDef *)packet->data;

                if (spi_config->pin_tx == 0 && spi_config->pin_rx == 0 && spi_config->pin_clk == 0)
                {
                    if (spi_config->id == 0)
                    {
                        gpio_set_function(4, GPIO_FUNC_SPI);
                        gpio_set_function(5, GPIO_FUNC_SPI);
                        gpio_set_function(6, GPIO_FUNC_SPI);
                    }
                    else
                    {
                        gpio_set_function(14, GPIO_FUNC_SPI);
                        gpio_set_function(15, GPIO_FUNC_SPI);
                        gpio_set_function(16, GPIO_FUNC_SPI);
                    }
                }
                else
                {
                    gpio_set_function(spi_config->pin_tx, GPIO_FUNC_SPI);
                    gpio_set_function(spi_config->pin_rx, GPIO_FUNC_SPI);
                    gpio_set_function(spi_config->pin_clk, GPIO_FUNC_SPI);
                }

                //if active we want to initialise the device..else deinit.
                if (spi_config->active)
                {
                    if (spi_config->id == 0)
                    {
                        spi_init(spi0, spi_config->clock_freq);

                        packet->pid = PID_OK;
                    }
                    else if (spi_config->id == 1)
                    {
                        spi_init(spi1, spi_config->clock_freq);

                        packet->pid = PID_OK;
                    }
                    else
                    {
                        packet->pid = PID_ERROR;
                    }
                }
                else
                {
                    if (spi_config->id == 0)
                    {
                        spi_deinit(spi0);

                        //need top free pull ups on i2c bus here

                        packet->pid = PID_OK;
                    }
                    else if (spi_config->id == 1)
                    {
                        spi_deinit(spi1);

                        //need top free pull ups on i2c bus here

                        packet->pid = PID_OK;
                    }
                    else
                    {
                        packet->pid = PID_ERROR;
                    }

                    gpio_set_function(spi_config->pin_tx, GPIO_FUNC_NULL);
                    gpio_set_function(spi_config->pin_rx, GPIO_FUNC_NULL);
                    gpio_set_function(spi_config->pin_clk, GPIO_FUNC_NULL);
                }

                tud_vendor_write(usb_buffer, 0x40);
            }break;


            case PID_SPI_TRANSMIT:
            {
                packet->pid = PID_OK;

                if (packet->length > 58)
                {
                    packet->pid = PID_ERROR;
                    tud_vendor_write(usb_buffer, 0x40);
                    break;
                }

                if (packet->intr == 0)
                {
                    ret = spi_write_blocking(spi0,packet->data,packet->length);
                }
                else if (packet->intr  == 1)
                {
                    ret = spi_write_blocking(spi1,packet->data,packet->length);
                }
                else
                {
                    packet->pid = PID_ERROR;
                }

                if(ret == PICO_ERROR_GENERIC || ret == PICO_ERROR_TIMEOUT)
                {
                    packet->pid = PID_ERROR;
                }

                tud_vendor_write(usb_buffer, 0x40);
            }break;

            case PID_SPI_RECIEVE:
            {
                packet->pid = PID_OK;

                if (packet->length > 58)
                {
                    packet->pid = PID_ERROR;
                    tud_vendor_write(usb_buffer, 0x40);
                    break;
                }

                if (packet->intr == 0)
                {
                    ret = spi_read_blocking(spi0,0,packet->data,packet->length);
                }
                else if (packet->intr  == 1)
                {
                    ret = spi_read_blocking(spi1,0,packet->data,packet->length);
                }
                else
                {
                    packet->pid = PID_ERROR;
                }
                
                if(ret == PICO_ERROR_GENERIC || ret == PICO_ERROR_TIMEOUT)
                {
                    packet->pid = PID_ERROR;
                }

                tud_vendor_write(usb_buffer, 0x40);
            }break;

            default:
            {
                packet->pid = PID_ERROR;

                tud_vendor_write(usb_buffer, 0x40);
            }break;

            }
            usb_rx_ready = 0;
        }
    }

    return 0;
}

void tud_mount_cb(void)
{
}

void tud_umount_cb(void)
{
}

void tud_suspend_cb(bool remote_wakeup_en)
{
    (void)remote_wakeup_en;
}

void tud_resume_cb(void)
{

}

void tud_vendor_rx_cb(uint8_t itf)
{
    tud_vendor_read(usb_buffer, 64);
    usb_rx_ready = 1;
}
