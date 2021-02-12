// Based on the work By DanGeiger
//
// Personalized by Paul BLOCK
// This is a French version
// Version 1.00  -  17 Septembre 2020


#include "config.h"
#include <WiFi.h>
#include "time.h"
#include <soc/rtc.h>
#include <HTTPClient.h>
#include <ArduinoOTA.h>

TTGOClass *ttgo;
BMA *sensor;

bool lenergy = false;                   // passe à 1 si en mode économie
bool irqSensor = false;                 // Avons nous reçu une IRQ d'un des capteurs ?
bool irqPower = true;                   // Avons nous une action sur l'alimentation ?
bool isDoubleClick = false;
bool KeyPressed = false;
bool Activite = false;                  // Le capteur a t-il détecté une actibvité ?
bool EnCharge = false;
bool DemandeOTA = false;                 // Faut-il attendre une mise à jour OTA ? 
int TimeToWait;
int maxScore=0;                         // Scrore de Flappy Bird 
int mSelect = 0;                        // Valeur de retour des les menus
int BattPer = 0;                        // etat de la batterie en % (de 0 à 100)
int displayTheme=2;
uint16_t osx = 120, osy = 120, omx = 120, omy = 120, ohx = 120, ohy = 120; // Saved H, M, S x & y coords - Utilisé pour l'horloge analogique
float sx = 0, sy = 1, mx = 1, my = 0, hx = -1, hy = 0;    // Saved H, M, S x & y multipliers - Utilisé pour l'horloge analogique
 
//Informations sur le réseau Wifi
char ssid[33];
char passwifi[65];
String VersionSoftware = "1.00.0000";
bool WIFIdefined = false;
bool WifiConnected = false;

// Gestion des alarmes
bool irqRTC = false;
bool Alarme1 = false;
uint8_t Alarme1Hour=0;
uint8_t Alarme1Min=0;

uint32_t targetTime = 0;                // for next 1 second display update
int      TimeToWaitForLowEnergy = 20;   // Durée préconfigurée d'attente avant passage en LowEnergy.
uint32_t WaitingForLowEnergy;           // Attente avant de passer en mode LowEnergy.
int      Background = 128;              // valeur du background

// Spécifique configuration RTC.
uint8_t hh, mm, ss, mmonth, dday, oldhh;// H, M, S variables
uint16_t yyear; // Year is 16 bit int

// Données à récupérer du site OpenWeatherMap -- Spécifique Météo.
int FreshOpenWeather=0;
int WaitingOpenWeather=0;
char OpenWeatherTemp[8];
char OpenWeatherHumidity[8];
char OpenWeatherPressure[8];
char OpenWeatherDescription[64];
char OpenWeatherIconID[10];
char OpenWeatherID[4];

HTTPClient http;

