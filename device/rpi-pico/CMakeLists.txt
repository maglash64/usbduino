cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(usbduino_project C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(usbduino)

target_sources(usbduino PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/usbduino.c
        ${CMAKE_CURRENT_LIST_DIR}/usb_descriptors.c
        ${CMAKE_CURRENT_LIST_DIR}/bsp.c
        )

target_include_directories(usbduino PUBLIC
        ${CMAKE_CURRENT_LIST_DIR})

pico_enable_stdio_uart(usbduino 1)

pico_add_extra_outputs(usbduino)

target_link_libraries(usbduino PUBLIC pico_stdlib tinyusb_device tinyusb_board hardware_adc hardware_i2c hardware_spi hardware_flash hardware_resets hardware_irq)