#include "config.h"
//#include "ImagesMeteo.c"

LV_IMG_DECLARE(IMG_01D);
LV_IMG_DECLARE(IMG_02D);
LV_IMG_DECLARE(IMG_03D);
LV_IMG_DECLARE(IMG_04D);
LV_IMG_DECLARE(IMG_09D);
LV_IMG_DECLARE(IMG_10D);
LV_IMG_DECLARE(IMG_11D);
LV_IMG_DECLARE(IMG_13D);
LV_IMG_DECLARE(IMG_50D);
LV_IMG_DECLARE(FOND);

static void meteo_event_handler(lv_obj_t * gauge1, lv_event_t event)
{
    if(event == LV_EVENT_PRESSED) {
      //  mSelect = lv_btnmatrix_get_active_btn(obj);
        KeyPressed = true;
    }
}

void appMeteo() {
static char txt_log[513];
int testWifi=3;

    lv_obj_t * label;

    TimeToWait = millis() + 10000;   // 10 seconde maxi dans le menu, sinon, on quitte.
    KeyPressed = false;  
    if (!WifiConnected) WiFi.begin(ssid, passwifi);
    while (WiFi.status() != WL_CONNECTED && testWifi) {
      delay(500);
      testWifi -= 1;
    }
    if (!FreshOpenWeather) GetMeteoInfo();                        // Mettre à jour les infos Météo
    txt_log[0] = '\0';

    lv_obj_clean(lv_scr_act());                                   // Clear Screen
    lv_obj_t * fond = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(fond, &FOND);
    lv_obj_align(fond, NULL, LV_ALIGN_IN_TOP_LEFT, 0,0);
    
    lv_obj_t * img1 = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(img1, &IMG_03D);
    if (!strncmp(OpenWeatherIconID,"02",2)) lv_img_set_src(img1, &IMG_02D);
    if (!strncmp(OpenWeatherIconID,"01",2)) lv_img_set_src(img1, &IMG_01D);
    if (!strncmp(OpenWeatherIconID,"04",2)) lv_img_set_src(img1, &IMG_04D);
    if (!strncmp(OpenWeatherIconID,"09",2)) lv_img_set_src(img1, &IMG_09D);
    if (!strncmp(OpenWeatherIconID,"11",2)) lv_img_set_src(img1, &IMG_11D);
    if (!strncmp(OpenWeatherIconID,"13",2)) lv_img_set_src(img1, &IMG_13D);
    if (!strncmp(OpenWeatherIconID,"50",2)) lv_img_set_src(img1, &IMG_50D);
    lv_obj_align(img1, NULL, LV_ALIGN_IN_TOP_LEFT, 0,0);
    // lv_obj_set_event_cb(img1, meteo_event_handler);

    lv_obj_t * label1 = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_long_mode(label1, LV_LABEL_LONG_BREAK);     /*Break the long lines*/
    lv_label_set_recolor(label1, true);                      /*Enable re-coloring by commands in the text*/
    lv_label_set_align(label1, LV_LABEL_ALIGN_CENTER);       /*Center aligned lines*/
    lv_label_set_text(label1, "#ffff00 Condition Meteo");
    lv_obj_set_width(label1, 200);
    lv_obj_align(label1, NULL, LV_ALIGN_IN_TOP_LEFT, 20, 0);

    static lv_style_t onestyle;
    lv_style_init(&onestyle);
    //lv_style_set_text_color(&onestyle, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_text_font(&onestyle, LV_STATE_DEFAULT, &lv_font_montserrat_22);

    lv_obj_t * txt = lv_label_create(lv_scr_act(), NULL);
    lv_obj_add_style(txt, LV_OBJ_PART_MAIN, &onestyle);
    lv_label_set_text(txt, txt_log);
    lv_label_set_recolor(txt, true);
    strcat(txt_log,"\n#ffffff  Temperature : "); strcat(txt_log,OpenWeatherTemp);
    strcat(txt_log,"\n#ffffff  Humidite    : "); strcat(txt_log,OpenWeatherHumidity);
    strcat(txt_log,"\n#ffffff  Pression    : "); strcat(txt_log,OpenWeatherPressure);
    strcat(txt_log,"\n#0000ff  ID          : "); strcat(txt_log,OpenWeatherID);
    strcat(txt_log,"#");
    lv_label_set_text(txt, txt_log); 
    lv_obj_align(txt, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 40);

    lv_obj_t * btn1 = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_event_cb(btn1, meteo_event_handler);
    lv_obj_align(btn1, txt, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
    label = lv_label_create(btn1, NULL);
    lv_label_set_text(label, "Quitter");

    while (!KeyPressed) {
      lv_task_handler(); 
      delay(20);
      if (TimeToWait < millis())KeyPressed = true;
    } 
    ttgo->tft->fillScreen(TFT_BLACK);
}
