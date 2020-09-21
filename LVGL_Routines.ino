
int mSelect = 0;

static void slider_event_cb(lv_obj_t * slider, lv_event_t event);
static lv_obj_t * slider_label;

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


uint8_t lv_ex_btnmatrix_1(void)
{
    static const char * btnm_map[] = {"Affichage", "Wifi", "\n",
                                     "Batterie", "\n",
                                     "Flappy Bird", "\n",
                                     "Quitter", ""};
                                  
    KeyPressed = false;
    lv_obj_t * btnm1 = lv_btnmatrix_create(lv_scr_act(), NULL);
    
    lv_btnmatrix_set_map(btnm1, btnm_map);
    lv_btnmatrix_set_btn_width(btnm1, 10, 2);        /*Make "Action1" twice as wide as "Action2"*/
    lv_btnmatrix_set_btn_ctrl(btnm1, 10, LV_BTNMATRIX_CTRL_CHECKABLE);
    lv_btnmatrix_set_btn_ctrl(btnm1, 11, LV_BTNMATRIX_CTRL_CHECK_STATE);
    lv_obj_set_size(btnm1, 240, 240);
    lv_obj_align(btnm1, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_event_cb(btnm1, menu_event_handler);
    
    while (!KeyPressed) {lv_task_handler(); delay(20);} // Wait for touch
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
    if(event == LV_EVENT_VALUE_CHANGED) {
        KeyPressed = true;
    }
}

void lv_ex_calendar_1(void)
{
    KeyPressed = false;

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

    while (!KeyPressed) {lv_task_handler(); delay(20);} // Wait for touch
    ttgo->tft->fillScreen(TFT_BLACK);
}


/************************************************************************************
 *                                Configuration affichage
 */

static void aff_event_handler(lv_obj_t * slider, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED) {
        TimeToWaitForLowEnergy = lv_slider_get_value(slider);
        lv_label_set_text_fmt(slider_label, "%d", TimeToWaitForLowEnergy);
    }
}

void config_affichage(){

    lv_obj_t * win = lv_win_create(lv_scr_act(), NULL);
    lv_win_set_title(win, "Configuration...");                        /*Set the title*/ 
    lv_obj_t * label;
    //static const char * btnm_map[] = {"Quitter", ""};
    
    ttgo->tft->fillScreen(TFT_BLACK);
    KeyPressed = false;

    lv_obj_t * txt = lv_label_create(win, NULL);
    lv_label_set_text(txt, "Configurer le temps\n"
                           "avant mise en veille.\n");
    
    // Create the slider
    lv_obj_t * slider = lv_slider_create(lv_scr_act(), NULL);
    lv_obj_set_width(slider, 200);
    lv_obj_align(slider, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_event_cb(slider, aff_event_handler);
    lv_slider_set_range(slider, 5, 60);
    lv_slider_set_value(slider, TimeToWaitForLowEnergy, 1);

    /* Create a label below the slider */
    slider_label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text_fmt(slider_label, "Temps : %d", TimeToWaitForLowEnergy);
    lv_obj_set_auto_realign(slider_label, true);
    lv_obj_align(slider_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    lv_obj_t * btn1 = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_event_cb(btn1, Quit_event_handler);
    lv_obj_align(btn1, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 40);
    label = lv_label_create(btn1, NULL);
    lv_label_set_text(label, "Quitter");

    while (!KeyPressed) {lv_task_handler(); delay(20);} // Wait for touch
    ttgo->tft->fillScreen(TFT_BLACK);
}


    
