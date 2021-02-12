

static void jeux_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED) {
        mSelect = lv_btnmatrix_get_active_btn(obj);
        KeyPressed = true;
    }
}

uint8_t appJeux(void)
{
    TimeToWait = millis() + 10000;   // 10 seconde maxi dans le menu, sinon, on quitte.
    
    static const char * btnm_map[] = {"Flappy Bird", "\n",
                                      "Tic Tac Too", "\n",
                                      "Quitter", ""};
                                  
    KeyPressed = false;
    lv_obj_t * btnm1 = lv_btnmatrix_create(lv_scr_act(), NULL);
    
    lv_btnmatrix_set_map(btnm1, btnm_map);
    lv_btnmatrix_set_btn_width(btnm1, 10, 2);        
    lv_btnmatrix_set_btn_ctrl(btnm1, 10, LV_BTNMATRIX_CTRL_CHECKABLE);
    lv_btnmatrix_set_btn_ctrl(btnm1, 11, LV_BTNMATRIX_CTRL_CHECK_STATE);
    lv_obj_set_size(btnm1, 240, 240);
    lv_obj_align(btnm1, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_event_cb(btnm1, jeux_event_handler);
    
    while (!KeyPressed) {
      lv_task_handler(); 
      delay(20);
      if (TimeToWait < millis()) {mSelect=99; KeyPressed = true;}
    } 
    ttgo->tft->fillScreen(TFT_BLACK);
    return mSelect;
}
