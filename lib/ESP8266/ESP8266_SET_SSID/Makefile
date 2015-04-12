CXX=avr-g++
CC=avr-gcc
INCLUDE=-I/Applications/Arduino.app//Contents/Resources/Java/hardware/arduino/avr/cores/arduino/ -I/Applications/Arduino.app/Contents/Resources/Java/hardware/arduino/avr/libraries/SoftwareSerial -I/opt/local/avr/include/avr/
LIBS=-L ~/lib -lm -larduino
MCU=-mmcu=atmega328
CPU_SPEED=-DF_CPU=16000000UL
CFLAGS=$(MCU) $(CPU_SPEED) -Os -w -Wl,--gc-sections -ffunction-sections -fdata-sections
#PORT=/dev/cuaU0 # FreeBSD
#ifeq ($(shell uname),Linux)
#   PORT=/dev/ttyACM0
#endif
PORT = /dev/tty.usbserial-A400Y62O



SRC = ESP8266_SET_SSID.c
HFILE = 
OBJ = ${SRC:.c=.out}
LD = avr-objcopy

all: options irmc.hex 

options:
	@echo irmc build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

irmc.elf: ${SRC} ${HFILE}
	@echo CC ${SRC} -o irmc.out
	@${CC} ${CFLAGS} ${INCLUDE} ${SRC} -o $@ ${LIBS}
	avr-size $@

irmc.hex: irmc.elf
	avr-objcopy -O ihex -R .eeprom irmc.elf $@

burn: irmc.hex
	@echo burning 
	@avrdude -c stk500v1 -p m328p -b 57600 -P ${PORT} -U flash:w:irmc.hex

clean:
	@echo cleaning.
	@rm -f irmc.hex irmc.elf ${OBJ} irmc.map
