// Settings.h
#ifndef SETTINGS_H
#define SETTINGS_H

#include "credentials.h"

// ---------------------- General settings ---------------------- //
extern char *DUCO_USER = (char*)CONF_DUCO_USER; 
extern char *MINER_KEY = (char*)CONF_MINER_KEY; 
extern char *RIG_IDENTIFIER = (char*)CONF_RIG_IDENTIFIER; 

// Note: SSIDs, PASSWORDS, and NUM_NETWORKS are now handled in credentials.h
// -------------------------------------------------------------- //

// -------------------- Advanced options ------------------------ //
#define WEB_DASHBOARD
#define LED_BLINKING
// #define USE_LAN
#define SERIAL_PRINTING
#define SERIAL_BAUDRATE 500000
#define LWD_TIMEOUT 30000
// #define DISABLE_BROWNOUT

// --> CAPTIVE PORTAL IS NOW ENABLED <--
#define CAPTIVE_PORTAL
// -------------------------------------------------------------- //

// ------------------------ Displays ---------------------------- //
// #define DISPLAY_SSD1306
// #define DISPLAY_16X2
// #define BLUSHYBOX
// #define DISPLAY_ST7789
// -------------------------------------------------------------- //

// ---------------------- IoT examples -------------------------- //
 #define USE_INTERNAL_SENSOR
// #define USE_DS18B20
// #define USE_DHT
// #define USE_HSU07M
// -------------------------------------------------------------- //

// ---------------- Variables and definitions ------------------- //
#if defined(ESP8266)
    #define LED_BUILTIN 2
#elif defined(CONFIG_FREERTOS_UNICORE) 
    #if defined(CONFIG_IDF_TARGET_ESP32C3)
      #define LED_BUILTIN 8
    #else
      #define LED_BUILTIN 15
    #endif
#else
    #ifndef LED_BUILTIN
      #define LED_BUILTIN 2
    #endif
    #if defined(BLUSHYBOX)
      #define LED_BUILTIN 4
    #endif
#endif

#define BLINK_SETUP_COMPLETE 2
#define BLINK_CLIENT_CONNECT 5
#define SOFTWARE_VERSION "4.3"

extern unsigned int hashrate = 0;
extern unsigned int hashrate_core_two = 0;
extern unsigned int difficulty = 0;
extern unsigned long share_count = 0;
extern unsigned long accepted_share_count = 0;
extern String node_id = "";
extern String WALLET_ID = "";
extern unsigned int ping = 0;

#if defined(USE_DS18B20)
  #include <OneWire.h>
  #include <DallasTemperature.h>
  const int DSPIN = 12;
  OneWire oneWire(DSPIN);
  DallasTemperature extern sensors(&oneWire);
#endif

#if defined(USE_DHT)
  #include <DHT.h>
  #define DHTPIN 12
  #define DHTTYPE DHT11
  DHT extern dht(DHTPIN, DHTTYPE);
#endif

#if defined(DISPLAY_SSD1306)
    #include <U8g2lib.h>
    #include <Wire.h>
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
#endif

#if defined(DISPLAY_16X2)
    #include "Wire.h"
    #include "Adafruit_LiquidCrystal.h"
    Adafruit_LiquidCrystal lcd(1, 2, 3, 4, 5, 6);
#endif

#if defined(DISPLAY_ST7789)
    #include <TFT_eSPI.h>
    TFT_eSPI tft = TFT_eSPI();
#endif

#if defined(USE_HSU07M)
    #include "Wire.h"
    #define HSU07M_ADDRESS 0x4B 
    float read_hsu07m() {
      Wire.beginTransmission(HSU07M_ADDRESS);
      Wire.write(0x00);
      Wire.endTransmission();
      delay(100);
      Wire.requestFrom(HSU07M_ADDRESS, 2);
      if(Wire.available() >= 2) {
          byte tempMSB = Wire.read();
          byte tempLSB = Wire.read();
          int tempRaw = (tempMSB << 8) | tempLSB;
          float tempC = (tempRaw / 16.0) - 40.0;
          return tempC;
      }
      return -1.0;
    }
#endif

#if defined(BLUSHYBOX)
    #define GAUGE_PIN 5
    #define GAUGE_MAX 190
    #define GAUGE_MIN 0
    #if defined(ESP8266)
      #define GAUGE_MAX_HR 80000
    #else
      #define GAUGE_MAX_HR 200000
    #endif
    extern float hashrate_old = 0.0;

    void gauge_set(float hashrate) {
        float old = hashrate_old;
        float new_val = hashrate;

        if (hashrate_old == 0) {
          float delta = (new_val - old) / 50;
          for (int x=0; x < 50; x++) {
              analogWrite(5, map(old + x*delta, 0, GAUGE_MAX_HR, GAUGE_MIN, GAUGE_MAX) + random(0, 10));
              delay(20);
          }
        } else {
          float delta = (new_val - old) / 10;
          for (int x=0; x < 10; x++) {
              analogWrite(5, map(old + x*delta, 0, GAUGE_MAX_HR, GAUGE_MIN, GAUGE_MAX) + random(0, 10));
              delay(10);
          }
        }
        hashrate_old = hashrate;
    }
#endif

IPAddress DNS_SERVER(1, 1, 1, 1); 

#endif