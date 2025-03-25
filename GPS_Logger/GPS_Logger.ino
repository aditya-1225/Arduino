#include <TinyGPS++.h>
#include <SPI.h>
#include <SD.h>

File myFile;
const int chipSelect = 53;
TinyGPSPlus gps;
String gpsSentence = "";

// LED pins for different errors
const int LED_SD_FAIL = 12;
const int LED_FILE_ERROR = 11;
const int LED_NO_FIX = 10;
const int LED_NO_DATA = 9;

unsigned long lastGpsTime = 0;  // Stores last received data timestamp
const unsigned long timeoutDuration = 2000;  // 2-second timeout

void setup() {
    Serial.begin(9600);
    Serial1.begin(9600);

    // Set LED pins as OUTPUT
    pinMode(LED_SD_FAIL, OUTPUT);
    pinMode(LED_FILE_ERROR, OUTPUT);
    pinMode(LED_NO_FIX, OUTPUT);
    pinMode(LED_NO_DATA, OUTPUT);

    Serial.println("GPS Receiver Initialized");
    Serial.print("Initializing SD card...");

    if (!SD.begin(chipSelect)) {  // Use chipSelect argument
        Serial.println("SD Initialization Failed!");
        analogWrite(LED_SD_FAIL, 109);  // Turn on LED 12 if SD card fails
        return;
    }
    Serial.println("SD Initialization Done.");
}

void loop() {
    bool gpsDataReceived = false;  // Flag to check if GPS data is available

    while (Serial1.available()) {
        gpsDataReceived = true;  // Data is coming in
        lastGpsTime = millis();  // Update last data received time
        char c = Serial1.read();
        gps.encode(c);
        Serial.print(c);

        if (c == '\n') {
            if (gpsSentence.startsWith("$GNGGA")) {
                if (!gps.location.isValid()) {
                    analogWrite(LED_NO_FIX, 109);  // No fix, turn on LED 10
                } else {
                    analogWrite(LED_NO_FIX, 0);  // Fix found, turn off LED 10
                }
            }

            myFile = SD.open("test1.txt", FILE_WRITE);
            if (myFile) {
                myFile.println(gpsSentence);
                myFile.close();
                analogWrite(LED_FILE_ERROR, 0);  // Turn off error LED if writing is successful
            } else {
                Serial.println("File Write Error");
                analogWrite(LED_FILE_ERROR, 109);  // File write error, turn on LED 11
            }

            gpsSentence = "";  // Clear sentence buffer after processing
        } else {
            gpsSentence += c;
        }
    }

    // Check if 2+ seconds have passed without receiving GPS data
    if (millis() - lastGpsTime > timeoutDuration) {
        analogWrite(LED_NO_DATA, 109);  // No data → Turn on LED 9
    } else {
        analogWrite(LED_NO_DATA, 0);   // Data received recently → Turn off LED 9
    }
}
