#include <EEPROM.h>
#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson

// this will display 80% when percentage is between 70% and 90%
// fully charged battery never exceeds 98% as far as I can tell.
#define BATT_ICON_WIDTH 48
#define BATT_ICON_HEIGHT 18
#define BHOVER3 (BATT_ICON_HEIGHT/3)
#define BWOVER5 ((BATT_ICON_WIDTH/5)-1)
#define BCHGT (BATT_ICON_HEIGHT - 6)
/*
 *           Gestion du buzzer
 */
void quickBuzz (int Wait) {
  digitalWrite (4, HIGH);
  delay (Wait);
  digitalWrite (4, LOW);
}

/*
 *           Gestion de l'énergie, passage du mode basse à haute énergie.
 */

void low_energy()
{ 
  if (!lenergy){
    ttgo->closeBL();
    // ttgo->stopLvglTick();
    //ttgo->bma->enableStepCountInterrupt(false);
    setCpuFrequencyMhz(80);
    ttgo->displaySleep();
    
    lenergy = true;
    if (displayTheme!=2) {        //Si thème 2, on garde le Wifi allumé.
      WiFi.mode(WIFI_OFF);
      WifiConnected = false;
    }
    if (oldhh!=hh) {
      oldhh=hh;
      FreshOpenWeather=false;
    }
    //Serial.println("ENTER IN LIGHT SLEEEP MODE");
    gpio_wakeup_enable ((gpio_num_t)AXP202_INT, GPIO_INTR_LOW_LEVEL);
    gpio_wakeup_enable ((gpio_num_t)RTC_INT, GPIO_INTR_LOW_LEVEL);
    gpio_wakeup_enable ((gpio_num_t)BMA423_INT1, GPIO_INTR_HIGH_LEVEL);
    //ttgo->power->adc1Enable(AXP202_VBUS_VOL_ADC1 | AXP202_VBUS_CUR_ADC1 | AXP202_BATT_CUR_ADC1 | AXP202_BATT_VOL_ADC1, false); 
    esp_sleep_enable_gpio_wakeup ();
    esp_light_sleep_start();
  }
}
    
void normal_energy()  {
    if (BattPer<20) {
      Background = 5;                            // Si la batterie est faible on force l'éclairage à 5 (sur 255).
      WaitingForLowEnergy = 3;                   // et on limite le temps d'affichage.
    } else setCpuFrequencyMhz(160);              // sinon, on augmente la fréquence. 
    
    //ttgo->power->adc1Enable(AXP202_VBUS_VOL_ADC1 | AXP202_VBUS_CUR_ADC1 | AXP202_BATT_CUR_ADC1 | AXP202_BATT_VOL_ADC1, true);
    //ttgo->startLvglTick();                       // Redémarrer le superviseur LVGL
    //Serial.println("ENTER IN NORMAL MODE");
    WaitingForLowEnergy = TimeToWaitForLowEnergy;
    ttgo->displayWakeup();
    ttgo->rtc->syncToSystem();
    ttgo->openBL();
    //ttgo->bma->enableStepCountInterrupt();
    lenergy = false;
    displayTime(true);
    //WiFi.mode(WIFI_ON);
}

/*
 *         Gestion des paramètres à sauvegarder.
 */

void Sauvegarde_config(){
   EEPROM.begin(512); 
   EEPROM.write(1,maxScore);                    // Emplacement 1 = score max de Flappy Bird
   EEPROM.write(3,TimeToWaitForLowEnergy);      // Emplacement 3 = temps d'affichage avant mise en veille
   EEPROM.write(5,WIFIdefined);
   EEPROM.put(21, ssid);
   EEPROM.put(54, passwifi);
   EEPROM.write(7, Alarme1);
   EEPROM.write(9, Alarme1Hour);
   EEPROM.write(11, Alarme1Min);
   EEPROM.write(13, displayTheme);
   if (BattPer>20) EEPROM.write(15, Background);
  
   EEPROM.put(130, VersionSoftware);
   EEPROM.commit();
   EEPROM.end();
/*  
   Serial.println("------------------ Sauvegarde des données en mémoire -----------------");
   Serial.print("Temps d'attente : "); Serial.println(TimeToWaitForLowEnergy);
   Serial.print("Score Flappy    : "); Serial.println(maxScore);
   Serial.print("SSID            : "); Serial.println(ssid);
   Serial.print("Pass Wifi       : "); Serial.println(passwifi);
   Serial.print("Définition Wifi : "); Serial.println(WIFIdefined);
   Serial.print("Mac Adresse     : "); Serial.println(WiFi.macAddress());
*/
}

