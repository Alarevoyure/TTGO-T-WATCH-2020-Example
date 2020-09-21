// An Arduino based framework for the Lilygo T-Watch 2020
// Much of the code is based on the sample apps for the
// T-watch that were written and copyrighted by Lewis He.
//(Copyright (c) 2019 lewis he)
//
// Based on the work By DanGeiger
//
// Personalized by Paul BLOCK
// This is a French version
// Version 1.00  -  17 Septembre 2020


#include "config.h"
#include <WiFi.h>
#include "time.h"
#include <soc/rtc.h>

TTGOClass *ttgo;
BMA *sensor;

bool lenergy = false;                   // passe à 1 si en mode économie
bool irq = false;                       // Avons nous reçu une IRQ d'un des capteurs ?
bool isDoubleClick = false;
bool KeyPressed = false;
int maxScore=0;                         // Scrore de Flappy Bird 

uint32_t targetTime = 0;                // for next 1 second display update
int      TimeToWaitForLowEnergy = 20;   // Durée préconfigurée d'attente avant passage en LowEnergy.
uint32_t WaitingForLowEnergy;           // Attente avant de passer en mode LowEnergy.
// uint32_t clockUpTime = 0;            // track the time the clock is displayed

uint8_t hh, mm, ss, mmonth, dday; // H, M, S variables
uint16_t yyear; // Year is 16 bit int

void setup()
{
    Serial.begin(115200);
    WaitingForLowEnergy = TimeToWaitForLowEnergy;
    //initSetup();
    ttgo = TTGOClass::getWatch();
    ttgo->begin();
    
    ttgo->tft->setTextFont(1);
    ttgo->tft->fillScreen(TFT_BLACK);
    ttgo->tft->setTextColor(TFT_YELLOW, TFT_BLACK); // Note: the new fonts do not draw the background colour
    Lecture_config();                               // Lecture de la config sauvegardée en mémoire.

    //Receive objects for easy writing
    sensor = ttgo->bma;

    //Check if the RTC clock matches, if not, use compile time
    ttgo->rtc->check();

    //Synchronize time to system time
    ttgo->rtc->syncToSystem();

    displayTime(true); // Our GUI to show the time
    ttgo->openBL(); // Turn on the backlight
    ttgo->lvgl_begin();

    pinMode (4, OUTPUT);  // Activer le buzzer
    quickBuzz ();

    ttgo->power->adc1Enable(AXP202_VBUS_VOL_ADC1 | AXP202_VBUS_CUR_ADC1 | AXP202_BATT_CUR_ADC1 | AXP202_BATT_VOL_ADC1, true);
    
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
       irq = 1;
    }, RISING); //It must be a rising edge
    
    // Enable BMA423 step count feature
    sensor->enableFeature(BMA423_STEP_CNTR, true);
    // Enable BMA423 isTilt feature
    sensor->enableFeature(BMA423_TILT, true);
    // Enable BMA423 isDoubleClick feature
    sensor->enableFeature(BMA423_WAKEUP, true);
    // Turn on step interrupt
    sensor->enableStepCountInterrupt();
    sensor->enableTiltInterrupt();
    // It corresponds to isDoubleClick interrupt
    sensor->enableWakeupInterrupt();
   // sensor->resetStepCounter();  
}

void loop()
{
    if (targetTime < millis()) {
        targetTime = millis() + 1000;
        //displayTime(ss == 0); // Call every second but only update time every minute
        if (!WaitingForLowEnergy) {
          low_energy();
        }
        else {
          displayTime(false); // Call every second but only update all every minute
          WaitingForLowEnergy-=1;
        }
        if (hh==0 && mm==0) sensor->resetStepCounter();  // Réinitialise le compteur de pas à minuit.
    }

    if (!lenergy){
       int16_t x, y;
       if (ttgo->getTouch(x, y)) {
           while (ttgo->getTouch(x, y)) {} // wait for user to release
           Serial.print("x="); Serial.println(x);
           Serial.print("y="); Serial.println(y);
           if (x>0 && x<80 && y>178 && y<240 ) lv_ex_calendar_1();     // Afficher le calendrier si on clique sur la date
           else {
              switch (lv_ex_btnmatrix_1()) { // Call modeMenu. The return is the desired app number
              case 0: // Zero is the clock, just exit the switch
                  config_affichage();   // Dans LVGL_Routines
                  break;
              case 1:
                  appWifi(); 
                  break;
              case 2:
                  appBattery();
                  break;
              case 3:
                  FlappyBird();
                  break;
              case 4:
                  break;
              }
           }
        WaitingForLowEnergy = TimeToWaitForLowEnergy;
        displayTime(true);
        }
    }

/****************************************************************************************
 *                    Que faire en cas d'interruption ?
 */
   if (irq) {
     //Serial.println("Réception IRQ");
     irq = 0;
     bool  rlst;
     do {
         // Read the BMA423 interrupt status,
         // need to wait for it to return to true before continuing
         rlst =  sensor->readInterrupt();
     } while (!rlst);
     
     // Check if it is a step interrupt
     if (sensor->isStepCounter()) {
         
     }
     if (sensor->isTilt()) {
        normal_energy(); 
        //Serial.println("Tilt");
     }
     if (sensor->isDoubleClick()) {
        normal_energy(); 
        isDoubleClick = true;
        //Serial.println("isDoubleClick");
     }
     delay(200);
  }
}
