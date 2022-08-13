blink: examples/blink.cpp src/usbd.cpp
	g++ -o bin/blink src/usbd.cpp examples/blink.cpp -lusb-1.0

adc: examples/adc.cpp src/usbd.cpp
	g++ -o bin/adc src/usbd.cpp examples/adc.cpp -lusb-1.0

clean:
	@rm bin/*