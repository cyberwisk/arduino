#include <ESP8266WiFi.h>
#include <Espalexa.h>

Espalexa espalexa;

// define the GPIO connected with Relays and switches
#define RelayPin1 0 
#define SwitchPin1 1
#define wifiLed 2

WiFiServer server(80);

int toggleState_1 = 0; //Define integer to remember the toggle state for relay 1

// prototypes
boolean connectWifi();

//callback functions
void firstLightChanged(uint8_t brightness);

// WiFi Credentials
const char* ssid = "Frajola";
const char* password = "dd34e56134";

// device names
String Device_1_Name = "Cristaleira";

boolean wifiConnected = false;

//our callback functions
void firstLightChanged(uint8_t brightness)
{
  //Control the device
  if (brightness == 255)
    {
      digitalWrite(RelayPin1, LOW);
      Serial.println("Device1 ON");
      toggleState_1 = 1;
    }
  else
  {
    digitalWrite(RelayPin1, HIGH);
    Serial.println("Device1 OFF");
    toggleState_1 = 0;
  }
}


void relayOnOff(int relay){
    EspalexaDevice* d = espalexa.getDevice(relay-1); //the index is zero-based
             if(toggleState_1 == 1){
              d->setPercent(100); //set value "brightness" in percent
              digitalWrite(RelayPin1, LOW); // turn on relay 1
              toggleState_1 = 0;
              Serial.println("Device1 ON");
              }
             else{
              d->setPercent(0); //set value "brightness" in percent
              digitalWrite(RelayPin1, HIGH); // turn off relay 1
              toggleState_1 = 1;
              Serial.println("Device1 OFF");
              }
      delay(300);  
}

// connect to wifi – returns true if successful or false if not
boolean connectWifi()
{
  boolean state = true;
  int i = 0;

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Connecting to WiFi");

  // Wait for connection
  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (i > 20) {
      state = false; break;
    }
    i++;
  }
  Serial.println("");
  if (state) {
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("Connection failed.");
  }
  return state;
}

void addDevices(){
  // Define your devices here.
  espalexa.addDevice(Device_1_Name, firstLightChanged); //simplest definition, default state off
  espalexa.begin();
}


void setup()
{
  Serial.begin(115200);
  pinMode(RelayPin1, OUTPUT);
  pinMode(wifiLed, OUTPUT);
  pinMode(SwitchPin1, INPUT_PULLUP);

  //During Starting all Relays should TURN OFF
  digitalWrite(RelayPin1, HIGH);

  // Initialise wifi connection
  wifiConnected = connectWifi();

  if (wifiConnected)
  {
    addDevices();
  }
  else
  {
    Serial.println("Cannot connect to WiFi. So in Manual Mode");
    delay(1000);
  }
  server.begin();
}

void loop()
{
   if (WiFi.status() != WL_CONNECTED)
  {
    WiFiClient client = server.available();
    Serial.print("WiFi Not Connected ");
    digitalWrite(wifiLed, LOW); //Turn off WiFi LED
    //Manual Switch Control
    if (digitalRead(SwitchPin1) == LOW){      
      relayOnOff(1);      
    }
  client.print(F("<br>ssid: "));
  client.println(ssid);
  client.print(F("<br>IP: "));
  client.println(WiFi.localIP());
  }
  else
  {
    Serial.print("WiFi Connected  ");
    digitalWrite(wifiLed, HIGH);
    //Manual Switch Control
    if (digitalRead(SwitchPin1) == LOW){
      relayOnOff(1);      
    }
    //WiFi Control
    if (wifiConnected){
      espalexa.loop();
      delay(1);
    }
    else {
      wifiConnected = connectWifi(); // Initialise wifi connection
      if(wifiConnected){
      addDevices();
      }
    }
  }
}
