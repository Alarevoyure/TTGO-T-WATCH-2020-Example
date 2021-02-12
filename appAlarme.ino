#include <AceTime.h>

static void OkAlarm_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) {
        KeyPressed = true;
        Alarme1 = true;
        //Serial.printf("rtc -> %d/%02d/%02d %d:%02d:%02d\n", yyear, mmonth, dday, hh, mm, ss);
        ttgo->rtc->disableAlarm();
        ttgo->rtc->setAlarm(Alarme1Hour, Alarme1Min, PCF8563_NO_ALARM, PCF8563_NO_ALARM);
        ttgo->rtc->enableAlarm();
        Serial.print("Alarme programmée pour : ");Serial.print(Alarme1Hour);Serial.print(":");Serial.println(Alarme1Min);
        Sauvegarde_config();
    }
}

static void QuitAlarm_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) {
        KeyPressed = true;
        Alarme1 = false;
        ttgo->rtc->disableAlarm();
        Sauvegarde_config();
    }
}


static void hour_event_handler(lv_obj_t * obj, lv_event_t event)
{
    TimeToWait = millis() + 20000;
    if(event == LV_EVENT_VALUE_CHANGED) {
        Alarme1Hour = lv_roller_get_selected(obj);
        printf("Selected hour: %i\n", Alarme1Hour);
    }
}

static void min_event_handler(lv_obj_t * obj, lv_event_t event)
{
    TimeToWait = millis() + 20000;
    if(event == LV_EVENT_VALUE_CHANGED) {
        Alarme1Min = lv_roller_get_selected(obj);
        printf("Selected Minute: %i\n", Alarme1Min);
    }
}


void appAlarme() {

    TimeToWait = millis() + 20000;   // 10 seconde maxi dans le menu, sinon, on quitte.
    KeyPressed = false;  

    lv_obj_clean(lv_scr_act());                                   // Clear Screen
    // Afficher le 'roller' pour les heures
    lv_obj_t *rolHour = lv_roller_create(lv_scr_act(), NULL);
    lv_roller_set_options(rolHour,"00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23",LV_ROLLER_MODE_INIFINITE);
    lv_roller_set_visible_row_count(rolHour, 3);
    lv_roller_set_selected(rolHour, Alarme1Hour, LV_ANIM_ON);
    lv_obj_align(rolHour, NULL, LV_ALIGN_CENTER, -40, 0);
    lv_obj_set_event_cb(rolHour, hour_event_handler);

    // Afficher le 'roller' pour les minutes
    lv_obj_t *rolMin = lv_roller_create(lv_scr_act(), NULL);
    lv_roller_set_options(rolMin, "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n30\n31\n32\n33\n34\n35\n36\n37\n38\n39\n40\n41\n42\n43\n44\n44\n45\n46\n47\n48\n49\n50\n51\n52\n53\n54\n55\n56\n57\n58\n59",LV_ROLLER_MODE_INIFINITE);
    lv_roller_set_visible_row_count(rolMin, 3);
    lv_roller_set_selected(rolMin, Alarme1Min, LV_ANIM_ON);
    lv_obj_align(rolMin, NULL, LV_ALIGN_CENTER, 40, 0);
    lv_obj_set_event_cb(rolMin, min_event_handler);

    // Ajouter les boutons OK et Quitter.
    lv_obj_t * label;
    lv_obj_t * btn1 = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_t * btn1Ok = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_event_cb(btn1, QuitAlarm_event_handler);
    lv_obj_set_width(btn1, 100);
    lv_obj_align(btn1, NULL, LV_ALIGN_IN_TOP_MID, -60, 190);
    label = lv_label_create(btn1, NULL);
    lv_label_set_text(label, "Quitter");
    lv_obj_set_event_cb(btn1Ok, OkAlarm_event_handler);
    lv_obj_set_width(btn1Ok, 100);
    lv_obj_align(btn1Ok, NULL, LV_ALIGN_IN_TOP_MID, 60, 190);
    label = lv_label_create(btn1Ok, NULL);
    lv_label_set_text(label, "OK");

    lv_obj_t * label1 = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_long_mode(label1, LV_LABEL_LONG_BREAK);     /*Break the long lines*/
    lv_label_set_recolor(label1, true);                      /*Enable re-coloring by commands in the text*/
    lv_label_set_align(label1, LV_LABEL_ALIGN_CENTER);       /*Center aligned lines*/
    lv_label_set_text(label1, "#0000ff Configuration alarme#\n"
                              "Heure  -  Minute");
    lv_obj_set_width(label1, 200);
    lv_obj_align(label1, NULL, LV_ALIGN_IN_TOP_MID, 0, 10);

    while (!KeyPressed) {
      lv_task_handler(); 
      delay(20);
      if (TimeToWait < millis())KeyPressed = true;
    } 
    ttgo->tft->fillScreen(TFT_BLACK);
}
