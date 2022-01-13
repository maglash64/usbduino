#include "../include/usbd.h"

void db(int i)
{
    printf("%d\n",i);
}

int main()
{
    USBD::Usbduino dev;
    int time = 50;
   
    if(dev.connect((char*)"/dev/usbd") < 0)
    {
        printf("Cannot open device!\n");
        return 0;
    }

    dev.pinMode(PORTB,GPIO_PIN_4,GPIO_MODE_OUTPUT);

    while(1)
    {
        //printf("input: %s\n",dev.digitalRead(PORTB,GPIO_PIN_4) ? "HIGH" : "LOW");
        dev.digitalWrite(PORTB,GPIO_PIN_4,1);
        dev.delay(time);
        dev.digitalWrite(PORTB,GPIO_PIN_4,0);
        dev.delay(time);
    }
    return 0;
}

