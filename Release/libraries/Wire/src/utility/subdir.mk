################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:\Eclipse_Arduino\arduinoPlugin\packages\arduino\hardware\avr\1.6.20\libraries\Wire\src\utility\twi.c 

C_DEPS += \
.\libraries\Wire\src\utility\twi.c.d 

LINK_OBJ += \
.\libraries\Wire\src\utility\twi.c.o 


# Each subdirectory must supply rules for building sources it contributes
libraries\Wire\src\utility\twi.c.o: C:\Eclipse_Arduino\arduinoPlugin\packages\arduino\hardware\avr\1.6.20\libraries\Wire\src\utility\twi.c
	@echo 'Building file: $<'
	@echo 'Starting C compile'
	"C:\Eclipse_Arduino\/arduinoPlugin/packages/arduino/tools/avr-gcc/4.9.2-atmel3.5.4-arduino2/bin/avr-gcc" -c -g -Os -Wall -Wextra -std=gnu11 -ffunction-sections -fdata-sections -flto -fno-fat-lto-objects -mmcu=atmega328p -DF_CPU=16000000L -DARDUINO=10802 -DARDUINO_AVR_UNO -DARDUINO_ARCH_AVR   -I"C:\Eclipse_Arduino\arduinoPlugin\packages\arduino\hardware\avr\1.6.20\cores\arduino" -I"C:\Eclipse_Arduino\arduinoPlugin\packages\arduino\hardware\avr\1.6.20\variants\standard" -I"C:\Users\guillotl\Arduino\libraries\Bounce2" -I"C:\Users\guillotl\Arduino\libraries\Grove_SHT31" -I"C:\Users\guillotl\Arduino\libraries\MySensors" -I"C:\Users\guillotl\Arduino\libraries\SSD1306Ascii-master" -I"C:\Users\guillotl\Arduino\libraries\SSD1306Ascii-master\src" -I"C:\Eclipse_Arduino\arduinoPlugin\packages\arduino\hardware\avr\1.6.20\libraries\SPI" -I"C:\Eclipse_Arduino\arduinoPlugin\packages\arduino\hardware\avr\1.6.20\libraries\SPI\src" -I"C:\Eclipse_Arduino\arduinoPlugin\packages\arduino\hardware\avr\1.6.20\libraries\Wire" -I"C:\Eclipse_Arduino\arduinoPlugin\packages\arduino\hardware\avr\1.6.20\libraries\Wire\src" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 "$<"  -o  "$@"
	@echo 'Finished building: $<'
	@echo ' '


