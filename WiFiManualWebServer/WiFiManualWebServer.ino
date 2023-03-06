/*
    This sketch demonstrates how to set up a simple HTTP-like server.
    The server will set a GPIO pin depending on the request
      http://server_ip/gpio/0 will set the GPIO2 low,
      http://server_ip/gpio/1 will set the GPIO2 high
    server_ip is the IP address of the ESP8266 module, will be
*/

#include <ESP8266WiFi.h>

#ifndef STASSID
#define STASSID "Frajola"
#define STAPSK  "dd34e56134"
#define RelayPin1 0 
#define SwitchPin1 1
#define wifiLed 2
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

void setup() {

  // prepare RelayPin1
  pinMode(RelayPin1, OUTPUT);
  digitalWrite(RelayPin1, 0);

  // Connect to WiFi network
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  // Start the server
  server.begin();
  //Serial.println(F("Server started"));
  //Serial.println(WiFi.localIP());
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  client.setTimeout(5000); // default is 1000

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  
  // Match the request
  int val;
  if (req.indexOf(F("/gpio/0")) != -1) {
    val = 0;
  } else if (req.indexOf(F("/gpio/1")) != -1) {
    val = 1;
  } else {
    val = digitalRead(RelayPin1);
  }

  // Set LED according to the request
  digitalWrite(RelayPin1, val);

  // read/ignore the rest of the request
  // do not client.flush(): it is for output only, see below
  while (client.available()) {
    // byte by byte is not very efficient
    client.read();
  }

  // Send the response to the client
  // it is OK for multiple small client.print/write,
  // because nagle algorithm will group them into one single packet
  client.print(F("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nGPIO is now "));
  client.print((val) ? F("high") : F("low"));
  client.print(F("<br>ssid: "));
  client.println(ssid);
  client.print(F("<br>IP: "));
  client.println(WiFi.localIP());
  client.print(F("<br>Click <a href='http://"));
  client.print(WiFi.localIP());
  client.print(F("/gpio/1'>here</a> to switch RelayPin GPIO on, or <a href='http://"));
  client.print(WiFi.localIP());
  client.print(F("/gpio/0'>here</a> to switch RelayPin GPIO off.</html>"));

  // The client will actually be *flushed* then disconnected
  // when the function returns and 'client' object is destroyed (out-of-scope)
  // flush = ensure written data are received by the other side
}
