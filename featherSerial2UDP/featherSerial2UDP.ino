#include <Wire.h>
#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>
#include "arduino_secrets.h"
int status = WL_IDLE_STATUS;
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                // your network key Index number (needed only for WEP)

WiFiUDP Udp;
const size_t bufferSize = 255;
char buffer[bufferSize];
char IPString[40];
IPAddress remoteIP(10,0,1,2);
unsigned int remotePort = 8888; 
unsigned int localPort = 2390; 
size_t bufferOffset = 0;

// Forward declarations
void processBuffer();
void printWiFiStatus();

// Global variables
int counter = 0;
//unsigned long lastSend = 0;

void setup() {
  WiFi.setPins(8,7,4,2);
  Wire.begin(8);
  Wire.onReceive(receiveEvent);
  Serial.begin(115200);
  Serial1.begin(115200);
    // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to wifi");
  printWiFiStatus();
  Udp.begin(localPort);
}

void loop() {
    while(Serial1.available()) {
    if (bufferOffset < bufferSize) {
      char c = Serial1.read();
      if (c != '\n') {
        // Add character to buffer
        buffer[bufferOffset++] = c;
      }
      else {
        // End of line character found, process line
        buffer[bufferOffset] = 0;
        //int ret = snprintf(buffer, bufferSize, "BPM: ");
        Udp.beginPacket(Udp.remoteIP(), remotePort);
        Udp.write(buffer);
        Udp.endPacket();
        Serial.println(buffer);
        bufferOffset = 0;
      }
    }
    else {
      Serial.println("buffer overflow, emptying buffer");
      bufferOffset = 0;
    }
  }
  delay(50);
}

void receiveEvent(int howMany) {
  while (1 < Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
  }
  int x = Wire.read();    // receive byte as an integer
  Serial.println(x);         // print the integer
}


void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
