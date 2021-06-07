#include "config.h"
#include <string.h>
#include <HTTPClient.h>
#include <ESP32httpUpdate.h>

//PCF8563_Class *rtc;

const char *ntpServer       = "0.fr.pool.ntp.org";
const long  gmtOffset_sec   = 3600;
const int   daylightOffset_sec = 0; //3600;

static void list_event_handler(lv_obj_t * obj, lv_event_t event)
{
  const char * wifi_ssid;
  
    TimeToWait = millis() + 10000;
    if(event == LV_EVENT_CLICKED) {
        wifi_ssid = lv_list_get_btn_text(obj);
        strcpy(ssid, wifi_ssid);
    }
}


static void WifiSelect_event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) {
        WIFIdefined = true;
        KeyPressed = true;
    }
}

static void wifi_event_handler(lv_obj_t * obj, lv_event_t event)
{
    char SSIDName[32] = "";
    int testWifi = 20;
     
    TimeToWait = millis() + 10000;
    if(event == LV_EVENT_VALUE_CHANGED) {
        mSelect = lv_btnmatrix_get_active_btn(obj);
        if (mSelect == 0) {                                            // Permet de choisir le réseau WIFI.
           WIFIdefined=false;
           // Afficher une fenêtre d'information 
           lv_obj_t * win = lv_win_create(lv_scr_act(), NULL);
           lv_win_set_title(win, "Scan Wifi");                       
           //lv_obj_t * txt = lv_label_create(win, NULL);
           
           //Afficher une liste avec l'ensemble des réseaux WIFI disponibles
           lv_obj_t * list1 = lv_list_create(lv_scr_act(), NULL);
           lv_obj_set_size(list1, 200, 130);
           lv_obj_align(list1, NULL, LV_ALIGN_IN_TOP_MID, 0, 45);

           //Ajouter un élément dans la liste par réseau.
           lv_obj_t * list_btn;
           int n = WiFi.scanNetworks();
           if (n == 0) {
           } else {
             for (int i = 0; i < n; ++i) {
               WiFi.SSID(i).toCharArray(SSIDName,32);
               list_btn = lv_list_add_btn(list1, LV_SYMBOL_WIFI, SSIDName );
               lv_obj_set_event_cb(list_btn, list_event_handler);
             }
           }
           lv_obj_t * label;
           lv_obj_t * btn1 = lv_btn_create(lv_scr_act(), NULL);
           lv_obj_t * btn1Ok = lv_btn_create(lv_scr_act(), NULL);
           lv_obj_set_event_cb(btn1, Quit_event_handler);
           lv_obj_set_width(btn1, 100);
           lv_obj_align(btn1, NULL, LV_ALIGN_IN_TOP_MID, -60, 190);
           label = lv_label_create(btn1, NULL);
           lv_label_set_text(label, "Quitter");
           lv_obj_set_event_cb(btn1Ok, WifiSelect_event_handler);
           lv_obj_set_width(btn1Ok, 100);
           lv_obj_align(btn1Ok, NULL, LV_ALIGN_IN_TOP_MID, 60, 190);
           label = lv_label_create(btn1Ok, NULL);
           lv_label_set_text(label, "OK");
       // KeyPressed = true;
      }
      if (mSelect != 0) KeyPressed = true;
    }
}


