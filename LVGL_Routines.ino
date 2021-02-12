
//int mSelect = 0;

static void slider_event_cb(lv_obj_t * slider, lv_event_t event);
static lv_obj_t * slider_label;
//static lv_obj_t * kb; 

/************************************************************************************
 *                              Routines communes
 */
static void Quit_event_handler(lv_obj_t * slider, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) {
         KeyPressed = true;
         Sauvegarde_config();
    }
}

/************************************************************************************
 *                                Affichage Menu
 */
static void menu_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED) {
        mSelect = lv_btnmatrix_get_active_btn(obj);
        KeyPressed = true;
    }
}


uint8_t lv_ex_menuPrincipal(void)
{
    TimeToWait = millis() + 10000;   // 10 seconde maxi dans le menu, sinon, on quitte.
    
    static const char * btnm_map[] = {"Affichage", "Wifi", "\n",
                                     "Batterie", "Alarme", "\n",
                                     "Jeux", "Calc", "Météo", "\n",
                                     "Niveau", "Quitter", ""};
                                  
    KeyPressed = false;
    lv_obj_t * btnm1 = lv_btnmatrix_create(lv_scr_act(), NULL);
    
    lv_btnmatrix_set_map(btnm1, btnm_map);
    lv_btnmatrix_set_btn_width(btnm1, 10, 2);        /*Make "Action1" twice as wide as "Action2"*/
    lv_btnmatrix_set_btn_ctrl(btnm1, 10, LV_BTNMATRIX_CTRL_CHECKABLE);
    lv_btnmatrix_set_btn_ctrl(btnm1, 11, LV_BTNMATRIX_CTRL_CHECK_STATE);
    lv_obj_set_size(btnm1, 240, 240);
    lv_obj_align(btnm1, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_event_cb(btnm1, menu_event_handler);
    
    while (!KeyPressed) {
      lv_task_handler(); 
      delay(20);
      if (TimeToWait < millis()) {mSelect=99; KeyPressed = true;}
    } 
    ttgo->tft->fillScreen(TFT_BLACK);
    return mSelect;
}

/************************************************************************************
 *                                Affichage du calendrier
 */
const char * day_names[7] = {"Di", "Lu", "Ma", "Me", "Je", "Ve", "Sa"};
const char * month_names[12] = {"Janvier", "Fevrier", "Mars", "Avril", "Mai", "Juin", "Juillet", "Aout", "Septembre", "Octobre","Novembre","Decembre"};

static void calendar_event_handler(lv_obj_t * obj, lv_event_t event)
{
    TimeToWait = millis() + 5000; 
    if(event == LV_EVENT_VALUE_CHANGED) {
        KeyPressed = true;
    }
}

void lv_ex_calendar_1(void)
{
    KeyPressed = false;
    TimeToWait = millis() + 10000;   // 10 seconde maxi dans le menu, sinon, on quitte.

    lv_obj_t  * calendar = lv_calendar_create(lv_scr_act(), NULL);
    
    lv_calendar_set_day_names(calendar, day_names);
    lv_calendar_set_month_names(calendar, month_names);
    lv_obj_set_size(calendar, 240, 240);
    lv_obj_align(calendar, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_event_cb(calendar, calendar_event_handler);

    lv_obj_set_style_local_text_font(calendar, LV_CALENDAR_PART_DATE, LV_STATE_DEFAULT, lv_theme_get_font_small());

    /*Set today's date*/
    RTC_Date tnow = ttgo->rtc->getDateTime();
    
    lv_calendar_date_t today;
    today.year = tnow.year;
    today.month = tnow.month;
    today.day = tnow.day;

    lv_calendar_set_today_date(calendar, &today);
    lv_calendar_set_showed_date(calendar, &today);

    while (!KeyPressed) {
      lv_task_handler(); 
      delay(20);
      if (TimeToWait < millis())KeyPressed = true;
    } 
    ttgo->tft->fillScreen(TFT_BLACK);
}


/************************************************************************************
 *                                Configuration affichage
 */

static void aff_event_handler(lv_obj_t * slider, lv_event_t event)
{
    TimeToWait = millis() + 5000;  
    if(event == LV_EVENT_VALUE_CHANGED) {
        TimeToWaitForLowEnergy = lv_slider_get_value(slider);
        lv_label_set_text_fmt(slider_label, "Temps : %d", TimeToWaitForLowEnergy);
    }
}

static void bck_event_handler(lv_obj_t * slider, lv_event_t event)
{
    TimeToWait = millis() + 5000;  
    if(event == LV_EVENT_VALUE_CHANGED) {
        Background = lv_slider_get_value(slider);
        //lv_label_set_text_fmt(slider_label, "%d", TimeToWaitForLowEnergy);
        ledcWrite(0, Background);
    }
}

void config_affichage(){

    TimeToWait = millis() + 10000;   // 10 seconde maxi dans le menu, sinon, on quitte.

    lv_obj_t * win = lv_win_create(lv_scr_act(), NULL);
    lv_win_set_title(win, "Configuration...");                        /*Set the title*/ 
    lv_obj_t * label;
    //static const char * btnm_map[] = {"Quitter", ""};
    
    ttgo->tft->fillScreen(TFT_BLACK);
    KeyPressed = false;

    lv_obj_t * txt = lv_label_create(win, NULL);
    lv_label_set_text(txt, "Mise en veille - background");
    
    // Create the slider for maiting time
    lv_obj_t * slider = lv_slider_create(lv_scr_act(), NULL);
    lv_obj_set_width(slider, 200);
    lv_obj_align(slider, txt, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
    lv_obj_set_event_cb(slider, aff_event_handler);
    lv_slider_set_range(slider, 5, 25);  //  5 à 25
    lv_slider_set_value(slider, TimeToWaitForLowEnergy, 1);

    // Create the slider for background
     lv_obj_t * slider1 = lv_slider_create(lv_scr_act(), NULL);
    lv_obj_set_width(slider1, 200);
    lv_obj_align(slider1, NULL, LV_ALIGN_CENTER, 0, 40);
    lv_obj_set_event_cb(slider1, bck_event_handler);
    lv_slider_set_range(slider1, 5, 250);  //  2 à 250
    lv_slider_set_value(slider1, Background, 1);

    /* Create a label below the slider */
    slider_label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text_fmt(slider_label, "Temps : %d", TimeToWaitForLowEnergy);
    lv_obj_set_auto_realign(slider_label, true);
    lv_obj_align(slider_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 8);

    lv_obj_t * btn1 = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_event_cb(btn1, Quit_event_handler);
    lv_obj_align(btn1, slider1, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
    label = lv_label_create(btn1, NULL);
    lv_label_set_text(label, "Quitter");

    while (!KeyPressed) {
      lv_task_handler(); 
      delay(20);
      if (TimeToWait < millis())KeyPressed = true;
    } 
    ttgo->tft->fillScreen(TFT_BLACK);
}

/************************************************************************************
 *                                Afficher une zone de saisie
 *                                Et retourner la valeur
 */


static void keyboard_event_cb(lv_obj_t * keyboard, lv_event_t event)
{
    (void) keyboard;    /*Unused*/
    TimeToWait = millis() + 30000;  
    
    lv_keyboard_def_event_cb (keyboard, event); // default keyboard / generic event handling callback functions, reference Function Description
 
    if(event == LV_EVENT_APPLY || event == LV_EVENT_CANCEL) {
       KeyPressed = true;
       // Serial.println(TexteSaisi);
    }
}

static void kb_event_handler(lv_obj_t * obj, lv_event_t event)
{
  const char * TexteSaisi;
  
    TimeToWait = millis() + 30000;  
    if(event == LV_EVENT_VALUE_CHANGED) {
       TexteSaisi = lv_textarea_get_text(obj);
       strcpy(passwifi, TexteSaisi);
    }
}

void GetWifiPassword (char * Message) {
  
    KeyPressed = false;
    TimeToWait = millis() + 30000;   // 30 seconde maxi dans la saisie de texte, sinon, on quitte.
    strcpy(passwifi, "");
    
    /*Create a keyboard and apply the styles*/
    lv_obj_clean(lv_scr_act());                                   // Clear Screen
    lv_obj_t * win = lv_win_create(lv_scr_act(), NULL);
    lv_win_set_title(win, "Saisie de texte");
           
    lv_obj_t *kb = lv_keyboard_create(lv_scr_act(), NULL);
    //kb = lv_keyboard_create(lv_scr_act(), NULL);
    lv_obj_set_event_cb(kb, keyboard_event_cb);
    lv_keyboard_set_cursor_manage(kb, true);

    /*Create a text area. The keyboard will write here*/
    lv_obj_t *ta = lv_textarea_create(lv_scr_act(), NULL);
    lv_textarea_set_text(ta, "");
    lv_textarea_set_one_line(ta, true);
    lv_obj_set_pos(ta, 5, 20);
    lv_obj_set_event_cb(ta, kb_event_handler);

    /* Create a label and position it above the text box */
    lv_obj_t * label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(label, Message);
    lv_obj_align(label, ta, LV_ALIGN_OUT_TOP_LEFT, 0, 0);

    /*Assign the text area to the keyboard*/
    lv_keyboard_set_textarea(kb, ta);
    
    while (!KeyPressed) {
      lv_task_handler(); 
      delay(20);
      if (TimeToWait < millis())KeyPressed = true;
    } 
    ttgo->tft->fillScreen(TFT_BLACK);

}
    