void Lecture_config(){
  
    EEPROM.begin(512); 
    maxScore = EEPROM.read(1);
    if (maxScore == 255) {
      maxScore = 0;
      EEPROM.write(1,maxScore);
      TimeToWaitForLowEnergy = 20;
      EEPROM.write(3,TimeToWaitForLowEnergy);
      EEPROM.write(5,WIFIdefined);
      EEPROM.write(7, Alarme1);
      EEPROM.write(9, Alarme1Hour);
      EEPROM.write(11, Alarme1Min);
    }
    else {
      TimeToWaitForLowEnergy = EEPROM.read(3);
      WIFIdefined = EEPROM.read(5);
      Alarme1 = EEPROM.read(7);
      Alarme1Hour = EEPROM.read(9);
      Alarme1Min = EEPROM.read(11);
      displayTheme = EEPROM.read(13);
      Background = EEPROM.read(15);
      EEPROM.get(21, ssid);
      EEPROM.get(54, passwifi);
      EEPROM.get(130, VersionSoftware);
    }
    EEPROM.commit();
    EEPROM.end();
/*    Serial.println("------------------ Lecture données en mémoire -----------------");
    Serial.print("Temps d'attente : "); Serial.println(TimeToWaitForLowEnergy);
    Serial.print("Score Flappy    : "); Serial.println(maxScore);
    Serial.print("Pass Wifi       : "); Serial.println(passwifi);
    Serial.print("SSID            : "); Serial.println(ssid);
    Serial.print("Définition Wifi : "); Serial.println(WIFIdefined);
    Serial.print("Mac Adresse     : "); Serial.println(WiFi.macAddress());
    Serial.print("Version Software: "); Serial.println(VersionSoftware);
*/    
    if (Alarme1) {
      ttgo->rtc->disableAlarm();
      ttgo->rtc->setAlarm(Alarme1Hour, Alarme1Min, PCF8563_NO_ALARM, PCF8563_NO_ALARM);
      ttgo->rtc->enableAlarm();
    }
}

void battery_icon(uint16_t ox, uint16_t oy, uint16_t fgc, uint16_t bgc){
int pp10;
   BattPer = ttgo->power->getBattPercentage();
   pp10 = BattPer + 10 ;
   ttgo->tft->fillRect(ox, oy, BATT_ICON_WIDTH, BATT_ICON_HEIGHT, bgc);
   ttgo->tft->drawRect(ox, oy, BATT_ICON_WIDTH - 3, BATT_ICON_HEIGHT, fgc);
   ttgo->tft->fillRect(ox + BATT_ICON_WIDTH - 3, oy + BHOVER3, 3, BHOVER3, fgc);
   for(int i = 0 ; i < 5 ; i++) {
     if(((i+1) * 20) < pp10) {
       ttgo->tft->fillRect(4 + ox + (BWOVER5 * i), oy + 3, BWOVER5-2, BCHGT, fgc);
     }
   }
}

