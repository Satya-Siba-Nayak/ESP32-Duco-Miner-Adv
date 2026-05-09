#pragma GCC optimize("-Ofast")
#include <ArduinoJson.h>

#if defined(ESP8266)
    #include <ESP8266WiFi.h>
    #include <ESP8266WiFiMulti.h> 
    #include <ESP8266mDNS.h>
    #include <ESP8266HTTPClient.h>
    #include <ESP8266WebServer.h>
    ESP8266WiFiMulti wifiMulti;
#else
    #include <ESPmDNS.h>
    #include <WiFi.h>
    #include <WiFiMulti.h>
    #include <HTTPClient.h>
    #include <WebServer.h>
    #include <WiFiClientSecure.h>
    WiFiMulti wifiMulti;
#endif

#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <WiFiClient.h>
#include <Ticker.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "MiningJob.h"
#include "Settings.h"

#ifdef USE_LAN
  #include <ETH.h>
#endif

#if defined(WEB_DASHBOARD)
  #include "Dashboard.h"
#endif

#if defined(DISPLAY_SSD1306) || defined(DISPLAY_16X2) || defined(DISPLAY_ST7789)
  #include "DisplayHal.h"
#endif

#if !defined(ESP8266) && defined(DISABLE_BROWNOUT)
    #include "soc/soc.h"
    #include "soc/rtc_cntl_reg.h"
#endif

#if defined(ESP8266)
    #define CORE 1
    typedef ESP8266WebServer WebServer;
#elif defined(CONFIG_FREERTOS_UNICORE)
    #define CORE 1
#else
    #define CORE 2
    #include <TridentTD_EasyFreeRTOS32.h>
    void Task1Code( void * parameter );
    void Task2Code( void * parameter );
    TaskHandle_t Task1;
    TaskHandle_t Task2;
#endif

#if defined(WEB_DASHBOARD)
    WebServer server(80);
#endif 

#if defined(CAPTIVE_PORTAL)
  #include <FS.h> 
  #include <WiFiManager.h>
  #include <Preferences.h>

  char duco_username[40];
  char duco_password[40];
  char duco_rigid[24];

  WiFiManager wifiManager;
  Preferences preferences;

  WiFiManagerParameter custom_duco_username("duco_usr", "Duino-Coin username", duco_username, 40);
  WiFiManagerParameter custom_duco_password("duco_pwd", "Duino-Coin mining key (if enabled in the wallet)", duco_password, 40);
  WiFiManagerParameter custom_duco_rigid("duco_rig", "Custom miner identifier (optional)", duco_rigid, 24);

  void saveConfigCallback() {
    preferences.begin("duino_config", false);
    preferences.putString("duco_username", custom_duco_username.getValue());
    preferences.putString("duco_password", custom_duco_password.getValue());
    preferences.putString("duco_rigid", custom_duco_rigid.getValue());
    preferences.end();
    RestartESP("Settings saved");
  }

  void reset_settings() {
    server.send(200, "text/html", "Settings have been erased.");
    delay(500);
    wifiManager.resetSettings();
    RestartESP("Manual settings reset");
  }

  void saveParamCallback(){
    Serial.println("[CALLBACK] saveParamCallback fired");
  }

  String getParam(String name){
    String value;
    if(wifiManager.server->hasArg(name)) {
      value = wifiManager.server->arg(name);
    }
    return value;
  }
#endif

void RestartESP(String msg) {
  #if defined(SERIAL_PRINTING)
    Serial.println(msg);
  #endif
  #if defined(ESP8266)
    ESP.reset();
  #else
    ESP.restart();
    abort();
  #endif
}

#if defined(ESP8266)
    Ticker lwdTimer;
    unsigned long lwdCurrentMillis = 0;
    unsigned long lwdTimeOutMillis = LWD_TIMEOUT;

    void ICACHE_RAM_ATTR lwdtcb(void) {
      if ((millis() - lwdCurrentMillis > LWD_TIMEOUT) || (lwdTimeOutMillis - lwdCurrentMillis != LWD_TIMEOUT))
        RestartESP("Loop WDT Failed!");
    }

    void lwdtFeed(void) {
      lwdCurrentMillis = millis();
      lwdTimeOutMillis = lwdCurrentMillis + LWD_TIMEOUT;
    }
