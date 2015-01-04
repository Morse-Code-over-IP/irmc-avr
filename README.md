irmc-avr
========

# Build Firmware
Under OSX:
port install avr-binutils avr-libc avr-gcc avrdude
make burn


# Hardware
I use an [duemilanove](http://arduino.cc/en/pmwiki.php?n=Main/arduinoBoardDuemilanove) with a 
[W5100 Ethernet Shield](http://arduino.cc/en/pmwiki.php?n=Main/ArduinoEthernetShield)

The wiring is as follows:
* Sounder: PORTD
* (TXLOOP PINC)
* Use PD6 as speaker output
* key is connected at PC5(ADC7)  (--> 5V!)

=== Original Post ===
Copied on 20150102 from http://fernski.blogspot.de/2013/05/sending-morsecode-via-atmega.html
> A few weeks ago I wrote an openBSD client to connect to CWCOM servers and called it irmc.  
> 
> A new version of the client was implemented using an Atmel microcontroller. I decided to use an arduino board that was gathering dust on my work bench. I was initially planning to use the arduino as a testbed only and build an actual board later, But I usually only have time to do weekend projects and the coding already took me the entire weekend. So I decided to put building a standalone hardware for later or maybe never.
>
> The source is available here http://fernan.bitbucket.org/irmc-avr.tgz. The code is written in C compiled using the avr-gcc tools. Orginally it was a port of the original irmc, but the limited memory of the atmega required a considerable rewrite. In addition to the limitation of the original irmc, here are few things to note.
> 1. Only 12 presets are programmed, instead being able to connect to an arbitrary frequency/wire.
> 2. PD6 is a GPIO that generates a ~650Hz square wave tone.
> 3. Some clock jitter results in 4ms variation in timming was observed.
> 4. I have it configured to interface to a separate sidetone generator for keying so the key input is expecting a 5V pulse.
> 5. I did not like the way I implemented the MorseKOB latch function in the original irmc, so I did not add that function into the avr version. (maybe someday) 
> 
> BUGS?: Probably a few. 
> 
> The final version was placed into an enclosure. The green LED blinks to indicate that it is connected to the cwcom servers.
> 
