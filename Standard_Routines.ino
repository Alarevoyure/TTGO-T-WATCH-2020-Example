#include <EEPROM.h>
/*
 *           Gestion du buzzer
 */
void quickBuzz () {
  digitalWrite (4, HIGH);
  delay (200);
  digitalWrite (4, LOW);
}

/*
 *           Gestion de l'énergie, passage du mode basse à haute énergie.
 */

void low_energy()
{
    if (ttgo->bl->isOn()) {
       // xEventGroupSetBits(isr_group, WATCH_FLAG_SLEEP_MODE);
        ttgo->closeBL();
  //      ttgo->stopLvglTick();
        ttgo->bma->enableStepCountInterrupt(false);
        ttgo->displaySleep();
  //      if (!WiFi.isConnected()) {
            lenergy = true;
            WiFi.mode(WIFI_OFF);
            // rtc_clk_cpu_freq_set(RTC_CPU_FREQ_2M);
            setCpuFrequencyMhz(20);
            Serial.println("ENTER IN LIGHT SLEEEP MODE");
            gpio_wakeup_enable ((gpio_num_t)AXP202_INT, GPIO_INTR_LOW_LEVEL);
            gpio_wakeup_enable ((gpio_num_t)BMA423_INT1, GPIO_INTR_HIGH_LEVEL);
            esp_sleep_enable_gpio_wakeup ();
            esp_light_sleep_start();
 //      }
    } 
    ttgo->power->adc1Enable(AXP202_VBUS_VOL_ADC1 | AXP202_VBUS_CUR_ADC1 | AXP202_BATT_CUR_ADC1 | AXP202_BATT_VOL_ADC1, false);
}
    
void normal_energy()  {
   //     ttgo->startLvglTick();
   Serial.println("ENTER IN NORMAL MODE");
   WaitingForLowEnergy = TimeToWaitForLowEnergy;
   ttgo->displayWakeup();
   ttgo->rtc->syncToSystem();
    ttgo->openBL();
    ttgo->bma->enableStepCountInterrupt();
    setCpuFrequencyMhz(160);
    lenergy = false;
    ttgo->power->adc1Enable(AXP202_VBUS_VOL_ADC1 | AXP202_VBUS_CUR_ADC1 | AXP202_BATT_CUR_ADC1 | AXP202_BATT_VOL_ADC1, true);
    displayTime(true);
 }

/*
 *         Gestion des paramètres à sauvegarder.
 */

void Sauvegarde_config(){
  EEPROM.begin(512); 
  EEPROM.write(1,maxScore);                    // Emplacement 1 = score max de Flappy Bird
  EEPROM.write(3,TimeToWaitForLowEnergy);      // Emplacement 3 = temps d'affichage avant mise en veille
  EEPROM.commit();
  EEPROM.end();
}

void Lecture_config(){
    EEPROM.begin(512); 
    maxScore = EEPROM.read(1);
    if (maxScore == 255) {
      maxScore = 0;
      EEPROM.write(1,maxScore);
    }
    TimeToWaitForLowEnergy = EEPROM.read(3);
    if (TimeToWaitForLowEnergy == 255) {
      TimeToWaitForLowEnergy = 20;
      EEPROM.write(3,TimeToWaitForLowEnergy);
    }
    EEPROM.commit();
    EEPROM.end();
}
 
