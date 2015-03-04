//--------------------------------------------------------------------------
// Wifi Modul ESP8266 Parameter als Accrsspoint setzen
//--------------------------------------------------------------------------
// Basis war das Beispiel von Ray Wang  http://rayshobby.net/?p=9734
//--------------------------------------------------------------------------

#define WIFI_DEBUG

#define WIFI_Serial Serial
#define Debug_Serial mySerial


#ifdef WIFI_DEBUG
#include <SoftwareSerial.h>
SoftwareSerial mySerial(10,11); // RX, TX
#endif

#define SSID  "myESP01"     // WiFi SSID
#define PASS  "12345678"  // WiFi Passwort
#define WIFI_CANNEL 8
#define WIFI_SECURE 3
// 0 = offen ohne Passwort
// 1 = WEB
// 2 = WPA_PSK
// 3 = WPA2_PSK
// 4 = WPA_WPA2_PSK



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
#define LEDPIN 13
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

  WIFI_Setup_Error = WIFI_Setup();

#ifdef WIFI_DEBUG
  if (WIFI_Setup_Error) {
    Debug_Serial.println("Error");
  }
#endif

  if (WIFI_Setup_Error) led_mode=LEDMODE_BLINK;
  else Set_Led(LOW);

}
//--------------------------------------------------------------------------
void loop() {

  Led_Handling();
}

//----------------------------------------------------------------------
int WIFI_Setup() {

  //---------------------------------------------------------------------------
  // WiFi Modus swetzen

#ifdef WIFI_DEBUG
  Debug_Serial.println("AT+CWMODE=2");
#endif

  WIFI_Serial.println("AT+CWMODE=2");
  delay(1000);
  // Normale Antwort AT+CWMODE=1 0x0D 0x0D 0x0A OK <crlf> 

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

  // "Normale Antwort AT+RST 0xD 0xD 0xA 0xD 0xA OK 0xD 0xA 0xD 0xA ets Jan  ... ready 0xD 0xA

  if(!WIFI_Serial.find("ready")) {
    return WIFI_ERROR;
  }

  // ---------------------------------------------------------------
  // Netzwerk Parameter setzen

#ifdef WIFI_DEBUG
  Debug_Serial.println("AT+CWSAP");
#endif

  WIFI_Serial.print("AT+CWSAP=\"");
  WIFI_Serial.print(SSID);
  WIFI_Serial.print("\",\"");
  WIFI_Serial.print(PASS);
  WIFI_Serial.print("\",");
  WIFI_Serial.print(WIFI_CANNEL);
  WIFI_Serial.print(",");
  WIFI_Serial.println(WIFI_SECURE);
  delay(1000);

  // Normale Antwort AT+CWJAP="<SSID>”,"<Passwort>" 0x0D 0x0D 0x0A 0x0D 0x0A OK 0x0D 0x0A 

  if(!WIFI_Serial.find("OK")){
    return WIFI_ERROR;
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