void setup()
{
    Serial.begin(115200);
    WaitingForLowEnergy = TimeToWaitForLowEnergy;
    setCpuFrequencyMhz(80);
    Serial.print("ESP Freq : "); Serial.println(getCpuFrequencyMhz());
    ttgo = TTGOClass::getWatch();
    ttgo->begin();
    Lecture_config();
    //appMeteo();
      
    ttgo->tft->setTextFont(1);
    ttgo->tft->fillScreen(TFT_BLACK);
    ttgo->tft->setTextColor(TFT_YELLOW, TFT_BLACK); // Note: the new fonts do not draw the background colour

    //Receive objects for easy writing
    sensor = ttgo->bma;

    //Check if the RTC clock matches, if not, use compile time
    ttgo->rtc->check();

    //Synchronize time to system time
    ttgo->rtc->disableAlarm();
    ttgo->rtc->syncToSystem();

    displayTime(true); // Our GUI to show the time
    ttgo->openBL(); // Turn on the backlight
    ttgo->lvgl_begin();

    pinMode (4, OUTPUT);  // Activer le buzzer
    quickBuzz (200);

    //Configure la gestion du background
    ledcSetup(0, 5000, 8);
    ledcAttachPin(12, 0);
        
/*********************************************************************************
 *                 Spécifique accéléromètre
 *                
 */
    Acfg cfg;
    cfg.odr = BMA4_OUTPUT_DATA_RATE_100HZ;
    cfg.range = BMA4_ACCEL_RANGE_2G;
    cfg.bandwidth = BMA4_ACCEL_NORMAL_AVG4;
    cfg.perf_mode = BMA4_CONTINUOUS_MODE;
    // Configure the BMA423 accelerometer
    sensor->accelConfig(cfg);
    sensor->enableAccel();
    pinMode(BMA423_INT1, INPUT);
    
    attachInterrupt(BMA423_INT1, [] {
       // Set interrupt to set irq value to 1
       irqSensor = 1;
    }, RISING); //It must be a rising edge
    
    // Enable BMA423 step count feature
    sensor->enableFeature(BMA423_STEP_CNTR, true);
    // Enable BMA423 isTilt feature
    sensor->enableFeature(BMA423_TILT, true);
    // Enable BMA423 isDoubleClick feature
    sensor->enableFeature(BMA423_WAKEUP, true);
    sensor->enableFeature(BMA423_ANY_MOTION, true);
    sensor->enableFeature(BMA423_ACTIVITY, true);
    // Turn on step interrupt
    //sensor->enableStepCountInterrupt();
    sensor->enableTiltInterrupt();
    // It corresponds to isDoubleClick interrupt
    sensor->enableWakeupInterrupt();
    // Activer le tracker d'activité
    sensor->enableActivityInterrupt();
    sensor->resetStepCounter();  

/*************************************************************************************************************
 *                                      Spécifique RTC
 */

   pinMode(RTC_INT, INPUT_PULLUP);
   attachInterrupt(RTC_INT, [] {irqRTC = true;} , FALLING);
   if (Alarme1) ttgo->rtc->enableAlarm();
   
/**************************************************************************************************************
 *                                       Spécifique gestion de l'alimentation
 */

    pinMode(AXP202_INT, INPUT_PULLUP);
    attachInterrupt(AXP202_INT, [] {
        irqPower = true;
    }, FALLING);
 
    ttgo->power->adc1Enable(AXP202_VBUS_VOL_ADC1 | AXP202_VBUS_CUR_ADC1 | AXP202_BATT_CUR_ADC1 | AXP202_BATT_VOL_ADC1, true);
    // Turn off unused power
    ttgo->power->setPowerOutPut(AXP202_EXTEN, AXP202_OFF);
    ttgo->power->setPowerOutPut(AXP202_DCDC2, AXP202_OFF);
    ttgo->power->setPowerOutPut(AXP202_LDO3, AXP202_OFF);
    ttgo->power->setPowerOutPut(AXP202_LDO4, AXP202_OFF);  
    ttgo->power->enableIRQ(AXP202_PEK_SHORTPRESS_IRQ | AXP202_VBUS_REMOVED_IRQ | AXP202_VBUS_CONNECT_IRQ | AXP202_CHARGING_IRQ, true);
    ttgo->power->clearIRQ();
}

/***************************************************************************************************************
 *                                                      LOOP
 */
