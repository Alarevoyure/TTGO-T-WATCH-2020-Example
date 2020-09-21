#include "config.h"

PCF8563_Class *rtc;

const char *ssid            = "Ferme201";
const char *password        = "";

const char *ntpServer       = "pool.ntp.org";
const long  gmtOffset_sec   = 3600;
const int   daylightOffset_sec = 3600;

//bool rtcIrq = false;

void appWifi() {
int testWifi = 20;

    ttgo->tft->setTextColor(TFT_GREEN, TFT_BLACK);
    ttgo->tft->setTextFont(2);
    //connect to WiFi
    ttgo->tft->setCursor(0,0);
    ttgo->tft->print("Connecting to ");
    ttgo->tft->println(ssid);
    ttgo->tft->println();
    ttgo->tft->print("--> ");

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED && testWifi) {
        delay(500);
        Serial.print(testWifi);
        ttgo->tft->print(".");
        testWifi -= 1;
    }

    if (testWifi) {
      ttgo->tft->println("\nCONNECTED\n");

      delay(1000);
      //init and get the time
      configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

      struct tm timeinfo;
      if (!getLocalTime(&timeinfo)) {
          ttgo->tft->println("Failed to obtain time, Restart in 3 seconds");
          Serial.println("Failed to obtain time");
          delay(3000);
          //esp_restart();
          while (1);
      } 
      else {
         ttgo->tft->println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
         ttgo->tft->println("Time synchronization succeeded");
         // Sync local time to external RTC
         ttgo->rtc->syncToRtc();
      }
    }
    else ttgo->tft->println("\n ERROR\n");

    int16_t x, y;
    while (!ttgo->getTouch(x, y)) {} // Wait for touch
    while (ttgo->getTouch(x, y)) {}  // Wait for release to exit
    //Clear screen 
    ttgo->tft->fillScreen(TFT_BLACK);
}
