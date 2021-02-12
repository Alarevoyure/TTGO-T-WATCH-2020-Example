// simple Polish Postfix four function calculator
#include <math.h>
#include "config.h"
//#include "DudleyWatch.h"

enum SWIPE_DIR { NODIR=31, UP, DOWN, LEFT, RIGHT, CWCIRCLE, CCWCIRCLE };

const char *calc_labels1[]
          = {
        "1", "2", "3", "+",
        "4", "5", "6", "-",
        "7", "8", "9", "*",
        ".", "0", "=", "/"};

const char button_values1[] = {'1', '2', '3', '+',
             '4', '5', '6', '-',
             '7', '8', '9', '*',
             '.', '0', '=', '/' };

static float stack[5];
static float C1=0, C2=0;

void clear_stack(){
   for(int i = 5 ; i > 0 ; i--) {
       stack[i] = 0;
    }
}

/*
void push(float x) {
    for(int i = (sizeof(stack)/sizeof(float)) - 1 ; i > 0 ; i--) {
       stack[i] = stack[i - 1];
    }
    stack[0] = x;
}

float pop(void) {
float r;
    r = stack[0];
    for(int i = 0 ; i < (sizeof(stack)/sizeof(float)) - 1 ; i++) {
       stack[i] = stack[i + 1];
    }
    return(r);
}
*/

void appCalculator(void) {
int mSelect = 0;
char c, *cp, ibuf[20], buf[200];
boolean needs_push;
int last_len, current_kbd;
int Operateur=0;       //+=1   -=2    *=3    /=4    ==5
int index=1;

    KeyPressed = false;
    TimeToWait = millis() + 20000;   // 40 seconde maxi dans le menu, sinon, on quitte.
    stack[1]=1;
    
    current_kbd = 0;
    for(int i = 0 ; i < sizeof(stack)/sizeof(float) ; i++) {
       stack[i] = 0.0;
    }
    memset(ibuf, '\0', sizeof(ibuf)/sizeof(char));
    memset(buf, '\0', sizeof(buf)/sizeof(char));
    draw_keyboard(16, calc_labels1, 4, true, "");
    do {
       delay(100);

       mSelect = poll_swipe_or_menu_press(16); // poll for touch or gesture
       c = 0xff;
       c = button_values1[mSelect];
       if(c != 0xff) {
           //Serial.printf("c = %02x = '%c'\n", c, c);
           switch(c) {
           case '0' : case '1' : case '2' : case '3' : case '4' :
           case '5' : case '6' : case '7' : case '8' : case '9' :
           case '.' :
               quickBuzz (50);
               if(needs_push) {
                  memset(ibuf, '\0', sizeof(ibuf)/sizeof(char));
               //   push(0.0);
                  needs_push = 0;
                  ttgo->tft->fillRect(0, 5, 240, 10, TFT_BLACK);
               }
               ibuf[strlen(ibuf)] = c;
               ibuf[strlen(ibuf)+1] = '\0';
               if (index==1) C1 = atof(ibuf);
               if (index==2) C2 = atof(ibuf);
               stack[0] = atof(ibuf);
               TimeToWait = millis() + 20000;
               break;
           case '=' :  
               quickBuzz (50);
               if (Operateur==5){
                  C1=0;
                  C2=0;
                  index=0;
                  stack[0]=0;
               }
               index += 1;
               needs_push = 1;
               if (index == 3) {
                 if (Operateur==1) stack[0] = C1+C2;
                 if (Operateur==2) stack[0] = C1-C2;
                 if (Operateur==3) stack[0] = C1*C2;
                 if (Operateur==4) stack[0] = C1/C2;
                 C1 = stack[0];
                 C2 = 0;
                 index = 2;
               }
               Operateur = 5;
               ttgo->tft->fillRect(200, 5, 240, 21, TFT_BLACK); ttgo->tft->drawCentreString( "=", 220, 5, 4); break;
               break;
           case '+' :
               quickBuzz (50);
               needs_push = 1;
               index += 1;
               Operateur = 1;
               if (index == 3) {
                 if (C2) stack[0] = C1+C2;
                 C1 = stack[0];
                 C2 = 0;
                 index = 2;
               }
               ttgo->tft->fillRect(200, 5, 240, 21, TFT_BLACK);ttgo->tft->drawCentreString( "+", 220, 5, 4); break;
               break;
            case '-' :
               quickBuzz (50);
               needs_push = 1;
               index += 1;
               Operateur = 2;
               if (index == 3) {
                 if (C2) stack[0] = C1-C2;
                 C1 = stack[0];
                 C2 = 0;
                 index = 2;
               }
               ttgo->tft->fillRect(200, 5, 240, 21, TFT_BLACK);ttgo->tft->drawCentreString( "-", 220, 5, 4); break;
               break;
           case '*' :
               quickBuzz (50);
               needs_push = 1;
               index += 1;
               Operateur = 3;
               if (index == 3) {
                 if (C2) stack[0] = C1*C2;
                 C1 = stack[0];
                 C2 = 0;
                 index = 2;
               }
               ttgo->tft->fillRect(200, 5, 240, 21, TFT_BLACK);ttgo->tft->drawCentreString( "*", 220, 5, 4); break;
               break;
           case '/' :
               quickBuzz (50);
               needs_push = 1;
               Operateur = 4;
               index += 1;
               if (index == 3) {
                 if (C2) stack[0] = C1/C2;
                 C1 = stack[0];
                 C2 = 0;
                 index = 2;
               }
               ttgo->tft->fillRect(200, 5, 240, 21, TFT_BLACK);ttgo->tft->drawCentreString( "/", 220, 5, 4); break;
               break;
           }

           if ( stack[0]!=stack[1]) {
             sprintf(buf, "%f", stack[0]);
             ttgo->tft->fillRect(0, 5, 200, 21, TFT_BLACK);
             ttgo->tft->setTextColor(TFT_GREEN);
             ttgo->tft->drawCentreString( buf, 0, 5, 4);
             last_len = strlen(buf);
             stack[1]=stack[0];
           }
       }
       if (TimeToWait < millis())KeyPressed = true;
    } while(!KeyPressed);
    
    ttgo->tft->fillScreen(TFT_BLACK);
}



