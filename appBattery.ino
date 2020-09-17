// Display Battery Data

void appBattery() {
int16_t x, y;

  ttgo->tft->setTextFont(2);
  ttgo->tft->setTextColor(TFT_YELLOW, TFT_BLACK);
  ttgo->tft->drawString("BATT STATS",  35, 30, 2);
  ttgo->tft->setTextColor(TFT_GREEN, TFT_BLACK);

  // Turn on the battery adc to read the values
  //ttgo->power->adc1Enable(AXP202_BATT_VOL_ADC1 | AXP202_BATT_CUR_ADC1 | AXP202_VBUS_VOL_ADC1 | AXP202_VBUS_CUR_ADC1, AXP202_ON);
  ttgo->power->adc1Enable(AXP202_VBUS_VOL_ADC1 | AXP202_VBUS_CUR_ADC1 | AXP202_BATT_CUR_ADC1 | AXP202_BATT_VOL_ADC1, true);

  // Turn off unused power
  //ttgo->power->setPowerOutPut(AXP202_EXTEN, AXP202_OFF);
  //ttgo->power->setPowerOutPut(AXP202_DCDC2, AXP202_OFF);
  //ttgo->power->setPowerOutPut(AXP202_LDO3, AXP202_OFF);
  //ttgo->power->setPowerOutPut(AXP202_LDO4, AXP202_OFF);

// Print the values
  while (!ttgo->getTouch(x, y)) { // Wait for touch
     float vbus_v = ttgo->power->getVbusVoltage();
     float vbus_c = ttgo->power->getVbusCurrent();
     float batt_v = ttgo->power->getBattVoltage();
     int per = ttgo->power->getBattPercentage();
     per = ttgo->power->getBattPercentage();
     ttgo->tft->setCursor(0, 100);
     ttgo->tft->print("Vbus: "); ttgo->tft->print(vbus_v); ttgo->tft->println(" mV");
     ttgo->tft->setCursor(0, 130);
     ttgo->tft->print("Vbus: "); ttgo->tft->print(vbus_c); ttgo->tft->println(" mA");
     ttgo->tft->setCursor(0, 160);
     ttgo->tft->print("BATT: "); ttgo->tft->print(batt_v); ttgo->tft->println(" mV");
     ttgo->tft->setCursor(0, 190);
     ttgo->tft->print("Per: "); ttgo->tft->print(per); ttgo->tft->println(" %");
     delay(300);
  }

  //while (!ttgo->getTouch(x, y)) {} // Wait for touch
  while (ttgo->getTouch(x, y)) {}  // Wait for release to exit
  //Clear screen 
  ttgo->tft->fillScreen(TFT_BLACK);
}
