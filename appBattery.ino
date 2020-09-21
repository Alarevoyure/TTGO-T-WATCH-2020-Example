// Display Battery Data

static lv_obj_t * vbatt_label;

static void batt_event_handler(lv_obj_t * gauge1, lv_event_t event)
{
    if(event == LV_EVENT_PRESSED) {
      //  mSelect = lv_btnmatrix_get_active_btn(obj);
        KeyPressed = true;
        Serial.println("here we go");
    }
}

void appBattery() {
  
int16_t x, y;
/*
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
  */
      /*Describe the color for the needles*/
    KeyPressed = false;  
    ttgo->power->adc1Enable(AXP202_VBUS_VOL_ADC1 | AXP202_VBUS_CUR_ADC1 | AXP202_BATT_CUR_ADC1 | AXP202_BATT_VOL_ADC1, true);
    
    float vbus_v = ttgo->power->getVbusVoltage();
    float vbus_c = ttgo->power->getVbusCurrent();
    float batt_v = ttgo->power->getBattVoltage();
    int per = ttgo->power->getBattPercentage();
    per = ttgo->power->getBattPercentage();
    Serial.print("batt_v  :")   ;Serial.println(batt_v)   ;
    
    static lv_color_t needle_colors[3];
    needle_colors[0] = LV_COLOR_BLUE;
    needle_colors[1] = LV_COLOR_ORANGE;
    needle_colors[2] = LV_COLOR_PURPLE;

    /*Create a gauge*/
    lv_obj_clean(lv_scr_act());                                   // Clear Screen
    lv_obj_t * gauge1 = lv_gauge_create(lv_scr_act(), NULL);
    lv_gauge_set_needle_count(gauge1, 1, needle_colors);
    lv_obj_set_size(gauge1, 200, 200);
    lv_gauge_set_critical_value(gauge1, 20);
    lv_obj_align(gauge1, NULL, LV_ALIGN_CENTER, 0, -9);
    lv_obj_set_event_cb(gauge1, batt_event_handler);

    vbatt_label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text_fmt(vbatt_label, "Tension batterie : %d mV", int(batt_v));
    lv_obj_set_auto_realign(vbatt_label, true);
    lv_obj_align(vbatt_label, gauge1, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    /*Set the values*/
    lv_gauge_set_value(gauge1, 0, per);
   // lv_gauge_set_value(gauge1, 1, 20);
   // lv_gauge_set_value(gauge1, 2, 30);

    while (!KeyPressed) {lv_task_handler(); delay(20);} // Wait for touch
    ttgo->tft->fillScreen(TFT_BLACK);
}
