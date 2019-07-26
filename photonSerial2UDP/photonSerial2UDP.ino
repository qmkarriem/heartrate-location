#include "Particle.h"

//#define SERIAL_DEBUG;

// Constants
//const unsigned long SEND_INTERVAL_MS = 2000;
const size_t bufferSize = 1024;
char IPString[40];
IPAddress remoteIP(172,20,10,3);
int remotePort = 8888;
UDP udp;

// Forward declarations
void processBuffer();

// Global variables
int counter = 0;
//unsigned long lastSend = 0;

char buffer[bufferSize];
size_t bufferOffset = 0;

/* // receive a new port from Particle cloud Terminal
void updateRemotePort(const char *event, const char *data) {
  remotePort = atoi(data);
  Particle.publish("remotePortCallback", remotePort);
} */

/*//receive a new IP address from Particle cloud Terminal
void updateRemoteIP(const char *event, const char *data) {
  unsigned char IPHandler[4] = {0}; //need to parse into . separated values
  size_t index = 0;
  while (*data){
    if (isdigit((unsigned char)*data)){
      IPHandler[index] *= 10;
      IPHandler[index] += *data - '0';
    } else {
      index++;
    }
    data++;
  }
  sprintf(IPString, "%i, %i, %i, %i", IPHandler[0], IPHandler[1], IPHandler[2], IPHandler[3]);
  remoteIP = IPHandler;
  Particle.publish("remoteIPCallback", IPString);
  Particle.publish("remoteIPCallback", String(remoteIP));
}*/

void setup() {
    //Particle.subscribe("setRemoteIP-HRM", updateRemoteIP);
    //Particle.subscribe("setRemotePort-HRM", updateRemotePort);
	Serial.begin(115200);
   // udp.begin(0);
	// Serial1 RX is connected to Feather TX (1)
	// Serial2 TX is connected to Feather RX (0)
	// Photon GND is connected to Feather GND
  // Photon VIN is connected to Feather 3V3
	Serial1.begin(115200);
	//Particle.publish("HRM1-IP", String(WiFi.localIP())); //Publish IP via particle console
}

void loop() {
	/*if (millis() - lastSend >= SEND_INTERVAL_MS) {
		lastSend = millis();

		Serial1.printlnf("%d", ++counter);
		Serial.printlnf("Sent to Feather: %d", counter);
	}*/

	// Read data from serial
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
				processBuffer();
				bufferOffset = 0;
			}
		}
		else {
			Serial.println("buffer overflow, emptying buffer");
			bufferOffset = 0;
		}
	}
}

void processBuffer() {
    //int ret = snprintf(buffer, bufferSize, "battery: %f percent", soc);
    Serial.printlnf("Heart %s", buffer);
    if (udp.sendPacket(buffer, bufferSize, remoteIP, remotePort) >= 0) {
    }
    else {
        #ifdef SERIAL_DEBUG
            Serial.printlnf("UDP send failed");
        #endif
        // On error, wait a moment, then reinitialize UDP and try again.
        delay(1000);
        udp.begin(0);
    }
}
