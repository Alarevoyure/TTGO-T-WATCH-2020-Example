
static void display_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED) {
        mSelect = lv_btnmatrix_get_active_btn(obj);
        KeyPressed = true;
    }
}

uint8_t MenuDisplay(void)
{
    TimeToWait = millis() + 10000;   // 10 seconde maxi dans le menu, sinon, on quitte.
    
    static const char * btnm_map[] = {"Affichage", "\n",
                                      "ST","Ana","CL", "\n",
                                      "Quitter", ""};
                                  
    KeyPressed = false;
    lv_obj_t * btnm1 = lv_btnmatrix_create(lv_scr_act(), NULL);
    
    lv_btnmatrix_set_map(btnm1, btnm_map);
    lv_btnmatrix_set_btn_width(btnm1, 10, 2);        
    lv_btnmatrix_set_btn_ctrl(btnm1, 10, LV_BTNMATRIX_CTRL_CHECKABLE);
    lv_btnmatrix_set_btn_ctrl(btnm1, 11, LV_BTNMATRIX_CTRL_CHECK_STATE);
    lv_obj_set_size(btnm1, 240, 240);
    lv_obj_align(btnm1, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_event_cb(btnm1, display_event_handler);
    
    while (!KeyPressed) {
      lv_task_handler(); 
      delay(20);
      if (TimeToWait < millis()) {mSelect=99; KeyPressed = true;}
    } 
    ttgo->tft->fillScreen(TFT_BLACK);
    return mSelect;
}

 /************************************************************************************************
  *  Sélection du Thème.
  */
void displayTime(boolean fullUpdate){
  ledcWrite(0, Background);  // Configurer l'intensité de l'affichage
  switch (displayTheme) {
  case 0:  
     displayNumTime(fullUpdate);
     break;
  case 1:  
     displayAnalogTime(fullUpdate);
     break;
  case 2:  
     displayCLTime(fullUpdate);
     break;
  }
}

/************************************************************************************************
  *  Affichage horloge analogique
  */
void displayAnalogTime(boolean fullUpdate) {
//  float sx = 0, sy = 1, mx = 1, my = 0, hx = -1, hy = 0;    // Saved H, M, S x & y multipliers
  float sdeg = 0, mdeg = 0, hdeg = 0;
  uint16_t x0 = 0, x1 = 0, yy0 = 0, yy1 = 0;
  boolean initial = 1;
  
  // Get the current data
  RTC_Date tnow = ttgo->rtc->getDateTime();

  hh = tnow.hour;
  mm = tnow.minute;
  ss = tnow.second;
  dday = tnow.day;
  mmonth = tnow.month;
  yyear = tnow.year;


   //           Dessiner le cadran
    if (fullUpdate) {
       ttgo->tft->fillScreen(TFT_BLACK);
       ttgo->tft->setTextColor(TFT_DARKCYAN, TFT_BLACK);  // Adding a background colour erases previous text automatically

       // Draw clock face
       ttgo->tft->fillCircle(120, 120, 118, TFT_DARKGREEN);
       ttgo->tft->fillCircle(120, 120, 110, TFT_BLACK);

       // Draw 12 lines
       for (int i = 0; i < 360; i += 30) {
           sx = cos((i - 90) * 0.0174532925);
           sy = sin((i - 90) * 0.0174532925);
           x0 = sx * 114 + 120;
           yy0 = sy * 114 + 120;
           x1 = sx * 100 + 120;
           yy1 = sy * 100 + 120;

           ttgo->tft->drawLine(x0, yy0, x1, yy1, TFT_RED);
       }

       // Draw 60 dots
       for (int i = 0; i < 360; i += 6) {
           sx = cos((i - 90) * 0.0174532925);
           sy = sin((i - 90) * 0.0174532925);
           x0 = sx * 102 + 120;
           yy0 = sy * 102 + 120;
           // Draw minute markers
           ttgo->tft->drawPixel(x0, yy0, TFT_WHITE);

           // Draw main quadrant dots
           if (i == 0 || i == 180) ttgo->tft->fillCircle(x0, yy0, 2, TFT_WHITE);
           if (i == 90 || i == 270) ttgo->tft->fillCircle(x0, yy0, 2, TFT_WHITE);
       }

       ttgo->tft->fillCircle(120, 121, 3, TFT_WHITE);
    }
    
    // Pre-compute hand degrees, x & y coords for a fast screen update
    sdeg = ss * 6;                // 0-59 -> 0-354
    mdeg = mm * 6 + sdeg * 0.01666667; // 0-59 -> 0-360 - includes seconds
    hdeg = hh * 30 + mdeg * 0.0833333; // 0-11 -> 0-360 - includes minutes and seconds
    hx = cos((hdeg - 90) * 0.0174532925);
    hy = sin((hdeg - 90) * 0.0174532925);
    mx = cos((mdeg - 90) * 0.0174532925);
    my = sin((mdeg - 90) * 0.0174532925);
    sx = cos((sdeg - 90) * 0.0174532925);
    sy = sin((sdeg - 90) * 0.0174532925);

    if (ss == 0 || initial) {
        initial = 0;
        // Erase hour and minute hand positions every minute
        ttgo->tft->drawLine(ohx, ohy, 120, 121, TFT_BLACK);
        ohx = hx * 62 + 121;
        ohy = hy * 62 + 121;
        ttgo->tft->drawLine(omx, omy, 120, 121, TFT_BLACK);
        omx = mx * 84 + 120;
        omy = my * 84 + 121;
    }

    ttgo->tft->setTextSize(1);
    //Afficher le nombre de pas.
    uint32_t step = sensor->getCounter(); 
    ttgo->tft->drawNumber(step, 115, 190, 2);   // Afficher le nombre de pas.
    //Afficher la date
    AffDate (0, 0, 180, 112, TFT_DARKCYAN, TFT_BLACK, 0, dday);
    // Afficher temp
    if (FreshOpenWeather) ttgo->tft->drawString(String(OpenWeatherTemp), 35, 112, 4);
    //ttgo->tft->drawChar(60, 112, OpenWeatherTemp);
    //Afficher la charge batterie
    battery_icon(100, 32, TFT_DARKCYAN, TFT_BLACK);
    //Afficher état alarme
    if (Alarme1) ttgo->tft->drawString("A", 117, 54, 2);
    
    // Redraw new hand positions, hour and minute hands not erased here to avoid flicker
    ttgo->tft->drawLine(osx, osy, 120, 121, TFT_BLACK);
    osx = sx * 90 + 121;
    osy = sy * 90 + 121;
    //ttgo->tft->drawLine(osx, osy, 120, 121, TFT_YELLOW);
    ttgo->tft->drawLine(ohx, ohy, 120, 121, TFT_WHITE);
    ttgo->tft->drawLine(omx, omy, 120, 121, TFT_WHITE);
    ttgo->tft->drawLine(osx, osy, 120, 121, TFT_RED);
    ttgo->tft->fillCircle(120, 121, 3, TFT_RED);

}

/***********************************************************************************************
 * Affichage horloge Numérique
 */

 void displayNumTime(boolean fullUpdate) {
  String mStr;
  int xpos;
  
  // Get the current data
  RTC_Date tnow = ttgo->rtc->getDateTime();

  hh = tnow.hour;
  mm = tnow.minute;
  ss = tnow.second;
  dday = tnow.day;
  mmonth = tnow.month;
  yyear = tnow.year;

  ttgo->tft->setTextSize(1);

  if (fullUpdate) {

    //Draw the back graphics - Top of display
    ttgo->tft->fillScreen(TFT_BLACK);
    ttgo->tft->fillRoundRect(0, 0, 239, 120, 40, TFT_PURPLE);
    ttgo->tft->fillRoundRect(40, 20, 196, 80, 20, TFT_BLACK);
    ttgo->tft->fillRect(80, 20, 159, 80, TFT_BLACK);
    ttgo->tft->fillRect(170, 0, 70, 20, TFT_BLACK);
    ttgo->tft->fillRect(110, 0, 4, 20, TFT_BLACK);
    ttgo->tft->fillRect(0, 45, 50, 7, TFT_BLACK);
    ttgo->tft->fillRect(0, 70, 50, 7, TFT_BLACK);
    //ttgo->tft->fillRect(179, 0, 60, 20, TFT_DARKCYAN);

    //Draw the back graphics - Bottom of display
    ttgo->tft->fillRoundRect(0, 130, 239, 109, 40, TFT_MAROON);
    ttgo->tft->fillRoundRect(40, 150, 199, 88, 20, TFT_BLACK);
    ttgo->tft->fillRect(0, 179, 50, 10, TFT_BLACK);
    ttgo->tft->fillRect(100, 160, 60, 10, TFT_YELLOW);
    ttgo->tft->fillRect(160, 160, 40, 10, TFT_DARKGREEN);
    ttgo->tft->fillRect(200, 160, 20, 10, TFT_GREEN);
    ttgo->tft->setTextColor(TFT_WHITE, TFT_BLACK);
    ttgo->tft->drawString("Pas", 70, 158, 2);
    ttgo->tft->fillRoundRect(119, 210, 120, 29, 15, TFT_DARKCYAN);
    steps_icon(145, 213, TFT_WHITE, TFT_DARKCYAN, 1);
  }
  ttgo->tft->setTextSize(1);
  ttgo->tft->setTextColor(TFT_YELLOW);
  if (Alarme1) ttgo->tft->drawString("A", 122, 2, 2);
  if (Activite) ttgo->tft->drawString("S", 142, 2, 2);
  //if (DemandeOTA) ttgo->tft->drawString("O", 132, 2, 2);
  
  // Display Time
  // Font 7 is a 7-seg display but only contains
  // characters [space] 0 1 2 3 4 5 6 7 8 9 0 : .

  ttgo->tft->setTextColor(0xFBE0, TFT_BLACK);
  xpos = 55; 
  if (hh < 10) xpos += ttgo->tft->drawChar('0', xpos, 35, 7);
  xpos += ttgo->tft->drawNumber(hh, xpos, 35, 7);
  xpos += 3;
  if (ss % 2) ttgo->tft->setTextColor(0xE0, TFT_BLACK);    // permet de faire clignoter les : de l'heure.
  else  ttgo->tft->setTextColor(0xFBE0, TFT_BLACK);
  xpos += ttgo->tft->drawChar(':', xpos, 35, 7);
  ttgo->tft->setTextColor(0xFBE0, TFT_BLACK);
  if (mm < 10) xpos += ttgo->tft->drawChar('0', xpos, 35, 7);
  ttgo->tft->drawNumber(mm, xpos, 35, 7);

  battery_icon(176, 1, TFT_DARKCYAN, TFT_BLACK);
  charge_icon(152, 1, TFT_WHITE, TFT_PURPLE);


  // Draw Month
  AffDate (9, 194, 0, 0, TFT_WHITE, TFT_MAROON, mmonth, 0);

  //Afficher le nombre de pas.
  uint32_t step = sensor->getCounter(); 
  //ttgo->tft->fillRoundRect(0, 130, 239, 10, 40, TFT_MAROON);
  ttgo->tft->setTextColor(TFT_YELLOW, TFT_DARKCYAN);
  ttgo->tft->drawNumber(step, 170, 215, 2);   // Afficher le nombre de pas.
    
 // if (ss % 2) ttgo->tft->setTextColor(0xE0, TFT_BLACK);    // permet de faire clignoter les : de l'heure.
 // else  ttgo->tft->setTextColor(0xFBE0, TFT_BLACK);
 // ttgo->tft->drawChar(':', 122, 35, 7);
  ttgo->tft->setTextColor(0xFBE0, TFT_BLACK);             // Puis, afficher les secondes
  ttgo->tft->setTextSize(1);
  xpos = 204;
  if (ss < 10) xpos += ttgo->tft->drawChar('0', xpos, 47, 4);
  ttgo->tft->drawNumber(ss, xpos, 47, 4);
  ttgo->tft->drawString(String(dday), 50, 194, 4);

  // Affiche les performances d'activité de la marche
  ttgo->tft->fillRoundRect(100, 170, 280, 20, 5, TFT_BLACK);
  ttgo->tft->fillRoundRect(int(step * 0.01)+100, 170, 10, 20, 5, TFT_WHITE);

}

/***********************************************************************************************
 * Affichage horloge Numérique 2
 */

 void displayCLTime(boolean fullUpdate) {
  String mStr;
  int xpos, cal;
  unsigned long BckColor1 = 0x371010;
  unsigned long BckColor2 = 0x371010;
  unsigned long BckColor3 = 0x002300;
  
  // Get the current data
  RTC_Date tnow = ttgo->rtc->getDateTime();

  hh = tnow.hour;
  mm = tnow.minute;
  ss = tnow.second;
  dday = tnow.day;
  mmonth = tnow.month;
  yyear = tnow.year;

  ttgo->tft->setTextSize(1);
  //ttgo->tft->setTextColor(TFT_YELLOW);
  
  if (fullUpdate) {
     ttgo->tft->fillRoundRect(3, 26, 237, 64, 5, BckColor1);
     ttgo->tft->fillRoundRect(3, 96, 237, 64, 5, BckColor2);
     ttgo->tft->fillRoundRect(3, 166, 237, 64, 5, BckColor3);
     if (!WifiConnected) WiFi.begin(ssid, passwifi);
  }

  ttgo->tft->setTextColor(0xFBE0, BckColor1);
  xpos = 50; 
  if (hh < 10) xpos += ttgo->tft->drawChar('0', xpos, 35, 7);
  xpos += ttgo->tft->drawNumber(hh, xpos, 35, 7);
  xpos += 3;
  if (ss % 2) ttgo->tft->setTextColor(BckColor1, BckColor1);    // permet de faire clignoter les : de l'heure.
  else  ttgo->tft->setTextColor(0xFBE0, BckColor1);
  xpos += ttgo->tft->drawChar(':', xpos, 35, 7);
  ttgo->tft->setTextColor(0xFBE0, BckColor1);
  if (mm < 10) xpos += ttgo->tft->drawChar('0', xpos, 35, 7);
  ttgo->tft->drawNumber(mm, xpos, 35, 7);

  ttgo->tft->setTextColor(0xFBE0, BckColor1);             // Puis, afficher les secondes
  ttgo->tft->setTextSize(1);
  xpos = 204;
  if (ss < 10) xpos += ttgo->tft->drawChar('0', xpos, 47, 4);
  ttgo->tft->drawNumber(ss, xpos, 47, 4);

  //Afficher si alarme active ou non
  if (Alarme1) ttgo->tft->drawString("AL", 20, 40, 2);
  
  battery_icon(176, 1, TFT_DARKCYAN, TFT_BLACK);
  charge_icon(152, 1, TFT_WHITE, TFT_BLACK);
  AffRSSI (138, 1, TFT_WHITE, TFT_BLACK);

  // Affichage de la date
  //ttgo->tft->setTextSize(1);
 // ttgo->tft->setTextColor(TFT_GREENYELLOW);
 // ttgo->tft->drawString(String(dday), 50, 188, 6);
  
  //Afficher la météo.
  ttgo->tft->setTextColor(TFT_YELLOW, BckColor2);
  ttgo->tft->drawString("Temp :", 60, 103, 4);
  ttgo->tft->drawString("Hum :", 60, 133, 4);
  //if (FreshOpenWeather) ttgo->tft->drawString(String(OpenWeatherTemp), 125, 105, 4);
  //if (FreshOpenWeather) ttgo->tft->drawString(String(OpenWeatherHumidity), 125, 135, 4);
  ttgo->tft->drawString(String(OpenWeatherTemp), 145, 103, 4);
  ttgo->tft->drawString(String(OpenWeatherHumidity), 145, 133, 4);
  
  //Afficher le nombre de pas.
  ttgo->tft->setTextColor(TFT_YELLOW, BckColor3);
  ttgo->tft->drawString("Pas :", 60, 173, 4);
  ttgo->tft->drawString("Cal :", 60, 203, 4);
  uint32_t step = sensor->getCounter(); 
  ttgo->tft->drawNumber(step, 145, 173, 4);   // Afficher le nombre de pas.
  if (Activite) cal = step * 0.08;
  else cal = step * 0.04;
  ttgo->tft->drawNumber(cal, 145, 203, 4);   // Afficher le nombre de calories brulées.
  if (Activite) ttgo->tft->drawString("S", 20, 180, 2);

  // Mettre à jour les infos Météo si c'est nécessaire. Fréquence toutes les heures.
  if (WifiConnected && !FreshOpenWeather) GetMeteoInfo();      

  // Afficher la date
  AffDate (20, 110, 16, 130, TFT_YELLOW, BckColor2, mmonth, dday);

}
