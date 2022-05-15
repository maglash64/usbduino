#include "../include/usbd.h"

/*
    This program showcases how you can use the GPIOs to blink the LED present on board.
*/

#define USER_LED 25

int main()
{
    USBD::Usbduino dev;

    if (dev.connect((char *)"/dev/usbd") < 0)
    {
        printf("Cannot open device!\nMake sure you have the usbd driver running and user has privilages to access it!\n");
        return 0;
    }

    dev.pinMode(USER_LED, USBD::GPIO_MODE_OUTPUT);

    while (1)
    {
        dev.digitalWrite(USER_LED, 1);
        dev.delay(500);
        dev.digitalWrite(USER_LED, 0);
        dev.delay(500);
    }

    return 0;
}
