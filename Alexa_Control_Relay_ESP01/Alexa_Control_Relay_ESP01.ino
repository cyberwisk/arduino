/**********************************************************************************
 *  TITLE: Alexa control 5 channel Relay Module using NodeMCU or ESP32
 *  Click on the following links to learn more. 
 *  YouTube Video: https://youtu.be/tIFEtHVLexw
 *  Related Blog : https://easyelectronicsproject.com/esp32-projects/
 *  by Subhajit (Tech StudyCell)
 *  
 *  Download the libraries:
 *  https://github.com/Aircoookie/Espalexa
 *  
 *  Preferences--> Aditional boards Manager URLs : 
 *  https://dl.espressif.com/dl/package_esp32_index.json, http://arduino.esp8266.com/stable/package_esp8266com_index.json
 *  
 *  Download Board:
 *  For ESP8266 NodeMCU (2.5.1): https://github.com/esp8266/Arduino
 *  For ESP32                  : https://github.com/espressif/arduino-esp32
 **********************************************************************************/

#include <ESP8266WiFi.h>
#include <Espalexa.h>
// define the GPIO connected with Relays
#define RelayPin1 0  //D1

// prototypes
boolean connectWifi();

//callback functions
void firstLightChanged(uint8_t brightness);

// WiFi Credentials
const char* ssid = "Frajola_2.4G";
const char* password = "dd34e56324";

// device names
String Device_1_Name = "Rele";

boolean wifiConnected = false;

Espalexa espalexa;

void setup()
{
  pinMode(RelayPin1, OUTPUT);

  // Initialise wifi connection
  wifiConnected = connectWifi();

  if (wifiConnected)
  {
    // Define your devices here.
    espalexa.addDevice(Device_1_Name, firstLightChanged); //simplest definition, default state off

    espalexa.begin();
  }
}

void loop()
{
  espalexa.loop();
  delay(1);
}

//our callback functions
void firstLightChanged(uint8_t brightness)
{
  //Control the device
  if (brightness == 255)
    {
      digitalWrite(RelayPin1, HIGH);
    }
  else
  {
    digitalWrite(RelayPin1, LOW);
  }
}

// connect to wifi  returns true if successful or false if not
boolean connectWifi()
{
  boolean state = true;
  int i = 0;

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if (i > 20) {
      state = false; break;
    }
    i++;
  }
  return state;
}
