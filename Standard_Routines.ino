
void quickBuzz () {
  digitalWrite (4, HIGH);
  delay (200);
  digitalWrite (4, LOW);
}


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
   //     updateStepCounter(ttgo->bma->getCounter());
   //     updateBatteryLevel();
   //     updateBatteryIcon(LV_ICON_CALCULATION);
   //     lv_disp_trig_activity(NULL);
    ttgo->openBL();
    ttgo->bma->enableStepCountInterrupt();
    setCpuFrequencyMhz(160);
    lenergy = false;
    ttgo->power->adc1Enable(AXP202_VBUS_VOL_ADC1 | AXP202_VBUS_CUR_ADC1 | AXP202_BATT_CUR_ADC1 | AXP202_BATT_VOL_ADC1, true);
    displayTime(true);
 }
