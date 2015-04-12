irmc-avr
========
IRMC stands for Internet Relay Morse Code. 
This is an implementation of a [MOIP](https://github.com/8cH9azbsFifZ/moip) client.

![Prototype](/img/prototype.png?raw=true "Prototype")

#Building
Build the firmware under OSX:
port install avr-binutils avr-libc avr-gcc avrdude
make burn

#Testing
Observe the "DG6FL/AVR" on http://mtc-kob.dyndns.org on channel 33.

## Testing with MorseKOB 3.0 (Java)
This [software](http://kob.sdf.org/morsekob/morsekob30/MorseKOB.jar) will run on
Linux, Windows and OSX. With the [RXTX software](http://morsekob.org/morsekob30/help.htm)
for Java (i.e. [librxtxSerial.jnilib](http://blog.brianhemeryck.me/installing-rxtx-on-mac-os-mountain-lion/) 
on OSX) it is even possible to connect to external hardware. 
NB: before you transmit make sure you uncheck the "circuit closer".

# Hardware
I use an [duemilanove](http://arduino.cc/en/pmwiki.php?n=Main/arduinoBoardDuemilanove) with a 
[W5100 Ethernet Shield](http://arduino.cc/en/pmwiki.php?n=Main/ArduinoEthernetShield)

The wiring is as follows: (see [pin mapping](http://arduino.cc/en/Hacking/PinMapping168))

| Pin	| Arduino Name 	| Function 	| 
| :---- | :-----------	| :-------	|
| PD6 	|               | Speaker |
| PC5 	| analog Input 5| Key (5V!) |
| PD7 	|               | Connect LED |
| PD4   |               | Data LED |

![Prototype](/doc/prototype_wiring.png?raw=true "Prototype")

# References
* Original [post](http://fernski.blogspot.de/2013/05/sending-morsecode-via-atmega.html) by Fernan Bolando (VE4FEB)

Code Quality
============
This is experimental code. The latest version working without too much tweaking is [v0.2](https://github.com/8cH9azbsFifZ/irmc-avr/archive/v0.2.zip)


