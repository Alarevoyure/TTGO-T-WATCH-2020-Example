// Show the accelerometer working == "bubble level"
// derived from Dan Geiger's accelerometer demo

#include "config.h"

void appNiveau() {
  ttgo->bma->begin();
  ttgo->bma->enableAccel();
  ttgo->tft->fillScreen(TFT_BLACK);
  int16_t x, y;
  int16_t xpos, ypos, oldx, oldy;

  Accel acc;

  while (!ttgo->getTouch(x, y)) { // Wait for touch to exit
    ttgo->bma->getAccel(acc);
    xpos = acc.x;
    ypos = acc.y;
    if (!(oldx==xpos && oldy==ypos)) {
      ttgo->tft->fillCircle(oldx / 10 + 119, oldy / 10 + 119, 10, TFT_BLACK); 
      ttgo->tft->drawCircle(120, 120, 25, TFT_GREEN);
      ttgo->tft->drawCircle(120, 120, 119, TFT_GREEN);
      ttgo->tft->drawLine(0, 120, 239, 120, TFT_GREEN);
      ttgo->tft->drawLine(120, 0, 120, 239, TFT_GREEN);
      ttgo->tft->fillCircle(xpos / 10 + 119, ypos / 10 + 119, 10, TFT_RED); // draw dot
      delay(100);
    }
    oldx = xpos;
    oldy = ypos;
  }

  //ttgo->tft->fillScreen(TFT_BLACK); // Clear screen
}
