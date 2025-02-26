/*
 * Created by ArduinoGetStarted.com
 *
 * This example code is in the public domain
 *
 * Tutorial page: https://arduinogetstarted.com/tutorials/arduino-rtc
 */

#include <RTClib.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
RTC_DS3231 rtc;
#define i2c_Address 0x3c
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1   //   QT-PY / XIAO
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
char daysOfTheWeek[7][12] = {
  "Sunday",
  "Monday",
  "Tuesday",
  "Wednesday",
  "Thursday",
  "Friday",
  "Saturday"
};

void setup () {
  Serial.begin(9600);
  display.begin(i2c_Address, 0x3C);  // here the 0x3c is the I2C address, check your i2c address if u have multiple devices.
  display.clearDisplay();
  // SETUP RTC MODULE
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1);
  }

  // automatically sets the RTC to the date & time on PC this sketch was compiled
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  // manually sets the RTC with an explicit date & time, for example to set
  // January 21, 2021 at 3am you would call:
  //rtc.adjust(DateTime(2023, 8, 11, 20, 35, 0));
}

void loop () {
  DateTime now = rtc.now();
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print("Date: ");
  display.print(now.day(), DEC);
  display.print('/');
  display.print(now.month(), DEC);
  display.print('/');
  display.print(now.year(), DEC);
  display.setTextSize(4);
// display.print(daysOfTheWeek[now.dayOfTheWeek()]);
display.setCursor(3, 20) ;
  if (now.hour()>12){
    display.print(now.hour()-12,DEC);
}
 else{
 display.print(now.hour(), DEC);
 }
  display.print(':');
  if(int(now.minute())<10){
  display.print("0") ;
  display.print(now.minute(),DEC);
  }
  else{
  display.print(now.minute(), DEC);}
 //display.print(':');
  //display.println(now.second(), DEC);
  display.display();
  delay(1000); // delay 1 seconds
  display.clearDisplay();
}