#else
    void lwdtFeed(void) {}
#endif

// --- GLOBALS & NAMESPACE START ---
bool isBootBoostActive = true;
unsigned long bootBoostStartTime = 0;
const unsigned long BOOT_BOOST_DURATION = 120000; // 2 minutes

namespace {
    MiningConfig *configuration = new MiningConfig(
        DUCO_USER,
        RIG_IDENTIFIER,
        MINER_KEY
    );
    
    #if defined(ESP32) && CORE == 2
      EasyMutex mutexClientData, mutexConnectToServer;
    #endif

    #ifdef USE_LAN
      static bool eth_connected = false;
    #endif

    void UpdateHostPort(String input) {
        DynamicJsonDocument doc(256);
        deserializeJson(doc, input);
        const char *name = doc["name"];
        configuration->host = doc["ip"].as<String>();
        configuration->port = doc["port"].as<int>();
        node_id = String(name);
    }

    void SetupWifi() {
      // Stub for legacy call handling
    }

    void VerifyWifi() {
      #ifdef USE_LAN
        while ((!eth_connected) || (ETH.localIP() == IPAddress(0, 0, 0, 0))) {
          delay(500);
        }
      #else
        while (WiFi.status() != WL_CONNECTED 
                || WiFi.localIP() == IPAddress(0, 0, 0, 0)
                || WiFi.localIP() == IPAddress(192, 168, 4, 2) 
                || WiFi.localIP() == IPAddress(192, 168, 4, 3)) {
            WiFi.disconnect();
            delay(500);
            wifiMulti.run();
            delay(500);
        }
      #endif
    }