void charge_icon(uint16_t ox, uint16_t oy, uint16_t fgcolor, uint16_t bgcolor){
uint16_t li = ox + 8;
uint16_t ri = ox + 16;
  if (EnCharge) {
    ttgo->tft->drawLine(li, oy + 15, ri, oy +  9, fgcolor);
    ttgo->tft->drawLine(li, oy + 14, ri, oy +  8, fgcolor);
    ttgo->tft->drawLine(li, oy +  9, ri, oy +  9, fgcolor);
    ttgo->tft->drawLine(li, oy +  8, ri, oy +  8, fgcolor);
    ttgo->tft->drawLine(li, oy +  9, ri, oy +  4, fgcolor);
    ttgo->tft->drawLine(li, oy +  8, ri, oy +  3, fgcolor);
  } else {
    ttgo->tft->drawLine(li, oy + 15, ri, oy +  9, bgcolor);
    ttgo->tft->drawLine(li, oy + 14, ri, oy +  8, bgcolor);
    ttgo->tft->drawLine(li, oy +  9, ri, oy +  9, bgcolor);
    ttgo->tft->drawLine(li, oy +  8, ri, oy +  8, bgcolor);
    ttgo->tft->drawLine(li, oy +  9, ri, oy +  4, bgcolor);
    ttgo->tft->drawLine(li, oy +  8, ri, oy +  3, bgcolor);
  }
}


void steps_icon (uint16_t ox, uint16_t oy, uint16_t fgcolor, uint16_t bgcolor, uint8_t mult) {
  if(mult < 1) { mult = 1; }
  else if(mult > 4) { mult = 4; }
  // Serial.printf("ox = %d, oy = %d, mult = %d\n", ox, oy, mult);
  ttgo->tft->fillRect(ox, oy, mult * 22, mult * 20, bgcolor);
  // fillRoundRect(x, y, w, h, radius, color)
  // left foot
  ttgo->tft->fillCircle(ox + (6 * mult), oy + (14 * mult), 1 * mult, fgcolor);//heel
  ttgo->tft->fillRoundRect(ox + (4 * mult), oy + (6 * mult), 4 * mult, 6 * mult, 2 * mult, fgcolor);
  // right foot
  ttgo->tft->fillCircle(ox + (13 * mult), oy + (11 * mult), 1 * mult, fgcolor);//heel
  ttgo->tft->fillRoundRect(ox + (11 * mult), oy + (3 * mult), 4 * mult, 6 * mult, 2 * mult, fgcolor);
}


void AffRSSI (uint16_t ox, uint16_t oy, uint16_t fgcolor, uint16_t bgcolor)        // Affiche la force du signal WIFI
{
  int SignalRSSI = WiFi.RSSI();
  if (SignalRSSI==0) {
    SignalRSSI=-100;
    WifiConnected=false;
  }
  else {
    WifiConnected=true;
    ttgo->tft->fillRect(ox, oy+14, 3, 4, bgcolor);
    ttgo->tft->fillRect(ox+4, oy+10, 3, 8, bgcolor);
    ttgo->tft->fillRect(ox+8, oy+6, 3, 12, bgcolor);
    ttgo->tft->fillRect(ox+12, oy+2, 3, 16, bgcolor);
    if (SignalRSSI > -85) ttgo->tft->fillRect(ox, oy+14, 3, 4, fgcolor);
    if (SignalRSSI > -80) ttgo->tft->fillRect(ox+4, oy+10, 3, 8, fgcolor);
    if (SignalRSSI > -70) ttgo->tft->fillRect(ox+8, oy+6, 3, 12, fgcolor);
    if (SignalRSSI > -66) ttgo->tft->fillRect(ox+12, oy+2, 3, 16, fgcolor);
  }
}

// Affiche date et mois
void AffDate (uint16_t mx, uint16_t my, uint16_t dx, uint16_t dy, uint16_t fgcolor, uint16_t bgcolor, uint8_t mmonth, uint8_t dday) {        
String mStr;

    // Draw Month
  switch (mmonth) {
    case 0: mStr = ""; break;
    case 1: mStr = "Jan"; break;
    case 2: mStr = "Fev"; break;
    case 3: mStr = "Mar"; break;
    case 4: mStr = "Avr"; break;
    case 5: mStr = "Mai"; break;
    case 6: mStr = "Jui"; break;
    case 7: mStr = "Jul"; break;
    case 8: mStr = "Aou"; break;
    case 9: mStr = "Sep"; break;
    case 10: mStr = "Oct"; break;
    case 11: mStr = "Nov"; break;
    case 12: mStr = "Dec"; break;
  }
  ttgo->tft->setTextColor(fgcolor, bgcolor);
  ttgo->tft->drawString(mStr, mx, my, 2);
  if (dday) ttgo->tft->drawString(String(dday), dx, dy, 4);
}

