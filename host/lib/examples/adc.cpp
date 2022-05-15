#include "../include/usbd.h"

/*
    This program showcases how you can use the ADC and read the data from a temprature sensor LM35 
    connected to one of the ADC pins and display its value over the console.
*/

int main()
{
    USBD::Usbduino dev;

    if (dev.connect((char *)"/dev/usbd") < 0)
    {
        printf("Cannot open device!\nMake sure you have the usbd driver running and user has privilages to access it!\n");
        return 0;
    }

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
    }

    return 0;
}