    String httpGetString(String URL) {
        String payload = "";
        WiFiClientSecure client;
        HTTPClient https;
        client.setInsecure();
        https.begin(client, URL);
        https.addHeader("Accept", "*/*");
        
        int httpCode = https.GET();
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            payload = https.getString();
        } else {
            VerifyWifi();
        }
        https.end();
        return payload;
    }

    void SelectNode() {
        String input = "";
        int waitTime = 1;
        while (input == "") {
            delay(waitTime * 1000);
            input = httpGetString("https://server.duinocoin.com/getPool");
            waitTime *= 2;
            if (waitTime > 32) RestartESP("Node fetch unavailable");
        }
        UpdateHostPort(input);
    }

    void SetupOTA() {
        ArduinoOTA.onStart([]() {});
        ArduinoOTA.onEnd([]() {});
        ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {});
        ArduinoOTA.onError([](ota_error_t error) {});
        ArduinoOTA.setHostname(RIG_IDENTIFIER);
        ArduinoOTA.begin();
    }

    #if defined(WEB_DASHBOARD)
        void dashboard() {
             String s = WEBSITE;

             #ifdef USE_LAN
              s.replace("@@IP_ADDR@@", ETH.localIP().toString());
             #else
              s.replace("@@IP_ADDR@@", WiFi.localIP().toString());
             #endif
             s.replace("@@HASHRATE@@", String((hashrate+hashrate_core_two) / 1000));
             s.replace("@@DIFF@@", String(difficulty / 100));
             s.replace("@@SHARES@@", String(share_count));
             s.replace("@@NODE@@", String(node_id));
             
             #if defined(ESP8266)
                 s.replace("@@DEVICE@@", "ESP8266");
             #elif defined(CONFIG_FREERTOS_UNICORE)
                 s.replace("@@DEVICE@@", "ESP32-S2/C3");
             #else
                 s.replace("@@DEVICE@@", "ESP32");
             #endif
             
             s.replace("@@ID@@", String(RIG_IDENTIFIER));
             s.replace("@@USER@@", String(configuration->DUCO_USER));
             s.replace("@@MEMORY@@", String(ESP.getFreeHeap()));
             s.replace("@@VERSION@@", String(SOFTWARE_VERSION));

             #if defined(CAPTIVE_PORTAL)
                 s.replace("@@RESET_SETTINGS@@", "&bull; <a href='/reset'>Reset settings</a>");
             #else
                 s.replace("@@RESET_SETTINGS@@", "");
             #endif
             
             server.send(200, "text/html", s);
        }

        void handleApi() {
             unsigned long currentMillis = millis();
             int uptime_sec = currentMillis / 1000;
             int days = currentMillis / 86400000;
             int hours = (currentMillis / 3600000) % 24;
             int mins = (currentMillis / 60000) % 60;
             String uptimeStr = String(days) + "d " + String(hours) + "h " + String(mins) + "m";
             
             // Dynamic Sensor Reading Block
             float tempVal = 0.0;
             #if defined(USE_DS18B20)
                 tempVal = sensors.getTempCByIndex(0);
             #elif defined(USE_DHT)
                 tempVal = dht.readTemperature();
             #elif defined(USE_HSU07M)
                 tempVal = read_hsu07m();
             #elif defined(USE_INTERNAL_SENSOR)
                 tempVal = temperatureRead();
             #endif

             #if defined(ESP8266)
                 String chipModel = "ESP8266";
                 int cpuFreq = ESP.getCpuFreqMHz();
             #elif defined(CONFIG_FREERTOS_UNICORE)
                 String chipModel = "ESP32-S2/C3";
                 int cpuFreq = getCpuFrequencyMhz();
             #else
                 String chipModel = "ESP32 (Dual Core)";
                 int cpuFreq = getCpuFrequencyMhz();
             #endif

             int rssi = WiFi.RSSI();
             String bssid = WiFi.BSSIDstr();
             String mac = WiFi.macAddress();
             int channel = WiFi.channel();

             String json = "{";
             json += "\"hashrate\":" + String((hashrate+hashrate_core_two) / 1000) + ",";
             json += "\"hr0\":" + String(hashrate / 1000) + ",";
             json += "\"hr1\":" + String(hashrate_core_two / 1000) + ",";
             json += "\"diff\":" + String(difficulty / 100) + ",";
             json += "\"shares\":" + String(share_count) + ",";
             json += "\"accepted\":" + String(accepted_share_count) + ",";
             json += "\"ping\":" + String(ping) + ",";
             json += "\"node\":\"" + String(node_id) + "\",";
             json += "\"memory\":" + String(ESP.getFreeHeap()) + ",";
             json += "\"temp\":" + String(tempVal) + ","; 
             json += "\"uptime\":\"" + uptimeStr + "\",";
             json += "\"uptime_sec\":" + String(uptime_sec) + ",";
             json += "\"chip\":\"" + chipModel + "\",";
             json += "\"freq\":" + String(cpuFreq) + ",";
             json += "\"rssi\":" + String(rssi) + ",";
             json += "\"bssid\":\"" + bssid + "\",";
             json += "\"mac\":\"" + mac + "\",";
             json += "\"channel\":" + String(channel) + ",";
             json += "\"boot_boost\":\"" + String(isBootBoostActive ? "true" : "false") + "\",";
             json += "\"user\":\"" + String(configuration->DUCO_USER) + "\"";
             json += "}";
             
             server.send(200, "application/json", json);
        }
    #endif
} // --- NAMESPACE END ---

MiningJob *job[CORE];

#if CORE == 2
  EasyFreeRTOS32 task1, task2;
#endif

void checkBootBoost() {
    if (isBootBoostActive && (millis() - bootBoostStartTime > BOOT_BOOST_DURATION)) {
        #if defined(ESP8266)
            // Do nothing
        #elif defined(CONFIG_FREERTOS_UNICORE)
            setCpuFrequencyMhz(160);
        #else
            setCpuFrequencyMhz(160);
        #endif
        isBootBoostActive = false;
    }
}

void task1_func(void *) {
    #if defined(ESP32) && CORE == 2
      VOID SETUP() { }
      VOID LOOP() {
        job[0]->mine();
      }
    #endif
}