int poll_swipe_or_menu_press(int num_choices) {
uint32_t lasttouch, interval;
int16_t nx, ny, x, y, x0, y0, xmax, ymax, amax, points;
enum SWIPE_DIR resdir;
  resdir = NODIR;
  if(num_choices < 14) { num_choices = 12; }
  else { num_choices = 16; }
  int xdir = 0;
  int ydir = 0;
  x0 = -1;
  y0 = -1;
  char dir;
  int16_t max_lrud[4] = {1000, -1, 1000, -1};
  int16_t pts_lrud[4] = {-1, -1, -1, -1};
  points = 0;
  lasttouch = millis();
  do {
    interval = millis() - lasttouch;
    // Serial.printf("(1) interval = %lu\n", interval);
    if(ttgo->getTouch(nx, ny)) {
      // Serial.print(F("T"));
      if((nx > 0) && (ny > 0) && (nx < 240) && (ny < 240)) {
  // Serial.println(F("G"));
  x = nx; y = ny;
  // remember the first touch:
  if(x0 < 0) { x0 = x; }
  if(y0 < 0) { y0 = y; }
  int i = points;
  if(pts_lrud[0] < 0 || x < max_lrud[0]) { max_lrud[0] = x; pts_lrud[0] = i++; }
  if(pts_lrud[1] < 0 || x > max_lrud[1]) { max_lrud[1] = x; pts_lrud[1] = i++; }
  if(pts_lrud[2] < 0 || y < max_lrud[2]) { max_lrud[2] = y; pts_lrud[2] = i++; }
  if(pts_lrud[3] < 0 || y > max_lrud[3]) { max_lrud[3] = y; pts_lrud[3] = i++; }
  points++;
      }
      lasttouch = millis();
    }
  } while(interval < 100);
#if DBG_GESTURES
  if(points) {
    Serial.printf("i = %d, p = %d\n", interval, points);
    Serial.printf("max_lrud[] = %d, %d, %d, %d\n", max_lrud[0], max_lrud[1], max_lrud[2], max_lrud[3]);
    Serial.printf("pts_lrud[] = %d, %d, %d, %d\n", pts_lrud[0], pts_lrud[1], pts_lrud[2], pts_lrud[3]);
  }
#endif
  if(x0 >= 0 && y0 >= 0) {
    // Serial.printf("(2) interval = %lu\n", interval);
    xdir = x - x0;
    ydir = y - y0;
    xmax = abs(xdir);
    ymax = abs(ydir);
    amax = (xmax > ymax) ? xmax : ymax ;
#if DBG_GESTURES
    Serial.printf("x0 = %d, x = %d, xdir = %d\n", x0, x, xdir);
    Serial.printf("y0 = %d, y = %d, ydir = %d, amax = %d\n", y0, y, ydir, amax);
#endif
    if(points > 100) {  // is gesture a circle ?
#if DBG_GESTURES
      int8_t max_order[4] = {-1, -1, -1, -1};
#endif
      int16_t min_value[4] = {1000, 1000, 1000, 1000};
      char lut[4] = { 'L', 'R', 'U', 'D' };
      char dir_order[9];
      dir_order[8] = '\0';
#if DBG_GESTURES
      Serial.printf("maybe a circle\n");
#endif
      // CW  circle -> u, r, d, l (starting at any point in the list)
      // CCW circle -> u, l, d, r (starting at any point in the list)
      // so if right after up and left after down, then CW
      // so if left after up and right after down, then CCW
      // else not a circle
      // order the max_lrud points
      int8_t min_index;
      for(int8_t j = 0 ; j < 4 ; j++) {
  for(int8_t i = 0 ; i < 4 ; i++) {
    if(pts_lrud[i] < min_value[j] && (j == 0 || pts_lrud[i] > min_value[j-1])) {
      min_value[j] = pts_lrud[i];
      min_index = i;
    }
  }
#if DBG_GESTURES
  max_order[j] = min_index;
#endif
  dir_order[j] = lut[min_index];
  dir_order[j+4] = lut[min_index];
      }
#if DBG_GESTURES
      Serial.printf("max_order[4] = %d, %d, %d, %d\n", max_order[0], max_order[1], max_order[2], max_order[3]);
      Serial.printf("dir_order = %s\n", dir_order);
#endif
      if(strstr(dir_order, "URDL")) {
#if DBG_GESTURES
  Serial.println(F("found CW CIRCLE"));
#endif
  return(CWCIRCLE);
      }
      if(strstr(dir_order, "ULDR")) {
#if DBG_GESTURES
  Serial.println(F("found CCW CIRCLE"));
#endif
  return(CCWCIRCLE);
      }
    }
    else if(amax > 60) { // moved across 1/4 of the screen, so probably a swipe
      dir = (ymax > xmax) ? 'y' : 'x' ;
#if DBG_GESTURES
      Serial.printf("points = %d\n", points);
      Serial.printf("xmax = %d, ymax = %d, dir = %c\n", xmax, ymax, dir);
#endif
      resdir = 
  (dir == 'x') ? ((xdir > 0) ? RIGHT : LEFT ) :
      (ydir > 0) ? DOWN : UP;
      return resdir;
    }
    else {  // must be button press
      if(num_choices == 12) {
  if (y < 85) {
    if (x < 80) return 0;
    else if (x > 160) return 2;
    else return 1;
  }
  else if (y < 135) {
    if (x < 80) return 3;
    else if (x > 160) return 5;
    else return 4;
  }
  else if (y < 185) {
    if (x < 80) return 6;
    else if (x > 160) return 8;
    else return 7;
  }
  else if (x < 80) return 9;
  else if (x > 160) return 11;
  else return 10;
      }
      else {  // must be 16 choices
  if (y < 85) {
    if (x < 60) return 0;
    else if (x < 120) return 1;
    else if (x < 180) return 2;
    else return 3;
  }
  else if (y < 135) {
    if (x < 60) return 4;
    else if (x < 120) return 5;
    else if (x < 180) return 6;
    else return 7;
  }
  else if (y < 185) {
    if (x < 60) return 8;
    else if (x < 120) return 9;
    else if (x < 180) return 10;
    else return 11;
  }
  if (x < 60) return 12;
  else if (x < 120) return 13;
  else if (x < 180) return 14;
  else return 15;
      }
    }
  }
  return -1;
}
