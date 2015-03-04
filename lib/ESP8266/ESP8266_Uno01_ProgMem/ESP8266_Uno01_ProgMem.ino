//--------------------------------------------------------------------------
// Wifi Modul ESP8266 Beispiel Programm
//--------------------------------------------------------------------------
// Basis war das Beispiel von Ray Wang  http://rayshobby.net/?p=9734
//--------------------------------------------------------------------------

#include <avr/pgmspace.h>

#define WIFI_DEBUG

#define WIFI_Serial Serial
#define Debug_Serial mySerial

#ifdef WIFI_DEBUG
#include <SoftwareSerial.h>
SoftwareSerial mySerial(10,11); // RX, TX
#endif

#define WIFI_ERROR_NONE 0
#define WIFI_ERROR 1

//---------------------------------------------------------------------------
// HTML
int HTML_Content_Length;
int HTML_Header_Length;
int HTML_Temp_Length;
byte HTML_Sende_Mode;
int HTML_Counter = 0;

#define HTML_SENDE_MODE_PREPARE 0
#define HTML_SENDE_MODE_SEND 1

//---------------------------------------------------------------------------
// WIFI
char WIFI_Net[30];
char WIFI_IP_Adress[20];
#define PORT 80
char WIFI_Host[24];
int WIFI_CWMODE;
#define CWMODE_STATION 1
#define CWMODE_ACCESSPOINT 2
int WIFI_Setup_Error = 0;


//---------------------------------------------------------------------------
// Receive-Buffer
#define RECV_BUFFER_SIZE 100
char RECV_Buffer[RECV_BUFFER_SIZE];

//---------------------------------------------------------------------------
// LED Handling
#define LEDMODE_AUS 0
#define LEDMODE_EIN 1
#define LEDMODE_BLINK 2
#define LEDPIN 13
int led_value = LOW;
int led_mode = LEDMODE_AUS;
unsigned long led_nextpuls = 0;
int led_pulsdauer = 500;

//-----------------------------------------------------------------------
// Routinen deklarieren
int WIFI_Setup();
void RECV_Buffer_Clear();
void Led_Handling();
void Set_Led(int p);
void HTML_Make_Header();
void HTML_Make_Content();
void HTML_Send(char * c);
void HTML_Send_Int(int z);

void HTML_Send_PROGMEM(const __FlashStringHelper* tx);

//--------------------------------------------------------------------------------------
void setup() {
  pinMode(LEDPIN, OUTPUT);

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
    Debug_Serial.println(F("Error"));
  }
#endif

  if (WIFI_Setup_Error) led_mode = LEDMODE_BLINK;
  else Set_Led(LOW);

  WIFI_Serial.setTimeout(20);
  WIFI_Host[0] = 0;
  RECV_Buffer_Clear();
}
//--------------------------------------------------------------------------
void loop() {

  int WIFI_Cannel, WIFI_Packet_Length;
  int i;
  char *buffer_pointer;
  byte len;

  // request: +IPD,ch,len:GET /?LED=xxx&PULS=nnnn ... Host: nnn.nnn.nnn.nnn:nnnn 0x0D ...

  if (WIFI_Serial.findUntil("+IPD,", "\r")) {

    WIFI_Cannel = WIFI_Serial.parseInt();
    WIFI_Serial.findUntil(",", "\r");
    WIFI_Packet_Length = WIFI_Serial.parseInt();

    if (WIFI_Serial.findUntil("GET /", "\r")) {

      WIFI_Serial.readBytesUntil(13, RECV_Buffer, RECV_BUFFER_SIZE);

      if (WIFI_Packet_Length > 0) {
        buffer_pointer = RECV_Buffer;
        if (strncmp(buffer_pointer, "?LED=", 5) == 0) {
          buffer_pointer += 5;
          if (strncmp(buffer_pointer, "Ein", 3) == 0) {
            led_mode = LEDMODE_EIN;
          }
          if (strncmp(buffer_pointer, "Aus", 3) == 0) {
            led_mode = LEDMODE_AUS;
          }
          if (strncmp(buffer_pointer, "Blk", 3) == 0) {
            led_mode = LEDMODE_BLINK;
          }
          buffer_pointer += 3;
          if (strncmp(buffer_pointer, "&PULS=", 6) == 0) {
            buffer_pointer += 6;
            for (i = 0; i < 5; i++) {
              if (buffer_pointer[i] < '0' || buffer_pointer[i] > '9') buffer_pointer[i] = 0;
            }
            led_pulsdauer = atoi(buffer_pointer);
          }
        }
        WIFI_Host[0] = 0;
        if (WIFI_Serial.find("Host: ")) {
          len = WIFI_Serial.readBytesUntil(13, WIFI_Host, 23);
          WIFI_Host[len] = 0;
        }
        HTML_Page(WIFI_Cannel);
      }
    }
    RECV_Buffer_Clear();
  }
  Led_Handling();
}



