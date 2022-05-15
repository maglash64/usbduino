#!/bin/bash
export PICO_SDK_PATH=~/Work/pico/pico-sdk
cd bin
cmake ..
make clean
make -j16