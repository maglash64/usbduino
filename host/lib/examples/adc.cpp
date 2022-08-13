#include "../include/usbd.h"

/*
    This program showcases how you can use the ADC and read the data from a LM35 temperature sensor 
    connected to one of the ADC pins and display its value over the console.
*/

#define ADC_PIN 28

int main()
{
    USBD::Usbduino dev;

    if (dev.connect() < 0)
    {
        printf("Cannot open device!\nMake sure you have the privilages to access it!\n");
        return 0;
    }

    dev.pinMode(ADC_PIN, USBD::GPIO_MODE_ANALOG);
    
    float f;
    int temp = 0;

    while (1)
    {
        f = dev.analogRead(ADC_PIN);
        f *= (3.3f / (1<<12));
        temp = (((f / 0.250f) * 25) * 0.5f) + (temp * 0.5f);
        printf("temperature: %d \n",temp);
        dev.delay(1);
    }

    dev.release();

    return 0;
}