//----------------------------------------------------------------------
int WIFI_Setup() {
  byte len;

  RECV_Buffer_Clear();

  //-----------------------------------------------------------
  // start server

#ifdef WIFI_DEBUG
  Debug_Serial .println(F("AT+CIPMUX"));
#endif

  WIFI_Serial .println(F("AT+CIPMUX=1"));
  delay(10);
  // normale Antwort :
  //   "AT+CIPMUX=1" 0xD 0xD 0xA 0xD 0xA "OK" 0xD 0xA

  if (!WIFI_Serial.find("OK")) {
    return WIFI_ERROR;
  }
  RECV_Buffer_Clear();

  //-----------------------------------------------------------
  // TCP service aktivieren

#ifdef WIFI_DEBUG
  Debug_Serial .println(F("AT+CIPSERVER"));
#endif
  WIFI_Serial .print(F("AT+CIPSERVER=1,"));
  WIFI_Serial.println(PORT);
  delay(10);

  // normale Antwort :
  //   "AT+CIPSERVER=1,8080" 0xD 0xD 0xA 0xD 0xA "OK" 0xD 0xA

  if (!WIFI_Serial.find("OK")) {
    return WIFI_ERROR;
  }

  RECV_Buffer_Clear();

  //-------------------------------------------------------------------------
  // Timeout für automatisches Trennen der Verbindung setzen

#ifdef WIFI_DEBUG
  Debug_Serial .println(F("AT+CIPSTO"));
#endif

  WIFI_Serial .println(F("AT+CIPSTO=0"));
  delay(10);
  if (!WIFI_Serial.find("OK")) {
    return WIFI_ERROR;
  }

  //---------------------------------------------------------------------------
  // WiFi Modus ermitteln

  RECV_Buffer_Clear();
  WIFI_Net[0] = 0;

#ifdef WIFI_DEBUG
  Debug_Serial .println(F("AT+CWMODE?"));
#endif

  WIFI_Serial .println(F("AT+CWMODE?"));
  delay(10);
  // Normale Antwort AT+CWMODE? 0x0D 0x0D 0x0A+CWMODE:1 0x0D 0x0A 0x0D 0x0A OK 0x0D 0x0A

  WIFI_CWMODE = 0;
  if (WIFI_Serial.find("AT+CWMODE?\r\r\n+CWMODE:")) {
    WIFI_CWMODE = WIFI_Serial.parseInt();
  }

#ifdef WIFI_DEBUG
  Debug_Serial .print(F("CWMode:"));
  Debug_Serial.println(WIFI_CWMODE);
#endif

  if (WIFI_CWMODE == 0) {
    return WIFI_ERROR;
  }

  //---------------------------------------------------
  // Ich bin Station. In welchem Netz bin ich ?

  if (WIFI_CWMODE == CWMODE_STATION) {

#ifdef WIFI_DEBUG
    Debug_Serial .println(F("AT+CWJAP?"));
#endif

    WIFI_Serial .println(F("AT+CWJAP?"));
    delay(10);
    // Normale Antwort AT+CWJAP? 0x0D 0x0D 0x0A +CWJAP:"<SSID>" 0x0D 0x0A 0x0D 0x0A OK 0x0D 0x0A

    len = 0;
    WIFI_Net[0] = 0;
    if (WIFI_Serial.find("AT+CWJAP?\r\r\n+CWJAP:\"")) {
      len = WIFI_Serial.readBytesUntil('\"', WIFI_Net, 20);
      WIFI_Net[len] = 0;
    }

    if (len > 0) {
#ifdef WIFI_DEBUG
      Debug_Serial .println(F("Netzwerk:"));
      Debug_Serial.println(WIFI_Net);
#endif
    }
    else {
      return WIFI_ERROR;
    }
  }
  //------------------------------------------
  // Ich bin Accesspoint, wie heisse ich ?
  if (WIFI_CWMODE == CWMODE_ACCESSPOINT) {

#ifdef WIFI_DEBUG
    Debug_Serial .println(F("AT+CWSAP?"));
#endif

    WIFI_Serial .println(F("AT+CWSAP?"));
    delay(10);
    // Normale Antwort AT+CWSAP? 0x0D 0x0D 0x0A +CWSAP:"MyESP",
    //                 "12345678",8,3 0x0D 0x0A 0x0D 0x0A OK 0x0D 0x0A
    len = 0;
    WIFI_Net[0] = 0;
    if (WIFI_Serial.find("AT+CWSAP?\r\r\n+CWSAP:\"")) {
      len = WIFI_Serial.readBytesUntil('\"', WIFI_Net, 20);
      WIFI_Net[len] = 0;
    }

    if (len > 0) {
#ifdef WIFI_DEBUG
      Debug_Serial .println(F("Netzwerk:"));
      Debug_Serial.println(WIFI_Net);
#endif
    }
    else {
      return WIFI_ERROR;
    }
  }

  //-----------------------------------------------------------
  // Get IP Adress

#ifdef WIFI_DEBUG
  Debug_Serial .println(F("AT+CIFSR"));
#endif

  WIFI_Serial .println(F("AT+CIFSR"));
  delay(1000);

  // Normale Antwort z.B.
  // AT+CIFSR 0xD 0xD 0xA 192.168.178.26 0xD 0xA

  len = 0;
  WIFI_IP_Adress[0] = 0;
  if (WIFI_Serial.find("AT+CIFSR\r\r\n")) {
    len = WIFI_Serial.readBytesUntil('\r', WIFI_IP_Adress, 20);
    WIFI_IP_Adress[len] = 0;
  }
#ifdef WIFI_DEBUG
  if (len > 0) {
    Debug_Serial .println(F("IP"));
    Debug_Serial.println(WIFI_IP_Adress);
  }
#endif
  if (len == 0) {
    return WIFI_ERROR;
  }

#ifdef WIFI_DEBUG
  Debug_Serial .println(F("Setup okay"));
#endif

  return WIFI_ERROR_NONE;
}