void loop()
{
bool  rlst;
int16_t x, y;

    if (targetTime < millis()) {
        targetTime = millis() + 1000;
        if (!WaitingForLowEnergy) {
          low_energy();
        }
        else {
          displayTime(false); 
          WaitingForLowEnergy-=1;
        }
        if (hh==0 && mm==0) sensor->resetStepCounter();  // Réinitialise le compteur de pas à minuit.
    }

    if (!lenergy){
       if (ttgo->getTouch(x, y)) {
           while (ttgo->getTouch(x, y)) {} // wait for user to release
           Serial.print("x="); Serial.println(x);
           Serial.print("y="); Serial.println(y);
           if (x>0 && x<80 && y>178 && y<240 ) lv_ex_calendar_1();     // Afficher le calendrier si on clique sur la date
           else {
              switch (lv_ex_menuPrincipal()) { // Call modeMenu. The return is the desired app number
              case 0: // Zero is the clock, just exit the switch
                  switch (MenuDisplay()) {
                  case 0:
                      config_affichage();   
                      break;
                  case 1:
                      displayTheme=0;
                      break;
                  case 2:
                      displayTheme=1;
                      break;
                  case 3:
                      displayTheme=2;
                      break;    
                  }
                  Sauvegarde_config();
                  break;
              case 1:
                  switch (appWifi()) {
                  case 0:
                      if (WIFIdefined) {
                         GetWifiPassword("Mot de passe WIFI :");
                         Sauvegarde_config();
                      }
                      break;  
                  case 1:
                      Serial.println("Demande de synchro");
                      SynchroWifi(ssid, passwifi);
                      break;
                  case 2:
                      Serial.println("Demande de mise à jour");
                      MiseAJourWifi(ssid, passwifi);
                      break;    
                  case 3:
                      break;
                  }
                  break;
              case 2:
                  appBattery();
                  break;
              case 3:
                  appAlarme();
                  break;
              case 4:
                  switch (appJeux()) {
                  case 0:
                    FlappyBird();
                    break;  
                  case 1:
                    appTicTac();
                    break;
                  case 2:
                    break;  
                  }
                  break;
              case 5:
                  appCalculator();
                  break;
              case 6:
                  appMeteo();
                  break;
              case 7:
                  appNiveau();
                  break;    
              case 8:
                  break;
              }
           }
        WaitingForLowEnergy = TimeToWaitForLowEnergy;
        if (BattPer<15) WaitingForLowEnergy = 3;       // Si la batterie est faible, on arrête l'écran rapidement.
        displayTime(true);
        }
    }

/****************************************************************************************
 *                    Que faire en cas d'interruption ?
 */
   if (irqSensor) {                                // Interruption sur le capteur de mouvement
     //Serial.println("Réception IRQ");
     irqSensor = 0;
     Activite = false;
     do {
         // Read the BMA423 interrupt status,
         // need to wait for it to return to true before continuing
         rlst =  sensor->readInterrupt();
     } while (!rlst);

     if (sensor->isTilt()) {
        //Serial.println("Tilt");
        if (lenergy) normal_energy(); 
     }
     if (sensor->isDoubleClick()) {
        isDoubleClick = true;
        if (lenergy) normal_energy(); 
        //Serial.println("isDoubleClick");
     }
     if (sensor->isActivity()){
        Activite = true;
     }
     //delay(100);
  }

  if (irqPower) {                                // Interruption sur la gestion de l'alimentation
     irqPower = false;
     ttgo->power->readIRQ();

     if (ttgo->power->isVbusPlugInIRQ()) {
         EnCharge = true;
     }
     if (ttgo->power->isVbusRemoveIRQ()) {
         EnCharge = false;
     }
     if (ttgo->power->isPEKShortPressIRQ()) {
         //Serial.println("Power Press");
     }
     if (lenergy) normal_energy(); 
     //else low_energy();
     ttgo->power->clearIRQ();
     //delay(100);
  }
  
  if (irqRTC) {                                 // Interruption sur le RTC.
     irqRTC = false;
     if (lenergy) normal_energy(); 
     ttgo->rtc->resetAlarm();
     int i = 30;
     while (i-- && !irqPower) {
        quickBuzz(200);
        delay(500);
     }
     ttgo->rtc->disableAlarm();
     ttgo->rtc->setAlarm(Alarme1Hour, Alarme1Min, PCF8563_NO_ALARM, PCF8563_NO_ALARM);
     ttgo->rtc->enableAlarm();
  }
}