void task2_func(void *) {
    #if defined(ESP32) && CORE == 2
      VOID SETUP() {
        job[1] = new MiningJob(1, configuration);
      }
      VOID LOOP() {
        job[1]->mine();
      }
    #endif
}

void setup() {
    #if !defined(ESP8266) && defined(DISABLE_BROWNOUT)
        WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
    #endif
    
    #if defined(SERIAL_PRINTING)
        Serial.begin(SERIAL_BAUDRATE);
        Serial.println("\n\nDuino-Coin " + String(configuration->MINER_VER));
    #endif
    
    pinMode(LED_BUILTIN, OUTPUT);
    assert(CORE == 1 || CORE == 2);
    randomSeed(ESP.getEfuseMac());
    WALLET_ID = String(random(0, 2811));
    job[0] = new MiningJob(0, configuration);
    
    WiFi.mode(WIFI_STA); 

    #if defined(ESP8266)
        WiFi.setSleepMode(WIFI_NONE_SLEEP);
    #else
        WiFi.setSleep(false);
    #endif
    
    #if defined(CAPTIVE_PORTAL)
        preferences.begin("duino_config", false);
        //Temp fix for wiping flash memory
        //preferences.clear();
        strcpy(duco_username, preferences.getString("duco_username", DUCO_USER).c_str());
        strcpy(duco_password, preferences.getString("duco_password", MINER_KEY).c_str());
        strcpy(duco_rigid, preferences.getString("duco_rigid", RIG_IDENTIFIER).c_str());
        
        String travel_ssid = preferences.getString("travel_ssid", "");
        String travel_pass = preferences.getString("travel_pass", "");
        preferences.end();

        configuration->DUCO_USER = duco_username;
        configuration->RIG_IDENTIFIER = duco_rigid;
        configuration->MINER_KEY = duco_password;

        RIG_IDENTIFIER = duco_rigid;

        for(int i = 0; i < NUM_NETWORKS; i++) {
            wifiMulti.addAP(SSIDs[i], PASSWORDS[i]);
        }

        if (travel_ssid != "") {
            wifiMulti.addAP(travel_ssid.c_str(), travel_pass.c_str());
        }

        bool connected = false;
        unsigned long scanStart = millis();
        
        // Scan for 15 seconds
        while(millis() - scanStart < 15000) {
            if(wifiMulti.run() == WL_CONNECTED) {
                connected = true;
                break;
            }
            delay(500);
        }

        if (!connected) {
            #if defined(SERIAL_PRINTING)
                Serial.println("No VIP networks found. Deploying setup AP...");
            #endif

            // Radio Fix
            WiFi.disconnect(true);
            WiFi.mode(WIFI_AP_STA);
            delay(500);
            
            String captivePortalHTML = R"(
              <title>Miner Setup</title>
              <style>
                body { background-color: #050507; color: #06b6d4; font-family: 'JetBrains Mono', monospace; padding: 20px; text-align: center;}
                button { background-color:#f59e0b; border-radius:8px; border:none; cursor:pointer; color:#111; font-weight:bold; padding:12px 20px; width:100%; margin-top: 15px; transition: 0.3s; }
                button:hover { background-color:#d97706; }
                input { background-color:#0e0e11; color:#fff; border-radius:8px; border:1px solid #222228; padding:10px; width:100%; margin-bottom: 10px; text-align: center; }
                h1 { color: #f59e0b; }
              </style>
            )";
          
            wifiManager.setConfigPortalTimeout(180); // 3-minute timeout before it reboots to try again
            wifiManager.setCustomHeadElement(captivePortalHTML.c_str());
            wifiManager.setSaveConfigCallback(saveConfigCallback);
            wifiManager.addParameter(&custom_duco_username);
            wifiManager.addParameter(&custom_duco_password);
            wifiManager.addParameter(&custom_duco_rigid);

            // Deploy a dynamic AP name based on your RIG ID
            String apName = "Duino-Miner-" + String(RIG_IDENTIFIER);
            
            if (!wifiManager.startConfigPortal(apName.c_str())) {
              #if defined(SERIAL_PRINTING)
                Serial.println("Failed to connect and hit timeout. Rebooting...");
              #endif
              delay(3000);
              RestartESP("Timeout in setup mode");
            }

            // Save the newly added travel/fallback credentials
            preferences.begin("duino_config", false);
            preferences.putString("travel_ssid", WiFi.SSID());
            preferences.putString("travel_pass", WiFi.psk());
            preferences.end();
            
            RestartESP("New Network Saved! Rebooting into mining mode...");
        }

        VerifyWifi();
        SelectNode();
    #endif

    SetupOTA();
    
    #if defined(WEB_DASHBOARD)
      if (!MDNS.begin(RIG_IDENTIFIER)) { }
      MDNS.addService("http", "tcp", 80);

      server.on("/", dashboard);
      server.on("/api", handleApi);
      
      server.on("/api/throttle", []() {
          isBootBoostActive = false; // Cancel auto-timer if manual override triggered
          String mode = server.arg("mode");
          #if defined(ESP8266)
              if(mode == "eco") { system_update_cpu_freq(80); os_update_cpu_frequency(80); }
              else { system_update_cpu_freq(160); os_update_cpu_frequency(160); }
          #elif defined(CONFIG_FREERTOS_UNICORE)
              if(mode == "eco") setCpuFrequencyMhz(80);
              else setCpuFrequencyMhz(160);
          #else
              if(mode == "eco") setCpuFrequencyMhz(80);
              else if(mode == "bal") setCpuFrequencyMhz(160);
              else setCpuFrequencyMhz(240);
          #endif
          server.send(200, "application/json", "{\"status\":\"success\", \"mode\":\"" + mode + "\"}");
      });

      server.on("/api/reboot", []() {
          server.send(200, "application/json", "{\"status\":\"REBOOT_INITIATED\"}");
          
          #if defined(ESP32) && CORE == 2
              if(Task1 != NULL) vTaskDelete(Task1);
              if(Task2 != NULL) vTaskDelete(Task2);
          #endif

          for(int i = 0; i < 6; i++) {
              digitalWrite(LED_BUILTIN, HIGH);
              delay(150);
              digitalWrite(LED_BUILTIN, LOW); 
              delay(150);
          }

          ESP.restart(); 
      });

      #if defined(CAPTIVE_PORTAL)
        server.on("/reset", reset_settings);
      #endif

      server.begin();
    #endif

    // START AUTO BOOT BOOST
    #if defined(ESP8266)
        system_update_cpu_freq(160);
        os_update_cpu_frequency(160);
        lwdtFeed();
    #else
        setCpuFrequencyMhz(240); // Boot at MAX
    #endif
    bootBoostStartTime = millis();

    job[0]->blink(BLINK_SETUP_COMPLETE);

    #if defined(ESP32) && CORE == 2
      mutexClientData = xSemaphoreCreateMutex();
      mutexConnectToServer = xSemaphoreCreateMutex();
      
      xTaskCreatePinnedToCore(system_events_func, "system_events_func", 10000, NULL, 1, NULL, 0);
      xTaskCreatePinnedToCore(task1_func, "task1_func", 10000, NULL, 1, &Task1, 0);
      xTaskCreatePinnedToCore(task2_func, "task2_func", 10000, NULL, 1, &Task2, 1);
    #endif
}

void system_events_func(void* parameter) {
  while (true) {
    delay(10);
    checkBootBoost();
    #if defined(WEB_DASHBOARD)
      server.handleClient();
    #endif
    ArduinoOTA.handle();
  }
}

void single_core_loop() {
    job[0]->mine();
    lwdtFeed();
    VerifyWifi();
    ArduinoOTA.handle();
    checkBootBoost();
    #if defined(WEB_DASHBOARD)
         server.handleClient();
    #endif
}

void loop() {
  #if defined(ESP8266) || defined(CONFIG_FREERTOS_UNICORE)
    single_core_loop();
  #endif
  delay(10);
}