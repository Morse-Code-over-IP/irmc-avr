irmc-avr
========
# What is IRMC?
IRMC stands for Internet Relay Morse Code. It implements the [CWCom protocol](http://kob.sdf.org/morsekob/docs/cwcom.pdf) as adopted by [MorseKOB](http://kob.sdf.org/morsekob/docs/history.pdf). You can try out the software in a [browser](http://kob.sdf.org/morsekob/morsekob30/index.htm) using Java.

## Why morse code over IP?
Why not? A number of different approaches for CW over IP exist. An early implementation has been the CWCom protocol, 
which has been the basis for Morse KOB, a ready-to-use software. Both chat programs have been written for CW exclusively and are compatible. An alternative implementation in C exists as well as a port to the ATMEL AVR Microcontroller. 

Besides a variety of other approaches exist, for example an IRC plugin ([CWirc](http://myspace.voo.be/pcoupard/cwirc/)), a [JavaScript implementation](http://morsecode.me), a [bandwidth-eating UDP implementation](http://hans.liss.pp.se/node/343), a [chat for iOS](http://pignology.net/cwwithme.html) or a modulated CW over IP using Mumble ([iCW](https://sites.google.com/site/icwoip/)).


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

# References

## Original post by Fernan Bolando (VE4FEB)
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

## Resources
* CWCom, John Samin (VK1EME): http://www.mrx.com.au/d_cwcom.htm
* Morse KOB, Les Kerr: https://sites.google.com/site/morsekob/ and http://kob.sdf.org/morsekob/
* Relay server for CW communicator and morse KOB: http://morsecode.dc3.com:7890 
* Sources for the Relay Server: http://sourceforge.net/projects/morse-rss-news/
* MorseKOB for AT Mega, Fernan Bolando (VE4FEB): http://fernski.blogspot.de/2013/05/sending-morsecode-via-atmega.html
