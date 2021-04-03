#include <Arduino.h>
#include <ArduinoOTA.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Daikin.h>
#define BLYNK_PRINT Serial

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#else
#include <WiFi.h>
#endif  // ESP8266
#if defined(ESP32)
#include <BlynkSimpleEsp32.h>
#endif  // ESP32

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "2tIJBr2gzkZwW_ZuxAJEQHKeoBTBQXvE";
char ssid[] = "Krukeng biochem tutor_2.4GHz";
char pass[] = "houng5314";

const uint16_t kIrLed = 4;  // ESP8266 GPIO pin to use. Recommended: GPIO4 (D2)
IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message
IRDaikin64 ac(kIrLed);   // Set the GPIO to be used to sending AC message
BlynkTimer timer;
WidgetLED led1(V1);
WidgetLED led2(V2);
WidgetLED led3(V3);
WidgetLED led4(V4);

int pinValueV10;
int pinValueV11;
int pinValueV12;
int temp;
int toggle = 0;
int period = 1000;
int x = 0;
int y = 0;
int pinValueV100 = 0;
unsigned long time_now = 0;
int sensorData = 0;

//Raw Data Fan
uint16_t rawData[139] = {9014, 4464,  674, 1634,  674, 532,  674, 532,  674, 1634,  674, 1632,  674, 1656,  650, 532,  674, 532,  674, 530,  674, 532,  672, 1632,  676, 530,  672, 532,  674, 532,  674, 532,  674, 530,  674, 532,  672, 532,  674, 532,  674, 532,  674, 532,  674, 1656,  652, 1634,  672, 1656,  652, 532,  674, 532,  674, 530,  674, 532,  674, 1654,  652, 532,  674, 1634,  674, 534,  672, 532,  674, 1634,  674, 532,  674, 19968,  674, 532,  674, 532,  672, 532,  674, 532,  674, 532,  672, 532,  674, 532,  674, 532,  674, 1656,  650, 1656,  652, 1656,  650, 532,  674, 532,  674, 1632,  676, 532,  674, 530,  674, 532,  674, 532,  674, 532,  674, 532,  672, 532,  674, 532,  674, 532,  674, 532,  674, 532,  674, 532,  674, 532,  674, 530,  674, 1634,  674, 532,  674, 532,  674, 1656,  652};  // GREE
uint8_t state[8] = {0x39, 0x04, 0xE0, 0x50, 0x00, 0x27, 0x00, 0x90};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Ceiling FAN   
BLYNK_WRITE(V50) // Off Button
{
  if ( param.asInt() ) // Off Fan
  {
  irsend.sendGree(0x09086050002000D0, 64);
  led4.on();
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BLYNK_WRITE(V51) // Low Speed Button
{
  if ( param.asInt() ) // Low Speed Fan
  {
  irsend.sendGree(0x0108205000200050, 64);
  led4.off();
  }
}

BLYNK_WRITE(V60) // Off Button
{
  if ( param.asInt() ) // Off Fan
  {
  irsend.sendGree(0x09086050002000D0, 64);
  led3.on();
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BLYNK_WRITE(V61) // Low Speed Button
{
  if ( param.asInt() ) // Low Speed Fan
  {
  irsend.sendGree(0x0108205000200050, 64);
  led3.off();
  }
}

BLYNK_WRITE(V70) // Off Button
{
  if ( param.asInt() ) // Off Fan
  {
  irsend.sendGree(0x09086050002000D0, 64);
  led2.on();
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BLYNK_WRITE(V71) // Low Speed Button
{
  if ( param.asInt() ) // Low Speed Fan
  {
  irsend.sendGree(0x0108205000200050, 64);
  led2.off();
  }
}

BLYNK_WRITE(V80) // Off Button
{
  if ( param.asInt() ) // Off Fan
  {
  irsend.sendGree(0x09086050002000D0, 64);
  led1.on();
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BLYNK_WRITE(V81) // Low Speed Button
{
  if ( param.asInt() ) // Low Speed Fan
  {
  irsend.sendGree(0x0108205000200050, 64);
  led1.off();
  }
}







BLYNK_WRITE(V82) // Medium Speed Button
{
  if ( param.asInt() ) // Medium Speed Fan
  {
  irsend.sendSymphony(0x13E, 11);
  }
}
/*
BLYNK_WRITE(V83) // High Speed Button
{
  if ( param.asInt() ) // High Speed Fan
  { irsend.sendRaw(rawData, 91, 38);
  }
}
*/
void sendAnalog()
{
  sensorData = analogRead(A0); //reading the analog input on A0
  if (sensorData >= 860 && sensorData <=880 && sensorData != 1024 && y < 24) {
  y++;
  }
  else if (sensorData >= 860 && sensorData <=880 && sensorData != 1024 && y >=24) {
//  Serial.println("AC is OFF");
    if (pinValueV100 == 1) {
    Blynk.virtualWrite(100,0);
    led1.off();
    pinValueV100 = 0;
    }
    else {
    }
  }
  else if (sensorData >= 200 && sensorData <=400 && sensorData != 1024) {
  y = 0;
//  Serial.println("AC is ON");
    if (pinValueV100 == 0) {
    Blynk.virtualWrite(100,1);
    led1.on();
    pinValueV100 = 1;
    }
    else {
    }
  }
}

void setup() {
  timer.setInterval(50L, sendAnalog);
  irsend.begin();
  ac.begin();
#if ESP8266
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
#else  // ESP8266
  Serial.begin(115200, SERIAL_8N1);
#endif  // ESP8266

//  Blynk.begin(auth, ssid, pass);
  Blynk.begin(auth, ssid, pass, "blynk.iot-cm.com", 8080);//blynk local server

  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  pinValueV10 = 1 ; //set AC mode to Cool by on boot time
  pinValueV12 = 1 ; //set AC fan speed to Auto on boot time
  Blynk.virtualWrite(100,0);
}

void loop() {
  ArduinoOTA.handle();
  Blynk.run();
  timer.run();
}
