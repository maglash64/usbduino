#include "../include/usbd.h"

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

    dev.pinMode(28, USBD::GPIO_MODE_ANALOG);

    float f;
    int temp = 0;

    while (1)
    {
        
        f = dev.analogRead(28);
        f *= (3.3f / (1<<12));
        temp = (((f / 0.250f) * 25) * 0.5f) + (temp * 0.5f);
        printf("adc: %d \n",temp);
        dev.delay(1);
        

        // dev.digitalWrite(USER_LED, 1);
        // dev.delay(1000);
        // dev.digitalWrite(USER_LED, 0);
        // dev.delay(1000);
    }

    return 0;
}
