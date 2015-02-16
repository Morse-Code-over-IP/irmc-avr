irmc-avr
========
IRMC stands for Internet Relay Morse Code. 
This is an implementation of a [MOIP](https://github.com/8cH9azbsFifZ/moip) client.

# Build Firmware
Under OSX:
port install avr-binutils avr-libc avr-gcc avrdude
make burn

Testing:
Observe the "DG6FL/AVR" on http://mtc-kob.dyndns.org on channel 33.

# Hardware
I use an [duemilanove](http://arduino.cc/en/pmwiki.php?n=Main/arduinoBoardDuemilanove) with a 
[W5100 Ethernet Shield](http://arduino.cc/en/pmwiki.php?n=Main/ArduinoEthernetShield)

The wiring is as follows: (see [pin mapping](http://arduino.cc/en/Hacking/PinMapping168))

| Pin	| Arduino Name 	| Function 	| 
| :---- | :-----------	| :-------	|
| PNC 	| 		| TXLOOP |
| PD6 	| 		| Speaker |
| PC5 	| analog Input 5| Key (5V!) |
| PD7 	| 		| Connect LED |
| PC1 	| analog Input 1| Disconnect Switch |

# References

# Original [post](http://fernski.blogspot.de/2013/05/sending-morsecode-via-atmega.html) by Fernan Bolando (VE4FEB)
> 1. Only 12 presets are programmed, instead being able to connect to an arbitrary frequency/wire.
> 2. PD6 is a GPIO that generates a ~650Hz square wave tone.
> 3. Some clock jitter results in 4ms variation in timming was observed.
> 4. I have it configured to interface to a separate sidetone generator for keying so the key input is expecting a 5V pulse.
> 5. I did not like the way I implemented the MorseKOB latch function in the original irmc, so I did not add that function into the avr version. (maybe someday) 
> The green LED blinks to indicate that it is connected to the cwcom servers.

Code Quality
============
This is experimental code.


