//--------------------------------------------------------------------------
// Wifi Modul ESP8266 Parameter als Station setzen
//--------------------------------------------------------------------------
// Basis war das Beispiel von Ray Wang  http://rayshobby.net/?p=9734
//--------------------------------------------------------------------------

#define WIFI_DEBUG

#define WIFI_Serial mySerial
#define Debug_Serial Serial

//#define WIFI_Serial Serial
//#define Debug_Serial mySerial

#ifdef WIFI_DEBUG
#include <SoftwareSerial.h>
SoftwareSerial mySerial(2,3); // RX, TX
#endif

#define SSID  "ok"     // WIFI SSID
#define PASS  "secret"     // WIFI Passwort

#include "/Users/gerolf/nwpass.h"

#define WIFI_ERROR_NONE 0
#define WIFI_ERROR 1

//---------------------------------------------------------------------------
// WIFI
int WIFI_Setup_Error = 0;

//---------------------------------------------------------------------------
// LED Handling
#define LEDMODE_AUS 0
#define LEDMODE_EIN 1
#define LEDMODE_BLINK 2
#define LEDPIN A0
int led_value=LOW;
int led_mode=LEDMODE_AUS;
unsigned long led_nextpuls=0;
int led_pulsdauer = 500;

//-----------------------------------------------------------------------
// Routinen deklarieren

int WIFI_Setup();
void Led_Handling();
void Set_Led(int p);

//--------------------------------------------------------------------------------------
/*
void setup() {
    pinMode(LEDPIN,OUTPUT);
    
    WIFI_Serial.begin(115200);
    WIFI_Serial.setTimeout(5000);
    
#ifdef WIFI_DEBUG
    Debug_Serial.begin(9600);
#endif
    //----------------------------------
    // 3 sec warten
    delay (3000);
    
    Set_Led(HIGH);
#ifdef WIFI_DEBUG
    Debug_Serial.println("Staring");
#endif
    WIFI_Setup_Error = WIFI_Setup();
    
#ifdef WIFI_DEBUG
    if (WIFI_Setup_Error) {
        Debug_Serial.println("Error on setup");
    }
#endif
    
    if (WIFI_Setup_Error) led_mode=LEDMODE_BLINK;
    else Set_Led(LOW);
    
}
 */
//--------------------------------------------------------------------------
/*
void loop() {
    
    Led_Handling();
}*/

//----------------------------------------------------------------------
int WIFI_Setup() {
    String str;
    
    //---------------------------------------------------------------------------
    // WiFi Modus swetzen
    
#ifdef WIFI_DEBUG
    Debug_Serial.println("AT+CWMODE=1");
#endif
    
    WIFI_Serial.println("AT+CWMODE=1");
    delay(1000);
    // Normale Antwort AT+CWMODE=1 0x0D 0x0D 0x0A OK <crlf>
    str = WIFI_Serial.readStringUntil('\n');
    Debug_Serial.println(str);
    
    if(WIFI_Serial.find("Error")){
        return WIFI_ERROR;
    }
    
    
    //-------------------------------------------------------------------
    // Modul resetten
#ifdef WIFI_DEBUG
    Debug_Serial.println("AT+RST");
#endif
    
    WIFI_Serial.println("AT+RST");
    delay(1000);
    
    str = WIFI_Serial.readStringUntil('\n');
    Debug_Serial.println(str);
    
    // "Normale Antwort AT+RST 0xD 0xD 0xA 0xD 0xA OK 0xD 0xA 0xD 0xA ets Jan  ... ready 0xD 0xA
    
    if(!WIFI_Serial.find("ready")) {
        // return WIFI_ERROR;
    }
    
    
    
    // ---------------------------------------------------------------
    // Netzwerk Parameter setzen
    
#ifdef WIFI_DEBUG
    Debug_Serial.println("AT+CWJAP");
#endif
    
    WIFI_Serial.print("AT+CWJAP=\"");
    WIFI_Serial.print(SSID);
    WIFI_Serial.print("\",\"");
    WIFI_Serial.print(PASS);
    WIFI_Serial.println("\"");
    delay(1000);
    
    // Normale Antwort AT+CWJAP="<SSID>”,"<Passwort>" 0x0D 0x0D 0x0A 0x0D 0x0A OK 0x0D 0x0A
    str = WIFI_Serial.readStringUntil('\n');
    Debug_Serial.println(str);
    
    if(!WIFI_Serial.find("OK")){
        // return WIFI_ERROR;
    }
    
#ifdef WIFI_DEBUG
    Debug_Serial.println("Setup okay");
#endif
    return WIFI_ERROR_NONE;
}

//--------------------------------------------------------------------
void Led_Handling(){
    
    if (led_mode==LEDMODE_AUS && led_value != LOW) Set_Led(LOW);
    if (led_mode==LEDMODE_EIN && led_value != HIGH) Set_Led(HIGH);
    if (led_mode==LEDMODE_BLINK && millis() > led_nextpuls){
        led_nextpuls = millis()+(unsigned long)led_pulsdauer;
        if (led_value == LOW) Set_Led(HIGH);
        else Set_Led(LOW);
    }
}

//--------------------------------------------------------------------
void Set_Led(int p){
    led_value = p;
    digitalWrite(LEDPIN,led_value);
}