uint8_t appWifi(void)
{
    TimeToWait = millis() + 10000;   // 10 seconde maxi dans le menu, sinon, on quitte.
    
    static const char * btnm_map[] = {"Scan", "Configuration", "\n",
                                      "Synchro", "MaJ", "\n",
                                      "Quitter", ""};
                                  
    KeyPressed = false;
    lv_obj_t * btnm1 = lv_btnmatrix_create(lv_scr_act(), NULL);
    
    lv_btnmatrix_set_map(btnm1, btnm_map);
    lv_btnmatrix_set_btn_width(btnm1, 10, 2);        
    lv_btnmatrix_set_btn_ctrl(btnm1, 10, LV_BTNMATRIX_CTRL_CHECKABLE);
    lv_btnmatrix_set_btn_ctrl(btnm1, 11, LV_BTNMATRIX_CTRL_CHECK_STATE);
    lv_obj_set_size(btnm1, 240, 240);
    lv_obj_align(btnm1, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_event_cb(btnm1, wifi_event_handler);
    
    while (!KeyPressed) {
      lv_task_handler(); 
      delay(20);
      if (TimeToWait < millis()) {mSelect=99; KeyPressed = true;}
    } 
    ttgo->tft->fillScreen(TFT_BLACK);
    return mSelect;
}


uint8_t SynchroWifi(char * wifi_ssid, char * wifi_pass){
  struct tm timeinfo;
  static char txt_log[513];

    txt_log[0] = '\0';
    int testWifi = 20;
    KeyPressed = false;
    strcat(txt_log,"Preparation connexion WIFI\n");
    lv_obj_t * win = lv_win_create(lv_scr_act(), NULL);
    lv_win_set_title(win, "Synchronisation WIFI");    
    lv_obj_t * txt = lv_label_create(win, NULL);
    lv_label_set_text(txt, txt_log);
    Lecture_config();
    if (WIFIdefined) {
       WiFi.begin(wifi_ssid, wifi_pass);
       strcat(txt_log,"Connexion ");
       while (WiFi.status() != WL_CONNECTED && testWifi) {
         strcat(txt_log,".");  
         lv_label_set_text(txt, txt_log);  
         lv_task_handler(); 
         delay(500);
         testWifi -= 1;
       }
       if (testWifi) {
          strcat(txt_log,"\nConnexion reussie.\n");
          lv_label_set_text(txt, txt_log); lv_task_handler(); 
          configTzTime("CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00", ntpServer);
          //configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
          delay(1000);
          if (!getLocalTime(&timeinfo)) {
             lv_label_set_text(txt, "Connexion au serveur.\nde temps impossible"); lv_task_handler(); 
             delay(3000);
          } 
          else {
             //Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
             strcat(txt_log,"Synchronisation reussie");
             lv_label_set_text(txt, txt_log); lv_task_handler(); 
             ttgo->rtc->syncToRtc();
             WifiConnected = 1;
          }
          if (WaitingOpenWeather == 0) {
              GetMeteoInfo();
              strcat(txt_log,"\n  --  Meteo  -- \n");
              strcat(txt_log,OpenWeatherDescription);
              lv_label_set_text(txt, txt_log); lv_task_handler(); 
          }
          //strcat(txt_log,"\nActivation OTA\n");
          //lv_label_set_text(txt, txt_log); lv_task_handler(); 
          //ConfigOTA();
       }
    }
    else {
      lv_label_set_text(txt, "Réseau WIFI non defini.\n"); lv_task_handler(); 
    }
    
    delay(3000);
    ttgo->tft->fillScreen(TFT_BLACK);
}


uint8_t MiseAJourWifi(char * wifi_ssid, char * wifi_pass){
  struct tm timeinfo;
  static char txt_log[513];
  int httpCode;
  String OldVersion;

    txt_log[0] = '\0';
    int testWifi = 20;
    KeyPressed = false;
    strcat(txt_log,"Preparation connexion WIFI\n");
    lv_obj_t * win = lv_win_create(lv_scr_act(), NULL);
    lv_win_set_title(win, "Update Software");    
    lv_obj_t * txt = lv_label_create(win, NULL);
    lv_label_set_text(txt, txt_log);
    Lecture_config();
    if (WIFIdefined) {
       WiFi.begin(wifi_ssid, wifi_pass);
       strcat(txt_log,"Connexion\n");
       while (WiFi.status() != WL_CONNECTED && testWifi) {
         strcat(txt_log,".");  
         lv_label_set_text(txt, txt_log); lv_task_handler(); 
         delay(500);
         testWifi -= 1;
       }
       if (testWifi) {
          strcat(txt_log,"\nConnexion WIFI reussie.\n");
          lv_label_set_text(txt, txt_log); lv_task_handler(); 
          configTzTime("CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00", ntpServer);
          //configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
          delay(1000);
          strcat(txt_log,"Connexion au serveur...\n"); 
          lv_label_set_text(txt, txt_log); lv_task_handler(); 

          //Vérification de la version dans le repository
          http.begin("http://store.ferme201.fr/download/ttgo/version.info");
          httpCode = http.GET();
          if (httpCode > 0) {
             if (httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                //txt_log[0] = '\0';
                strcat(txt_log, "Version actuelle : ");
                strcat(txt_log, VersionSoftware.c_str());
                strcat(txt_log, "\nVersion en ligne : ");
                strcat(txt_log, payload.c_str());
                lv_label_set_text(txt, txt_log); lv_task_handler(); 
                if (payload != VersionSoftware) {
                  OldVersion = VersionSoftware;
                  VersionSoftware = payload;
                  Sauvegarde_config();
                  strcat(txt_log, "\nMise a jour en cours\n");
                  lv_label_set_text(txt, txt_log); lv_task_handler(); 
                  t_httpUpdate_return ret = ESPhttpUpdate.update("http://store.ferme201.fr/download/ttgo/ttgo.bin");
                  switch(ret) {
                     case HTTP_UPDATE_FAILED:
                        strcat(txt_log, "HTTP_UPDATE_FAILD Error\n");
                     break;
                     case HTTP_UPDATE_NO_UPDATES:
                        strcat(txt_log, "HTTP_UPDATE_NO_UPDATES\n");
                     break;
                     case HTTP_UPDATE_OK:
                        strcat(txt_log, "HTTP_UPDATE_OK\n");
                     break;
                  } 
                  VersionSoftware = OldVersion;
                  Sauvegarde_config();
                }
                else {
                  strcat(txt_log, "\nvous utilisez la derniere version.");
               }
            }
          } else {
             strcat(txt_log, "[HTTP] GET... failed\n");
          }
          lv_label_set_text(txt, txt_log); lv_task_handler();
          http.end();
          delay(2000);
       }
    }
    else {
      lv_label_set_text(txt, "Réseau WIFI non defini.\n"); lv_task_handler(); 
    }
    
    delay(3000);
    ttgo->tft->fillScreen(TFT_BLACK);
}
