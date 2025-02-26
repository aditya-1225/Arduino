#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(9, 2);  // CE, CSN

const byte address[6] = "00001";

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(1, address);
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening();
  radio.setChannel(100);
  radio.setDataRate(RF24_250KBPS);
}
struct Data {

  float t;
  float p;

} data;


void loop() {
  if (radio.available()) {
    radio.read(&data, sizeof(data));

    
    float temperature = data.t;
    float pressure = data.p;

    Serial.print(temperature);
    Serial.print(",");
    Serial.println(pressure);
    


  }
  
}