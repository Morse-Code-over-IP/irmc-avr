/*
  ReverseBeaconNetwork client

  Written by Hajo Dezelski DL1SDZ
  Email: d...@gmail.com

   This code is so trivial that I'm releasing it completely without
   restrictions.  If you find it useful, it would be nice if you dropped
   me an email, maybe plugged my blog @http://hajos-kontrapunkte.blogspot.de/ or included
   a brief acknowledgement in whatever derivative you create, but that's
   just a courtesy.  Feel free to do whatever.

 */

#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>

// The shield uses the I2C SCL and SDA pins. On classic Arduinos
// this is Analog 4 and 5 so you can't use those for analogRead() anymore
// However, you can connect other I2C sensors to the I2C bus and share
// the I2C bus.

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

// Definitions

// These #defines make it easy to set the backlight color
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7

#define CALL_LEN 7
#define NUM_LEN 2
#define TIME_LEN 4
#define DATA_LEN 80


const int NROWS = 2;                                     // LCD
const int NCOLS = 16;                                    // LCD

int col = 0;
int row = 0;
int i = 0;

char line2[NCOLS] = " ";
char call_sk[CALL_LEN] = "";
char call_dx[CALL_LEN] = "";
char dbm[NUM_LEN] = "";
char wpm[NUM_LEN] = "";
char a_time[TIME_LEN] = "";

boolean blank = false;


// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {    0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = {  192, 168, 2, 177 };        //The IP address the WebServer will run on
byte gateway[] = {  192, 168, 2, 1 };   //If you have a router on your network
byte subnet[] = {  255, 255, 255, 0 };

IPAddress server(69,36,242,140); // telnet.reversebeacon.net 

// Initialize the Ethernet client library
// with the IP address and port of the server 
EthernetClient client;

void setup() {
  
    // set up the LCD's number of rows and columns: 
  lcd.begin(16, 2);

  
  // start the Ethernet connection:
  Ethernet.begin(mac, ip);
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("connecting...");
  lcd.println("connecting...");
  // if you get a connection, report back via serial:
  if (client.connect(server, 7000)) {
    Serial.println("connected");
	lcd.setCursor(0,0);
    lcd.println("connected");
  } 
  else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
	lcd.setCursor(0,1);
    lcd.println("connection failed");
  }
}


void checkChar(char ch) {
  if(isAlphaNumeric(ch)) {

   lcd.print(ch);
   blank = false;
   }
   
    if(isWhitespace(ch)) {
	 if (blank == false) {
      lcd.print(ch);
	  blank = true;
    }
	}
	
	if((ch == '.') ) {
      lcd.print(ch);
	   blank = false;
    }
	
	if((ch == '/') ) {
      lcd.print(ch);
	   blank = false;
    }	
		if((ch == '-') ) {
      lcd.print(' ');
	   blank = true;
    }	
	}
	
void analyze( char ch) {

int p = 0;

    if ( ch != '\n') {
	
		if (i == 6) { 
		lcd.clear();
		lcd.setCursor(0,1);
		}
		
	    if ((i >= 7) && (i <= 9)) { //call_sk
		checkChar(ch);
		}
		if (i == 19) { 
		lcd.setCursor(9,0);
		}
	    if ((i >= 20) && (i <= 26)) { //Frequency
		checkChar(ch);
		}
		if (i == 28) { 
		lcd.setCursor(0,0);
		}
		
	    if ((i >= 29) && (i <= 35)) { //call_dx
		if (i == 29) {
		   if ((ch == 'A') || (ch == 'K') || (ch == 'N') || (ch == 'W')) {
		      lcd.setBacklight(RED); }
		   if ((ch == '3') || (ch == '4') || (ch == '7') || (ch == '9')) {
		      lcd.setBacklight(BLUE); }
           if ((ch == 'D') || (ch == 'G') || (ch == 'M') || (ch == 'V')) {
		      lcd.setBacklight(GREEN); }			  
		   if ((ch == 'O') || (ch == 'P') || (ch == 'J') || (ch == 'S')) {
		      lcd.setBacklight(TEAL); }
           if ((ch == 'U') || (ch == 'R') || (ch == 'L') || (ch == 'H')) {
		      lcd.setBacklight(YELLOW); }
           if ((ch == 'E') || (ch == 'I') || (ch == 'F') || (ch == 'C')) {
		      lcd.setBacklight(VIOLET); }			  
		   if ((ch == 'B') || (ch == 'X') || (ch == 'Y') || (ch == 'Z')) {
		      lcd.setBacklight(WHITE); }
   
			}
		checkChar(ch);
		}
		
		if (i == 47) { 
		lcd.setCursor(4,1);
		}
        if ((i >= 48) && (i <= 49)) { //db
		checkChar(ch);
		}
		if (i == 50) { 
		lcd.print('d');
		}
		
	    if ((i >= 55) && (i <= 56)) { //bpm
		checkChar(ch);
		}
		if (i == 57) { 
		lcd.print('w');
		lcd.print(' ');
		}
	    if ((i >= 72) && (i <= 75)) { //time
		checkChar(ch);
		}
		if (i == 76) { 
		lcd.print('z');
		}
		i++; 
		
	} else {
	  // Serial.print(i);
	 for (p = 0;  p < 100; p++) {};
	 i=1;
	  }
}

void loop()
{
  // if there are incoming bytes available 
  // from the server, read them and print them:
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
    analyze(c);
  }

  // as long as there are bytes in the serial queue,
  // read them and send them out the socket if it's open:
  while (Serial.available() > 0) {
    char inChar = Serial.read();
    if (client.connected()) {
      client.print(inChar); 	  
  }
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
	lcd.setCursor(0,1);
    lcd.println("disconnecting.");
    client.stop();
    // do nothing:
    while(true);
  }
}




