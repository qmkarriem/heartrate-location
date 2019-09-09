#include "Particle.h"
const size_t bufferSize = 512;
char IPString[40];
IPAddress remoteIP(10,0,1,2);
int remotePort = 8888;
UDP udp;

void processBuffer();
int counter = 0;
char buffer[bufferSize];
size_t bufferOffset = 0;

SYSTEM_MODE(SEMI_AUTOMATIC);
void setup() {
	WiFi.on();
	WiFi.connect();
	while(!WiFi.ready()){
		delay(25);
	}
  udp.begin(8888);
	Serial1.begin(115200);
}

void loop() {
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
			//Serial.println("buffer overflow, emptying buffer");
			bufferOffset = 0;
		}
	}
}

void processBuffer() {
    if (udp.sendPacket(buffer, bufferSize, remoteIP, remotePort) >= 0) {}
    else {
        delay(1000); // On error, wait a moment, then reinitialize UDP and try again.
        udp.begin(8888);
    }
}
