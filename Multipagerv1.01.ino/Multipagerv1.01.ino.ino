/**

  DEXTERPAGER WEB PAGER reader !
  Made for TTGo esp32 having TFT !
  10-2-2021 made by DexterBot
  it has 4 API servers having different outputs to show ,
  now all P2000 pager server ,
  use thingspeak.com to make a web page you like in 2 a API
  so you could use the data to show on your Dexterpager
*/
#define DEBUG 0 // 1 enable 0 disable (DEBUG) for serial output 115200 baud //
int AUTOSHOW = 0; // 0 disabled 1 enabled show's new message automativ on screen or hold both buttons on boot !!
#define SHOWBAT 10 // wil show bat level after 5 pages refreshed and also pulls mesages from site in this rate .
////////////////// setup your wifi here //////////////////
#define APPNAME "TELE2-EFC84F"    //Acces point name 
#define WPAKEY "C16BEBD9E8"      // WPA KEY 
///////////////////////////////////////////////////////////
#include <Arduino.h>
#include "Images.h"
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
WiFiMulti wifiMulti;
#include <SPI.h>
#include <TFT_eSPI.h> // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();
uint8_t selector1 = 0;
uint8_t selector2 = 0;
uint8_t selector3 = 0;
uint8_t mescounter = 0;
String checker; // new stuff i try //
String tmp2;
int vref = 1100;
///////////////////////////////////////////////////////////////////////////////////////////////////
#define SERVERAPI1 "https://api.thingspeak.com/apps/thinghttp/send_request?api_key=12BB7OMBEIN3IIJJ"
#define SERVERAPI2 "https://api.thingspeak.com/apps/thinghttp/send_request?api_key=Y8PWPLDACM1J84EQ"
#define SERVERAPI3 "https://api.thingspeak.com/apps/thinghttp/send_request?api_key=NVP6DSJDGEYPGZ22"
#define SERVERAPI4 "https://api.thingspeak.com/apps/thinghttp/send_request?api_key=DXLRW4WOOIRQWZSM"
#define TimeServer "https://api.thingspeak.com/apps/thinghttp/send_request?api_key=BZOJWIRDNJ6GAMM6"
//#define LED_PIN 21
#define ADC_EN              14  //ADC_EN is the ADC detection enable port
#define ADC_PIN             34
void setup() {
  pinMode(0, INPUT_PULLUP);
  pinMode(35, INPUT_PULLUP);
  //  pinMode(LED_PIN, OUTPUT);
  if (DEBUG == 1) Serial.begin(115200);
  tft.init();
  tft.fillScreen(TFT_BLACK);
  tft.setRotation(3);
  tft.setSwapBytes(true);
  tft.pushImage(0, 0,  240, 128, start);
  delay (3000);
  tft.fillScreen(TFT_BLACK);
  tft.setTextFont(2);
  tft.setTextColor(TFT_RED);
  tft.drawString("   MULTI RX ", 70, 15);
  tft.setTextFont(4);
  tft.drawString(" Made by dexterbot", 10, 50);
  tft.setTextFont(1);
  tft.drawString("   version 1.01", 66, 100);
  delay (2000);
  if (digitalRead(35) == LOW && digitalRead(0) == LOW) {
    animate();
    AUTOSHOW = 1;
  }
  tft.pushImage(0, 0,  240, 138, pager);
  for (uint8_t t = 4; t > 0; t--) {
    if (DEBUG == 1)  Serial.flush();
    tft.setTextColor(TFT_CYAN);
    tft.setCursor(53, 30);
    tft.printf("SYSTEM STARTING %d...\n", t);
    delay(1000);
    tft.pushImage(0, 0,  240, 138, pager);
  }
  tft.setTextColor(TFT_CYAN);
  tft.setCursor(62, 30);
  tft.printf("! SYSTEM LOADING !");
  tft.setTextColor(TFT_GREEN);
  tft.setCursor(190, 60);
  animate();
  delay(1000);
  wifiMulti.addAP(APPNAME, WPAKEY);
  checkmesage1();
  tft.pushImage(0, 0,  240, 138, pager);
  tft.setTextColor(TFT_CYAN);
  tft.setCursor(73, 30);
  tft.printf("! SYSTEM OK !");
  animate();
  tft.fillRect(25, 94, 26, 3, TFT_PURPLE);
  tft.fillRect(72, 94, 28, 3, TFT_PURPLE);
  delay(1000);
  pinMode(ADC_EN, OUTPUT);
  digitalWrite(ADC_EN, HIGH);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop() {
  mescounter = mescounter + 1;
  selector1 = digitalRead(35);
  delay(200);
  selector2 = digitalRead(0);
  delay(200);
  tft.pushImage(0, 0,  240, 138, pager);
  tft.fillRect(25, 94, 26, 3, TFT_PURPLE);
  tft.fillRect(72, 94, 28, 3, TFT_PURPLE);
  if (checker == tmp2) { // checks if messages are the same ,//
    tft.setTextFont(1);
    tft.setTextColor(TFT_RED);
    tft.setCursor(60, 60);
    tft.printf("! NO NEW MESSAGES !");
    tft.setCursor(190, 62);
    tft.setTextColor(TFT_YELLOW);
    tft.printf("< <");
    tft.fillRect(25, 94, 26, 3, TFT_PURPLE);
    tft.fillRect(72, 94, 28, 3, TFT_RED);
    delay(500);
  }
  if (checker != tmp2) { // checks if messages are the same ,//
    tft.setTextFont(1);
    tft.setTextColor(TFT_GREEN);
    tft.setCursor(60, 60);
    tft.printf("!  NEW MESSAGES  !");
    tft.setCursor(190, 62);
    tft.setTextColor(TFT_YELLOW);
    tft.printf(" < < ");
    tft.fillRect(25, 94, 26, 3, TFT_GREEN);
    tft.fillRect(72, 94, 28, 3, TFT_PURPLE);
    delay(500);
  }
  if (selector2 == 0) {
    tft.setTextColor(TFT_CYAN);
    tft.setTextFont(2);
    tft.pushImage(0, 0,  240, 138, pager);
    tft.drawString("SELECT SERVER", 70, 30);
    tft.setTextColor(TFT_GREEN);
    selector3 = selector3 + 1;
    if (selector3 > 3 )selector3 = 0;
    if (selector3 == 0 ) tft.drawString("1 P2000 ", 30, 60);
    if (selector3 == 1 ) tft.drawString("2 FIREDEPT ", 30, 60);
    if (selector3 == 2 ) tft.drawString("3 NOS NEWS ", 30, 60);
    if (selector3 == 3 ) tft.drawString("4 NOS WEER ", 30, 60);
    delay(2000);
    tft.pushImage(0, 0,  240, 135, pager);
    tft.drawString("SERVER SELECTED", 63, 30);
    delay(1000);
    tft.pushImage(0, 0,  240, 138, pager);
    tft.fillRect(25, 94, 26, 3, TFT_PURPLE);
    tft.fillRect(72, 94, 28, 3, TFT_PURPLE);
    animate();
    selector1 = 1;
    selector2 = 1;
  }
  if (AUTOSHOW == 0) {
    if (selector1 == 0) {
      gettime();
      getmesage();
    }
  }
  if (AUTOSHOW == 1) {
    if (selector1 == 0 || checker != tmp2) {
      selector1 = 0;
      gettime();
      getmesage();
      selector1 = 1;
    }
  }
  if (selector1 == 1) {
    tft.setTextColor(TFT_YELLOW);
    tft.setCursor(190, 62);
    tft.printf("< <");
    tft.setTextFont(2);
    tft.setTextColor(TFT_CYAN);
    tft.setCursor(26, 30);
    tft.printf("!        PUSH BUTTON        !");
    delay(1000);
    tft.pushImage(0, 0,  240, 138, pager);
    tft.fillRect(25, 94, 26, 3, TFT_PURPLE);
    tft.fillRect(72, 94, 28, 3, TFT_PURPLE);
    tft.setTextColor(TFT_CYAN);
    tft.setCursor(45, 30);
    tft.printf(" ! WAITING FOR DATA !");
    if (AUTOSHOW == 1) {
      tft.setCursor(194, 40);
      tft.setTextColor(TFT_PURPLE);
      tft.printf("AT");
    }
    animate();
  }
  if (mescounter == SHOWBAT) {
    printVolts();
    checkmesage1();
    mescounter = 0;
  }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void gettime() {
  tft.setTextColor(TFT_YELLOW);
  if (selector1 == 0) {
    tft.pushImage(0, 0,  240, 138, ano);
    tft.setTextColor(TFT_YELLOW);
  }
  if ((wifiMulti.run() == WL_CONNECTED)) {
    if (selector1 == 1) {
      tft.setTextColor(TFT_CYAN);
    }
    HTTPClient http;
    http.begin(TimeServer);
    int httpCode = http.GET();
    String payload = http.getString();
    char buf[1000];
    payload.toCharArray(buf, 1000);
    String tmp2 = buf ;
    tmp2.trim();
    tft.fillScreen(TFT_BLACK);
    if (selector1 == 0) {
      tft.setCursor(5, 23);
      tft.setTextFont(1);
      tft.println(tmp2);
      tft.setCursor(3, 19);
      tft.drawLine(0, 10, 239, 10, TFT_RED);
      tft.drawLine(0, 40, 239, 40, TFT_RED);
      tft.setCursor(85, 23);
      if (selector3 == 0 )  tft.printf(" ! P2000 !");
      if (selector3 == 1 )  tft.printf(" ! FiREDepT !");
      if (selector3 == 2 )  tft.printf(" ! NEWS !");
      if (selector3 == 3 )  tft.printf(" ! WHEATER !");
    }
    if (selector1 == 1) {
      tft.setCursor(30, 62);
      tft.setTextFont(1);
      tft.println(tmp2);
    }
    if (DEBUG == 1) Serial.println(tmp2);
    http.end();
    if (selector1 == 0)delay(3000);
  }
}

void getmesage() {
  if ((wifiMulti.run() == WL_CONNECTED)) {
    tft.setTextColor(TFT_YELLOW);
    HTTPClient http;
    if (DEBUG == 1)  Serial.print("[HTTP] begin...\n");
    if (selector3 == 0 )  http.begin (SERVERAPI1);// 3 different API inputs //
    if (selector3 == 1 ) http.begin(SERVERAPI2); //
    if (selector3 == 2 )http.begin(SERVERAPI3); //
    if (selector3 == 3 )http.begin(SERVERAPI4); //
    int httpCode = http.GET();
    String payload = http.getString();
    char buf[1000];
    payload.toCharArray(buf, 1000);
    String tmp2 = buf ;
    checker = tmp2; // new stuff i try //
    ///////////////////////////////////////////////////////////////////
    if (selector3 == 0 || selector3 == 1)tft.pushImage(200, 88,  30, 28, light);
    if (selector3 == 3 )tft.pushImage(185, 83,  50, 50, weer);
    if (selector3 == 2 )tft.pushImage(185, 83,  50, 50, NEWS);
    tmp2.trim();
    tft.setCursor(0, 50);
    if (selector3 == 0 ) tft.setTextFont(2);
    if (selector3 == 1 ) tft.setTextFont(2);
    if (selector3 == 2 ) tft.setTextFont(2);
    if (selector3 == 3 ) tft.setTextFont(2);
    tft.println(tmp2);
    if (DEBUG == 1)  Serial.println(tmp2);
    http.end();
    delay(8000);
    tft.pushImage(0, 0,  240, 138, pager);
    tft.setTextFont(1);
    selector1 = 1;
  }
}

void checkmesage1() {
  if (digitalRead(0) == 0) return;
  if (digitalRead(35) == 0) return;
  if ((wifiMulti.run() == WL_CONNECTED)) {
    HTTPClient http;
    if (selector3 == 0 )http.begin (SERVERAPI1);// 3 different API inputs //
    if (selector3 == 1 )http.begin(SERVERAPI2); //
    if (selector3 == 2 )http.begin(SERVERAPI3); //
    if (selector3 == 3 )http.begin(SERVERAPI4); //
    int httpCode = http.GET();
    String payload = http.getString();
    char buf[1000];
    payload.toCharArray(buf, 1000);
    tmp2 = buf ;
    http.end();
  }
}
void printVolts()
{
  tft.pushImage(0, 0,  240, 138, pager);
  tft.fillRect(25, 94, 26, 3, TFT_PURPLE);
  tft.fillRect(72, 94, 28, 3, TFT_PURPLE);
  tft.setTextColor(TFT_CYAN);
  tft.setTextFont(2);
  uint16_t v = analogRead(34); //read the A0 pin value
  delay (200);
  float battery_voltage = ((float)v / 4095.0) * 2.0 * 3.3 * (vref / 1000.0);

  if (battery_voltage < 3.90) //set the voltage considered low battery here
  {
    tft.setTextColor(TFT_RED);
    tft.setCursor(26, 30);
    tft.printf("!!!!  -!!--LOWPOWER--!!-  !!!!");
    delay (200);
    tft.setCursor(191, 27);
    tft.printf("     ");
  }
  if (battery_voltage > 3.90) {
    tft.setCursor(26, 30);
    tft.printf("!!!!!!   BATTERY VOLTAGE  !!!!!!");
    tft.setCursor(80, 60);
    tft.setTextColor(TFT_RED);
    tft.println(battery_voltage);
    tft.setCursor(105, 60);
    tft.println(" = Volt");
    delay (1200);
  }
}

void animate() {
  tft.setTextColor(TFT_YELLOW);
  tft.setCursor(190, 62);
  tft.printf("<<");
  delay(100);
  tft.setCursor(170, 62);
  tft.printf("<<");
  delay(100);
  tft.setCursor(150, 62);
  tft.printf("<<");
  delay(100);
  tft.setCursor(130, 62);
  tft.printf("<<");
  delay(100);
  tft.setCursor(110, 62);
  tft.printf("<<");
  delay(100);
  tft.setCursor(90, 62);
  tft.printf("<<");
  delay(100);
  tft.setCursor(70, 62);
  tft.printf("<<");
  delay(100);
  tft.setCursor(50, 62);
  tft.printf("<<");
  delay(100);
  tft.setCursor(30, 62);
  tft.printf("<<");
  delay(100);
}

