#!/bin/bash

##
##  This script compiles the buggy firmware and uploads it to the microcontroller
##  Run by executing "./build_and_upload_macos.sh" from the root of the project
##

# compile the firmware
make

# upload the firmware and reset the microcontroller
'/Applications/STMicroelectronics/STM32Cube/STM32CubeProgrammer/STM32CubeProgrammer.app/Contents/MacOs/bin/STM32_Programmer_CLI' \
    -c port=SWD freq=4000 reset=HWrst -w ./build/aml-bvat-acc.elf -rst