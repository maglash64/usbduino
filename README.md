# usbduino
Allows you to write code on host(linux desktop) and control a microcontroller connected to the host over USB. 

Currently working for raspberry pi pico.

Still didn't get what it does? Let me explain.

Below are some of the scenarios where this will be helpful.

Suppose you want to write a GUI application for a Desktop Computer(or an Embedded Computer like RPi!) and then want to control some hardware such as 
a relay using a button in the GUI, maybe you have a microcontroller board at your disposal such as raspberry pi pico.

How can you do it?

Well there are many ways to achieve this and one of them is to download this contraption that i have made and use it.

Currently working for raspberry pi pico, just go to the build folder and flash the usbduino.uf2 to your pico.

goto host/lib/examples and look at some examples..

you can build the blink example by running make blink from usbduino/host/lib

now run the example from bin/blink with sudo.

now you are ready to go!

You can use the usbd.h header in your project to read and write over GPIOs, ADC, UART, I2C, SPI !
