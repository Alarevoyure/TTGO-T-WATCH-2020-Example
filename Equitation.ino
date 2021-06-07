/*******************************************************************************************************
 *    Module spécial Equitation !!
 *    
 *    Version 1.00
 *    P.BLOCK2021
 */

 void appEquitation() {
    String Direction = "Aquisition";
    int16_t x, y;
    uint8_t prevRotation, rotation;
    Acfg cfg;
    Accel acc;
        /*!
        Output data rate in Hz, Optional parameters:
            - BMA4_OUTPUT_DATA_RATE_0_78HZ
            - BMA4_OUTPUT_DATA_RATE_1_56HZ
            - BMA4_OUTPUT_DATA_RATE_3_12HZ
            - BMA4_OUTPUT_DATA_RATE_6_25HZ
            - BMA4_OUTPUT_DATA_RATE_12_5HZ
            - BMA4_OUTPUT_DATA_RATE_25HZ
            - BMA4_OUTPUT_DATA_RATE_50HZ
            - BMA4_OUTPUT_DATA_RATE_100HZ
            - BMA4_OUTPUT_DATA_RATE_200HZ
            - BMA4_OUTPUT_DATA_RATE_400HZ
            - BMA4_OUTPUT_DATA_RATE_800HZ
            - BMA4_OUTPUT_DATA_RATE_1600HZ
    */
    cfg.odr = BMA4_OUTPUT_DATA_RATE_100HZ;
    /*!
        G-range, Optional parameters:
            - BMA4_ACCEL_RANGE_2G
            - BMA4_ACCEL_RANGE_4G
            - BMA4_ACCEL_RANGE_8G
            - BMA4_ACCEL_RANGE_16G
    */
    cfg.range = BMA4_ACCEL_RANGE_2G;
    /*!
        Bandwidth parameter, determines filter configuration, Optional parameters:
            - BMA4_ACCEL_OSR4_AVG1
            - BMA4_ACCEL_OSR2_AVG2
            - BMA4_ACCEL_NORMAL_AVG4
            - BMA4_ACCEL_CIC_AVG8
            - BMA4_ACCEL_RES_AVG16
            - BMA4_ACCEL_RES_AVG32
            - BMA4_ACCEL_RES_AVG64
            - BMA4_ACCEL_RES_AVG128
    */
    cfg.bandwidth = BMA4_ACCEL_NORMAL_AVG4;

    /*! Filter performance mode , Optional parameters:
        - BMA4_CIC_AVG_MODE
        - BMA4_CONTINUOUS_MODE
    */
    cfg.perf_mode = BMA4_CONTINUOUS_MODE;

    // Configure the BMA423 accelerometer
    sensor->accelConfig(cfg);

    // Enable BMA423 accelerometer
    // Warning : Need to use feature, you must first enable the accelerometer
    // Warning : Need to use feature, you must first enable the accelerometer
    // Warning : Need to use feature, you must first enable the accelerometer
    sensor->enableAccel();
    
    while (!ttgo->getTouch(x, y)) { // Wait for touch to exit
       rotation = sensor->direction();
       sensor->getAccel(acc);
       if (prevRotation != rotation) {
          prevRotation = rotation;
          Serial.println(rotation);
          switch (rotation) {
            case DIRECTION_DISP_DOWN:
            //Direction = "Down";
            //No use
            break;
          case DIRECTION_DISP_UP:
            //Direction = "Up";
            //No use
            break;
          case DIRECTION_BOTTOM_EDGE:
            Direction = "Bottom";
            break;
          case DIRECTION_TOP_EDGE:
            Direction = "Top";
            break;
          case DIRECTION_RIGHT_EDGE:
            Direction = "Right";
             break;
          case DIRECTION_LEFT_EDGE:
            Direction = "Left";
            break;
          default:
            break;
          }
         //ttgo->tft->drawCentreString(Direction, 120, 120, 4);
        }
        ttgo->tft->fillScreen(TFT_BLACK);
        ttgo->tft->setTextFont(4);
       // ttgo->tft->setTextSize(4);
        ttgo->tft->drawString(Direction, 10, 40, 4);
        ttgo->tft->setCursor(10, 100);
        ttgo->tft->print("X:"); ttgo->tft->println(acc.x);
        ttgo->tft->setCursor(10, 130);
        ttgo->tft->print("Y:"); ttgo->tft->println(acc.y);
        ttgo->tft->setCursor(10, 160);
        ttgo->tft->print("Z:"); ttgo->tft->println(acc.z);
       delay(500);
    }
 }
