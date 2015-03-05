CC=avr-gcc
CXX=avr-g++
MCU=-mmcu=atmega328p 
CPU_SPEED=-DF_CPU=16000000UL
CFLAGS=$(MCU) $(CPU_SPEED) -Os -w -I./
BOARD=arduino
LIBNAME=lib$(BOARD).a
#INCDIR=#~/include/$(BOARD)

OBJECTS= pins_arduino.o wiring.o wiring_analog.o wiring_digital.o \
		   wiring_pulse.o wiring_shift.o Print.o HardwareSerial.o  \
		   Tone.o WMath.o WString.o WInterrupts.o forward.o

default: $(OBJECTS)
	avr-ar rcs $(LIBNAME) $^
#	mkdir -p $(INCDIR)
#	mkdir -p ~/lib/
#	cp *.h $(INCDIR)/
#	mv $(LIBNAME) ~/lib/
	rm *.o

clean:
	rm *.o
	rm $(LIBNAME)


%.o : %.c
	$(CC) $< $(CFLAGS) -c -o $@

%.o : %.cpp
	$(CXX) $< $(CFLAGS) -c -o $@