//----------------------------------------------------------------------

void HTML_Page(int WIFI_Cannel) {

  HTML_Counter++;

#ifdef WIFI_DEBUG
  Debug_Serial.print(F("Zaehler: "));
  Debug_Serial.println(HTML_Counter);
#endif

  HTML_Sende_Mode = HTML_SENDE_MODE_PREPARE;

  HTML_Temp_Length = 0;
  HTML_Make_Content();
  HTML_Content_Length = HTML_Temp_Length;

  HTML_Temp_Length = 0;
  HTML_Make_Header();
  HTML_Header_Length = HTML_Temp_Length;

  WIFI_Serial.print(F("AT+CIPSEND="));
  WIFI_Serial.print(WIFI_Cannel);
  WIFI_Serial.print(F(","));
  WIFI_Serial.println(HTML_Header_Length + HTML_Content_Length);

  delay(20);

  RECV_Buffer_Clear();

  HTML_Sende_Mode = HTML_SENDE_MODE_SEND;
  HTML_Make_Header();
  HTML_Make_Content();

  delay(10);
}

//---------------------------------------------------------------------------
void HTML_Make_Header() {

  HTML_Send_PROGMEM(F("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n"));
  HTML_Send_PROGMEM(F("Content-Length:"));
  HTML_Send_Int(HTML_Content_Length);
  HTML_Send_PROGMEM(F("\r\n\r\n"));
}

