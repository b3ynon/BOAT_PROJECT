#include <SD.h>
#include <SPI.h> // Required by SD.h

const int ledPin = 2;          // Renamed to match usage in loop
const int buttonPin = 3;       // Renamed to match usage in loop

// --- GLOBAL VARIABLES ---
int test_counter = 1;          // Initialize the counter
File dataFile;                 // Define the file object

void setup() {
  Serial.begin(9600);      // For PC Monitor
  Serial2.begin(9600);  // For Ebyte LoRa (Must match module baud)
  
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT);

  // Initialize SD Card (Teensy 4.1 specific)
  if (!SD.begin(BUILTIN_SDCARD)) {    
    Serial.println("SD Card Failed!");
    // Blink LED fast if SD fails
    while(1) {
      digitalWrite(ledPin, HIGH); delay(100);
      digitalWrite(ledPin, LOW); delay(100);
    }
  }
  Serial.println("System Ready. Press Button to start Test 1.");
}

void loop() {
  if (digitalRead(buttonPin) == HIGH) {   // checks for button press (Active Low)
   delay(200); // Debounce
   
   digitalWrite(ledPin, HIGH);   // LED on to indicate "Busy"
   Serial.print("Starting Test #");
   Serial.println(test_counter);

   // Create dynamic filename
   char filename[15]; 
   snprintf(filename, sizeof(filename), "TEST_%d.CSV", test_counter);

   // Send Trigger to Base Station
   // We use 'print' not 'println' so we don't send extra newline characters
   Serial2.print('S'); 
   Serial.println("Trigger Sent. Waiting for packets...");

   // Run the recording function
   run_test_sequence(filename);

   test_counter++;            // Increment for next distance
   digitalWrite(ledPin, LOW); // LED off
   Serial.println("Test Complete. Move to next location.");
  }
}

void run_test_sequence(const char* filename) {
  // 1. Open SD File
  dataFile = SD.open(filename, FILE_WRITE);
  
  if (dataFile) {
    dataFile.println("PacketCount, RSSI");
    Serial.println("File Created. Listening...");
  } else {
    Serial.println("Error opening SD file!");
    return;
  }

  // 2. Variables
  int packetsReceived = 0;
  unsigned long lastPacketTime = millis();
  const unsigned long timeout = 22000; // 22s Timeout (Allowing 20s for transmission + buffer)

  // 3. Listening Loop
  while (packetsReceived < 1000 && (millis() - lastPacketTime < timeout)) {
    
    // Check if data is available
    if (Serial2.available()) { 
      
      // Reset timeout watchdog
      lastPacketTime = millis();
      
      // A. Parse the Number (1, 2, 3...)
      // parseInt reads characters until it hits a non-digit (or timeout)
      int count = Serial2.parseInt(); 
      
      // B. Wait for RSSI Byte
      // CRITICAL: parseInt might finish before the RSSI byte arrives in the buffer.
      // We wait up to 10ms for that final byte to appear.
      unsigned long waitStart = millis();
      while (!Serial2.available() && (millis() - waitStart < 10));

      if (Serial2.available()) {
         // C. Read RSSI Byte
         int rssiRaw = Serial2.read();
         
         // Ebyte Logic: RSSI = -(256 - ByteVal)
         int rssi = -(256 - rssiRaw);
         
         // D. Log Data
         if (count > 0) { // Filter out any parsing zeros/errors
           dataFile.print(count);
           dataFile.print(",");
           dataFile.println(rssi);
           
           Serial.print("RX: "); Serial.print(count);
           Serial.print(" | Signal: "); Serial.println(rssi);
           
           packetsReceived++;
         }
      }
    }
  }

  // 4. Save and Close
  dataFile.close();
  Serial.println("Sequence Finished. Data Saved.");
}
