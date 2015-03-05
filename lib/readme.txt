ESP8266 Module

http://www.amazon.de/ESP8266-Wireless-Transceiver-Arduino-Raspberry/dp/B00P2KDTEC/ref=sr_1_1?ie=UTF8&qid=1424003908&sr=8-1&keywords=arduino+wlan
http://www.amazon.de/Funkmodul-kombiniert-ESP8266EX-Arduino-IDE-Unterstützungö-Grün/dp/B00ONWKA2O/ref=sr_1_5?ie=UTF8&qid=1424003908&sr=8-5&keywords=arduino+wlan

http://zeflo.com/2014/esp8266-weather-display/

http://arduino-hannover.de/2014/12/11/wifi-kochbuch-mit-esp8266/


Radino
http://wiki.in-circuit.de/images/6/69/305000076A_radino_WiFi.pdf

http://wiki.in-circuit.de/index.php5?title=radino_WiFi#Downloads
http://wiki.in-circuit.de/images/7/7d/radino_Pinout_allgemein.jpg



 The SC16IS750 enables the use of baudrates greater than 57600 when
  communicating with ESP8266. (The Arduino runs at 8 Mhz, therfore limited
  to baudrates less than 57600)
  
- radino WiFi setup:
  
  Arduino <---SPI---> SC16IS750 <---UART---> ESP8266
  
- Internal connections between Arduino(ATmega32U4),SC16IS750 and ESP8266:
  
  Arduino          ESP8266        SC16IS750
  
  A4               CH_PD          ---
  D4               RST            ---
  D9               GPIO0          ---
  ---              GPIO2          GPIO0
  
- Pin setup for ESP8266:

  Pin            normal operation        Firmware-Update-Mode
  
  CH_PD          HIGH                    HIGH
  GPIO0          HIGH                    LOW
  GPIO2          HIGH                    HIGH
  RST            HIGH                    HIGH
  
 */



// Use SPI-library for communication with SC16IS750
#include <SPI.h>
#include "SC16IS750.h"






// Pin definitions of ESP8266
//       Pin-Name   Arduino-Pin-Number
#define  CH_PD      A4
#define  CH_RST     4
#define  GPIO0      9
//       Pin-Name   SC16IS750-GPIO-Number
#define  GPIO2      0






/*
 * Setup connections for normal operation of ESP8266 WiFi-Chip
*/
void ESP8266_setNormalOperationMode()
{
  pinMode(CH_PD, OUTPUT);  // Chip-Enable of ESP8266
  digitalWrite(CH_PD, HIGH);  // HIGH
  
  pinMode(GPIO0, OUTPUT);  // GPIO0 of ESP8266
  digitalWrite(GPIO0, HIGH);  // HIGH
  
  // GPIO of SC16 is connected to GPIO2 of ESP8266
  SC16_GPIO_pinMode(GPIO2, SC16_GPIO_OUTPUT);  // GPIO2 of ESP8266
  SC16_GPIO_digitalWrite(GPIO2, 1);  // HIGH
  
  // Perform RESET of ESP8266
  pinMode(CH_RST,OUTPUT);
  digitalWrite(CH_RST, LOW);  // Pull RESET of WIFI-Chip
  delay(200);
  digitalWrite(CH_RST, HIGH);  // Disable RESET of WIFI-Chip
}








Init:
  SC16_init();  // Init SC16IS750 SPI-UART-Module
  SC16_setbaudRate(ESP8266_BAUDRATE);  // Set Baudrate of SPI-UART-Module
  ESP8266_setNormalOperationMode();  // Put ESP8266 into normal operation mode
  
  delay(250);
  SC16_flush();  // Clear UART-buffers
  
  wdt_enable(WDTO_8S);  // Enable Watchdog-Timer, Watchdog reset after 8 seconds
  
  // Test if ESP8266-Wifi-Module is ready
  // Send command "AT" and wair for response "OK"
  while(!sendAndWait( "AT","OK","OK", 100, 100, 0));
  wdt_reset();  // Reset Watchdog Timer
  
  // Set ESP8266-Wifi-Module to Mode 1
  // --> 
  while(!sendAndWait("AT+CWMODE=1","no change","OK", 500, 100, 0));
  wdt_disable();  // Disable Watchdog Timer
 
  // Try to connect to specified WiFi-Network
  // This function uses Watchdog-Timer, if connection does not
  // occur within 8 seconds, radino WiFi will reset
  reconnect(); 
  
  delay(100);
  SC16_flush();  // Clear UART-buffers