void draw_keyboard (uint8_t num_keys, const char **b_labels, uint8_t font, bool leave_room_for_label, char *top_label) {
uint8_t yvals[4], yh, row, col;
uint16_t icolor;
  ttgo->tft->fillScreen(TFT_BLACK);
  if(leave_room_for_label) {
    yvals[0] = 35;
    yvals[1] = 85;
    yvals[2] = 135;
    yvals[3] = 185;
    yh = 50;
    if(top_label) {
      ttgo->tft->setTextColor(TFT_GREEN);
      ttgo->tft->drawCentreString( top_label, 120, 5, font);
    }
  }
  else {
    yvals[0] =   0;
    yvals[1] =  60;
    yvals[2] = 120;
    yvals[3] = 180;
    yh = 60;
  }
  int16_t xvals[4];
  int16_t xtvals[4];
  xvals[0] = 0;
  if(num_keys == 12) {
    for(int i = 0 ; i < 3 ; i++) {
      xvals[i]  = i * (246 / 3);
      xtvals[i] = xvals[i] + (246 / 6);
    }
  }
  else {
    for(int i = 0 ; i < 4 ; i++) {
      xvals[i]  = i * (248 / 4);
      xtvals[i] = xvals[i] + (248 / 8);
    }
  }
  // note: space at the top do display what is typed
  // was ttgo->tft->fillRect(0, 35, 80, 50, TFT_BLUE);
  // number keys are 80 x 50, four rows of three
  // x=0, 81, 161, y=35, 85, 135, 185
  ttgo->tft->setTextColor(TFT_GREEN);
  for(row = 0 ; row < 4 ; row++) {
    for(col = 0 ; col < (num_keys / 4) ; col++) {
      int ino = col + (row * (num_keys / 4));
      icolor = (!strcmp(b_labels[ino], "CANCEL") || !strcmp(b_labels[ino], "DONE")) ? TFT_DARKGREY : TFT_BLUE ;
      ttgo->tft->fillRoundRect(xvals[col], yvals[row],
  (num_keys == 12) ? 75 : 55, yh-5, 6, icolor);
      ttgo->tft->drawCentreString(b_labels[ino], xtvals[col], yvals[row]+5, font);
    }
  }
}


void GetMeteoInfo() {
  String URLOpenWeather;
  String payload;
  int httpCode;
  const size_t bufferSize = 2048;  // Buffer pour le traitement JSON des données OpenWeatherMap
 
   URLOpenWeather = "http://api.openweathermap.org/data/2.5/weather?q=PARIS,FR&APPID=xxxxxxxxxxxxxxxxxxxxxxxxxxxxx&units=metric";
   http.begin(URLOpenWeather);
   httpCode = http.GET();
   
   // httpCode will be negative on error
   if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      // file found at server
      if (httpCode == HTTP_CODE_OK) {
         payload = http.getString();
         DynamicJsonBuffer jsonBuffer(bufferSize);
         JsonObject& root = jsonBuffer.parseObject(payload);
 
         if (!root.success()) {
             Serial.println("JSON parsing failed!");
         } else {
            // Here were copy the strings we're interested in using to your struct data
            strcpy(OpenWeatherTemp, root["main"]["temp"]);
            strcpy(OpenWeatherHumidity, root["main"]["humidity"]);
            strcpy(OpenWeatherPressure, root["main"]["pressure"]);
            strcpy(OpenWeatherDescription, root["weather"][0]["description"]);
            strcpy(OpenWeatherIconID, root["weather"][0]["icon"]);
            strcpy(OpenWeatherID, root["weather"][0]["id"]);
            FreshOpenWeather = 1;
         }
      }
   } else {
      Serial.printf("[HTTP] GET... failed");
      FreshOpenWeather = 0;
   }
   http.end();
}
