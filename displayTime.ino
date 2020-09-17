// An advanced Time Display
// This is a drop-in replacement for the displayTime()
// In the original Instructable. s such, it redraws GUI
// every minute so you will see a little flicker.

int xpos;

void displayTime(boolean fullUpdate) {
  
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
    ttgo->tft->fillRect(170, 0, 45, 20, TFT_BLACK);
    ttgo->tft->fillRect(110, 0, 4, 20, TFT_BLACK);
    ttgo->tft->fillRect(0, 45, 50, 7, TFT_BLACK);
    ttgo->tft->fillRect(0, 70, 50, 7, TFT_BLACK);
    ttgo->tft->fillRect(215, 0, 24, 20, TFT_DARKCYAN);

    //Draw the back graphics - Bottom of display
    ttgo->tft->fillRoundRect(0, 130, 239, 109, 40, TFT_MAROON);
    ttgo->tft->fillRoundRect(40, 150, 199, 88, 20, TFT_BLACK);
    ttgo->tft->fillRect(0, 179, 50, 10, TFT_BLACK);
    ttgo->tft->fillRect(100, 160, 40, 10, TFT_YELLOW);
    ttgo->tft->fillRect(140, 160, 40, 10, TFT_DARKGREEN);
    ttgo->tft->fillRect(180, 160, 40, 10, TFT_GREEN);
    ttgo->tft->setTextColor(TFT_WHITE, TFT_BLACK);
    ttgo->tft->drawString("Pas", 70, 158, 2);
    ttgo->tft->fillRoundRect(119, 210, 120, 29, 15, TFT_DARKCYAN);

    ttgo->tft->setTextSize(1);
    ttgo->tft->setTextColor(TFT_YELLOW);
    ttgo->tft->drawString("Power", 124, 2, 2);
  }
  
  // Display Time
  // Font 7 is a 7-seg display but only contains
  // characters [space] 0 1 2 3 4 5 6 7 8 9 0 : .

  ttgo->tft->setTextColor(0xFBE0, TFT_BLACK);
  xpos = 55; 
  if (hh < 10) xpos += ttgo->tft->drawChar('0', xpos, 35, 7);
  xpos += ttgo->tft->drawNumber(hh, xpos, 35, 7);
  xpos += 3;
  if (ss % 2) ttgo->tft->setTextColor(0xE0, TFT_BLACK);    // permet de faire clignoter les : de l'heure.
  xpos += ttgo->tft->drawChar(':', xpos, 35, 7);
  ttgo->tft->setTextColor(0xFBE0, TFT_BLACK);
  if (mm < 10) xpos += ttgo->tft->drawChar('0', xpos, 35, 7);
  ttgo->tft->drawNumber(mm, xpos, 35, 7);

  // Display Battery Level
  ttgo->tft->setTextSize(1);
  ttgo->tft->setTextColor(TFT_GREEN, TFT_BLACK);
  int per = ttgo->power->getBattPercentage();
  per = ttgo->power->getBattPercentage();
  if (per<=100) ttgo->tft->drawString(String(per) + "%  ", 179, 2, 2);

  // Affichage de la date
  //ttgo->tft->setTextSize(1);
  ttgo->tft->setTextColor(TFT_GREENYELLOW);
  ttgo->tft->drawString(String(dday), 50, 188, 4);
  
  // Draw Month
  String mStr;
  switch (mmonth) {
    case 1: mStr = "Jan"; break;
    case 2: mStr = "Feb"; break;
    case 3: mStr = "Mar"; break;
    case 4: mStr = "Avr"; break;
    case 5: mStr = "Mai"; break;
    case 6: mStr = "Jui"; break;
    case 7: mStr = "Jul"; break;
    case 8: mStr = "Aou"; break;
    case 9: mStr = "Sep"; break;
    case 10: mStr = "Oct"; break;
    case 11: mStr = "Nov"; break;
    case 12: mStr = "Dec"; break;
  }
  ttgo->tft->setTextColor(TFT_WHITE);
  ttgo->tft->drawString(mStr, 9, 194, 2);
  
  //Afficher le nombre de pas.
  uint32_t step = sensor->getCounter();
  //ttgo->tft->fillRoundRect(0, 130, 239, 10, 40, TFT_MAROON);
  ttgo->tft->setTextColor(TFT_YELLOW, TFT_DARKCYAN);
  ttgo->tft->drawNumber(step, 170, 215, 2);   // Afficher le nombre de pas.
  
  if (ss % 2) ttgo->tft->setTextColor(0xE0, TFT_BLACK);    // permet de faire clignoter les : de l'heure.
  else  ttgo->tft->setTextColor(0xFBE0, TFT_BLACK);
  ttgo->tft->drawChar(':', 122, 35, 7);
  ttgo->tft->setTextColor(0xFBE0, TFT_BLACK);             // Puis, afficher les secondes
  ttgo->tft->setTextSize(1);
  xpos = 204;
  if (ss < 10) xpos += ttgo->tft->drawChar('0', xpos, 47, 4);
  ttgo->tft->drawNumber(ss, xpos, 47, 4);

  // Affiche les performances d'activité de la marche
   ttgo->tft->fillRoundRect(100, 170, 280, 20, 5, TFT_BLACK);
  ttgo->tft->fillRoundRect(int(step * 0.01)+100, 170, 10, 20, 5, TFT_WHITE);

    /* Build a bargraph every 10 seconds
  int secmod = ss % 10;
  if (secmod) { // Show growing bar every 10 seconds
    ttgo->tft->fillRect(126 + secmod * 10, 215, 6, 15, TFT_ORANGE);
  } else {
    ttgo->tft->fillRoundRect(119, 210, 120, 29, 15, TFT_DARKCYAN);
  } */
}
