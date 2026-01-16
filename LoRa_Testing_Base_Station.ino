#include <SoftwareSerial.h>   // to avoid interference with the unos serial pins and the serial monitor
SoftwareSerial LoRa(2, 3);

void setup() {
  Serial.begin(9600);
  LoRa.begin(9600);

  Serial.println("Base Station Ready. Waiting for Trigger 'S'...");
}


void loop() {
  // 1. Listen for the Trigger from the Teensy
  if (LoRa.available()) {
    char incoming = LoRa.read();
    
    // 2. If we receive 'S', start the burst
    if (incoming == 'S') {
      Serial.println("Trigger 'S' Received! Starting Test Sequence...");
      send_burst_sequence();
      Serial.println("Sequence Finished. Waiting for next trigger.");
    }
  }
}


//function to send the bust 
void send_burst_sequence() {
  // Loop from 1 to 500
  for (int i = 1; i <= 500; i++) {
    
    // SEND PACKET
    LoRa.print(i);
    LoRa.print(","); 
    
    // Debug to your screen
    Serial.print("Sent: ");
    Serial.println(i);
    
    //  TIMING
    // 300ms delay.
    //  had to increase delay to accuratley measuere packer loss 
    delay(300);
  }
}
