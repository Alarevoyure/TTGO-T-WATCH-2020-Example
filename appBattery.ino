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

    TimeToWait = millis() + 10000;   // 10 seconde maxi dans le menu, sinon, on quitte.
    KeyPressed = false;  
    ttgo->power->adc1Enable(AXP202_VBUS_VOL_ADC1 | AXP202_VBUS_CUR_ADC1 | AXP202_BATT_CUR_ADC1 | AXP202_BATT_VOL_ADC1, true);
    
    float vbus_v = ttgo->power->getVbusVoltage();
    float vbus_c = ttgo->power->getVbusCurrent();
    float batt_v = ttgo->power->getBattVoltage();
    BattPer = ttgo->power->getBattPercentage();
    
    static lv_color_t needle_colors[1];
    needle_colors[0] = LV_COLOR_BLUE;

    /*Create a gauge*/
    lv_obj_clean(lv_scr_act());                                   // Clear Screen
    lv_obj_t * gauge1 = lv_gauge_create(lv_scr_act(), NULL);
    lv_gauge_set_needle_count(gauge1, 1, needle_colors);
    lv_obj_set_size(gauge1, 200, 200);
    lv_gauge_set_critical_value(gauge1, 20);
    lv_obj_align(gauge1, NULL, LV_ALIGN_CENTER, 0, -9);
    lv_obj_set_event_cb(gauge1, batt_event_handler);

    //vbatt_label = lv_label_create(lv_scr_act(), NULL);
    //lv_label_set_text_fmt(vbatt_label, "Tension batterie : %d mV", int(batt_v));
    //lv_obj_set_auto_realign(vbatt_label, true);
    //lv_obj_align(vbatt_label, gauge1, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);

    /*Set the values*/
    lv_gauge_set_value(gauge1, 0, BattPer);

    while (!KeyPressed) {
      lv_task_handler(); 
      delay(20);
      if (TimeToWait < millis())KeyPressed = true;
    } 
    ttgo->tft->fillScreen(TFT_BLACK);
}
