#include "../include/usbd.h"

/*
    This program showcases how you can use the GPIOs to blink the LED present on board.
*/

#define USER_LED 25

int main()
{
    USBD::Usbduino dev;

    if (dev.connect() < 0)
    {
        printf("Cannot open device!\nMake sure you have the privilages to access it!\n");
        return 0;
    }

    dev.pinMode(USER_LED, USBD::GPIO_MODE_OUTPUT);

    while (1)
    {
        dev.digitalWrite(USER_LED, 1);
        dev.delay(50);
        dev.digitalWrite(USER_LED, 0);
        dev.delay(50);
    }

    dev.release();

    return 0;
}