//---------------------------------------------------------------------------
void HTML_Make_Content() {

  HTML_Send_PROGMEM(F("<HTML><HEAD> <title>Arduino Steuerung</title></HEAD>\n"));

  HTML_Send_PROGMEM(F("<BODY bgcolor=\"#ADCEDE\" text=\"#000000\">"));

  HTML_Send_PROGMEM(F("<FONT size=\"6\" FACE=\"Verdana\">Arduino Steuerung<BR/></FONT>"));

  HTML_Send_PROGMEM(F("<FONT size=\"3\" FACE=\"Verdana\">\n<BR/>Hubbie's Arduino Uno<BR/>"));

  HTML_Send_PROGMEM(F("Led : "));
  switch (led_mode)  {
    case LEDMODE_AUS:
      HTML_Send_PROGMEM(F("aus"));
      break;
    case LEDMODE_EIN:
      HTML_Send_PROGMEM(F("ein"));
      break;
    case LEDMODE_BLINK:
      HTML_Send_PROGMEM(F("blinkt"));
      break;
  }
  HTML_Send_PROGMEM(F("<BR/>\n"));

  //--------------------------------------
  // Aufrufzaehler
  HTML_Send_PROGMEM(F("Aufrufz&auml;hler = "));
  HTML_Send_Int(HTML_Counter);

  HTML_Send_PROGMEM(F("<BR/></font>"));

  HTML_Send_PROGMEM(F("<FORM ACTION=\"http://"));
  HTML_Send(WIFI_Host);
  HTML_Send_PROGMEM(F("\">"));

  HTML_Send_PROGMEM(F("<P><FONT size=\"3\" FACE=\"Verdana\">Led schalten :<BR/>"));

  if (led_mode == LEDMODE_EIN)  {
    HTML_Send_PROGMEM(F("<INPUT TYPE=\"RADIO\" NAME=\"LED\" VALUE=\"Ein\"> Einschalten<BR/>"));
    HTML_Send_PROGMEM(F("<INPUT TYPE=\"RADIO\" NAME=\"LED\" VALUE=\"Aus\" CHECKED> Ausschalten<BR/>"));
  }
  else
  {
    HTML_Send_PROGMEM(F("<INPUT TYPE=\"RADIO\" NAME=\"LED\" VALUE=\"Ein\" CHECKED> Einschalten<BR/>"));
    HTML_Send_PROGMEM(F("<INPUT TYPE=\"RADIO\" NAME=\"LED\" VALUE=\"Aus\"> Ausschalten<BR/>"));
  }

  HTML_Send_PROGMEM(F("<INPUT TYPE=\"RADIO\" NAME=\"LED\" VALUE=\"Blk\"> Blinken"));

  HTML_Send_PROGMEM(F("&emsp;&emsp;&emsp;&emsp;&emsp;Plusdauer:<INPUT NAME=\"PULS\" TYPE=\"TEXT\" size=\"4\" MAXLENGTH=\"4\" VALUE=\""));
  HTML_Send_Int(led_pulsdauer);
  HTML_Send_PROGMEM(F("\"> mSec<BR/>"));
  HTML_Send_PROGMEM(F("<BR/>"));

  //--------------------------------------
  // Absende-Knopf

  HTML_Send_PROGMEM(F("<INPUT TYPE=\"SUBMIT\" VALUE=\" Absenden \">"));
  HTML_Send_PROGMEM(F("</FONT></P>"));
  HTML_Send_PROGMEM(F("</BODY></HTML>"));
}

//---------------------------------------------------------------------------
void HTML_Send_Int(int p_int) {
  char tmp_text[8];
  itoa(p_int, tmp_text, 10);
  HTML_Send(tmp_text);
}

//---------------------------------------------------------------------------
void HTML_Send(char * p_text) {
  HTML_Temp_Length += strlen(p_text);
  if (HTML_Sende_Mode == HTML_SENDE_MODE_SEND) {
    WIFI_Serial.print(p_text);
  }
}

//---------------------------------------------------------------------------
void HTML_Send_PROGMEM(const __FlashStringHelper* p_text) {

  HTML_Temp_Length += strlen_P((const char*)p_text);
  if (HTML_Sende_Mode == HTML_SENDE_MODE_SEND) {
    WIFI_Serial.print(p_text);
  }
}


//------------------------------------------------------------
void RECV_Buffer_Clear() {
  for (int i = 0; i < RECV_BUFFER_SIZE; i++) RECV_Buffer[i] = 0;
  while (WIFI_Serial.available())WIFI_Serial.read();
}

//--------------------------------------------------------------------
void Led_Handling() {

  if (led_mode == LEDMODE_AUS && led_value != LOW) Set_Led(LOW);
  if (led_mode == LEDMODE_EIN && led_value != HIGH) Set_Led(HIGH);
  if (led_mode == LEDMODE_BLINK && millis() > led_nextpuls) {
    led_nextpuls = millis() + (unsigned long)led_pulsdauer;
    if (led_value == LOW) Set_Led(HIGH);
    else Set_Led(LOW);
  }
}

//--------------------------------------------------------------------
void Set_Led(int p) {
  led_value = p;
  digitalWrite(LEDPIN, led_value);
}








