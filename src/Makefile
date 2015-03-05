SRC = irmc.c w5100.c socket.c node.c esp8266.c
HFILE = irmc.h w5100.h
OBJ = ${SRC:.c=.out}
CC = avr-gcc
INCLUDE = -I/opt/local/avr/include/avr/ -I../lib/arduino_gcc_tutorial/arduino-lib
CFLAGS = -Wall -O -Wl,-Map,irmc.map -DF_CPU=16000000UL -mmcu=atmega328
PORT = /dev/tty.usbserial-A700dZIr  
#PORT = /dev/tty.usbserial-A400Y62O
LD = avr-objcopy

all: options irmc.hex 

options:
	@echo irmc build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

irmc.elf: ${SRC} ${HFILE}
	@echo CC ${SRC} -o irmc.out
	@${CC} ${INCLUDE} ${CFLAGS} ${SRC} -o $@
	avr-size $@

irmc.hex: irmc.elf
	avr-objcopy -O ihex -R .eeprom irmc.elf $@

burn: irmc.hex
	@echo burning 
	@avrdude -c stk500v1 -p m328p -b 57600 -P ${PORT} -U flash:w:irmc.hex

clean:
	@echo cleaning.
	@rm -f irmc.hex irmc.elf ${OBJ} irmc.map
