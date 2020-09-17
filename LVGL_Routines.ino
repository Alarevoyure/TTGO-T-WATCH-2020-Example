
bool KeyPressed = false;
int mSelect = 0;

/************************************************************************************
 *                                Affichage Menu
 */
static void menu_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED) {
        mSelect = lv_btnmatrix_get_active_btn(obj);
        printf("Clicked on : %02d \n", mSelect);
        KeyPressed = true;
    }
}


static const char * btnm_map[] = {"Heure", "Wifi", "\n",
                                  "Batterie", "\n",
                                  "Flappy Bird", "\n",
                                  "Quitter", ""};

uint8_t lv_ex_btnmatrix_1(void)
{
    KeyPressed = false;
//   ttgo->lvgl_begin();
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
const char * day_names[7] = {"Lun", "Ma", "Me", "Jeu", "Ve", "Sa", "Dim"};
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
