#include "Watchy_Multi.h"

#include "BadForEye.h"

#include "Seven_Segment10pt7b.h"
#include "DSEG7_Classic_Regular_15.h"
#include "DSEG7_Classic_Bold_25.h"
#include "DSEG7_Classic_Regular_39.h"
#include "7_SEG_icons.h"

#include "DIN_1451_Engschrift_Regular12pt7b.h"
#include "DIN_1451_Engschrift_Regular64pt7b.h"

#define TEMP_UNIT_METRIC true
#define DARKMODE false
#define FACE_COUNT 4

uint32_t faceIdx = 0;

void WatchyMulti::handleButtonPress() {
  uint64_t wakeupBit = esp_sleep_get_ext1_wakeup_status();                                                                                             
  if( guiState == WATCHFACE_STATE && 
      wakeupBit & BACK_BTN_MASK && wakeupBit & UP_BTN_MASK ) {
    faceIdx = (faceIdx+1) % FACE_COUNT;
  }
}

void WatchyMulti::drawWatchFace() {
  if( faceIdx == 1 ) {
    drawWatchFaceBadForEye();
  } else if( faceIdx == 2 ) {
    drawWatchFaceLine();    
  } else if( faceIdx == 3 ) {
    drawWatchFaceBahn();
  } else {
    drawWatchFace7SEG();
  }
}

const unsigned char *numbers [10] = {numbers0, numbers1, numbers2, numbers3, numbers4, numbers5, numbers6, numbers7, numbers8, numbers9};

void WatchyMulti::drawWatchFaceBadForEye(){
    display.fillScreen(GxEPD_BLACK);
    display.drawBitmap(0, 0, window, DISPLAY_WIDTH, DISPLAY_HEIGHT, GxEPD_WHITE);

    //Hour
    display.drawBitmap(50, 10, numbers[currentTime.Hour/10], 39, 80, GxEPD_BLACK); //first digit
    display.drawBitmap(110, 10, numbers[currentTime.Hour%10], 39, 80, GxEPD_BLACK); //second digit
    

    //Minute
    display.drawBitmap(50, 110, numbers[currentTime.Minute/10], 39, 80, GxEPD_BLACK); //first digit
    display.drawBitmap(110, 110, numbers[currentTime.Minute%10], 39, 80, GxEPD_BLACK); //second digit
}

#ifndef DARKMODE
#define DARKMODE false
#endif

const uint8_t BATTERY_SEGMENT_WIDTH = 7;
const uint8_t BATTERY_SEGMENT_HEIGHT = 11;
const uint8_t BATTERY_SEGMENT_SPACING = 9;
const uint8_t WEATHER_ICON_WIDTH = 48;
const uint8_t WEATHER_ICON_HEIGHT = 32;

void WatchyMulti::drawWatchFace7SEG(){
    display.fillScreen(DARKMODE ? GxEPD_BLACK : GxEPD_WHITE);
    display.setTextColor(DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    drawTime7SEG();
    drawDate7SEG();
    drawSteps7SEG();
 
    drawWeather7SEG();
    drawBattery7SEG();
    display.drawBitmap(120, 77, WIFI_CONFIGURED ? wifi : wifioff, 26, 18, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    if(BLE_CONFIGURED){
        display.drawBitmap(100, 75, bluetooth, 13, 21, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    }
}

void WatchyMulti::drawTime7SEG(){
    display.setFont(&DSEG7_Classic_Bold_53);
    display.setCursor(5, 53+5);
    int displayHour;
    if(HOUR_12_24==12){
      displayHour = ((currentTime.Hour+11)%12)+1;
    } else {
      displayHour = currentTime.Hour;
    }
    if(displayHour < 10){
      display.print("0");
    }
    display.print(displayHour);
    display.print(":");
    if(currentTime.Minute < 10){
        display.print("0");
    }
    display.println(currentTime.Minute);
}

void WatchyMulti::drawDate7SEG(){
    display.setFont(&Seven_Segment10pt7b);

    int16_t  x1, y1;
    uint16_t w, h;

    String dayOfWeek = dayStr(currentTime.Wday);
    display.getTextBounds(dayOfWeek, 5, 85, &x1, &y1, &w, &h);
    if(currentTime.Wday == 4){
        w = w - 5;
    }
    display.setCursor(85 - w, 85);
    display.println(dayOfWeek);

    String month = monthShortStr(currentTime.Month);
    display.getTextBounds(month, 60, 110, &x1, &y1, &w, &h);
    display.setCursor(85 - w, 110);
    display.println(month);

    display.setFont(&DSEG7_Classic_Bold_25);
    display.setCursor(5, 120);
    if(currentTime.Day < 10){
      display.print("0");
    }
    display.println(currentTime.Day);
    display.setCursor(5, 150);
    display.println(tmYearToCalendar(currentTime.Year));// offset from 1970, since year is stored in uint8_t
}
void WatchyMulti::drawSteps7SEG(){
     // reset step counter at midnight
    if (currentTime.Hour == 0 && currentTime.Minute == 0){
      sensor.resetStepCounter();
    }
    uint32_t stepCount = sensor.getCounter();
    display.drawBitmap(10, 165, steps, 19, 23, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    display.setCursor(35, 190);
    display.println(stepCount);
}

void WatchyMulti::drawBattery7SEG(){
    display.drawBitmap(154, 73, battery, 37, 21, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    display.fillRect(159, 78, 27, BATTERY_SEGMENT_HEIGHT, DARKMODE ? GxEPD_BLACK : GxEPD_WHITE);//clear battery segments
    int8_t batteryLevel = 0;
    float VBAT = getBatteryVoltage();
    if(VBAT > 4.1){
        batteryLevel = 3;
    }
    else if(VBAT > 3.95 && VBAT <= 4.1){
        batteryLevel = 2;
    }
    else if(VBAT > 3.80 && VBAT <= 3.95){
        batteryLevel = 1;
    }
    else if(VBAT <= 3.80){
      batteryLevel = 0;
    }

    for(int8_t batterySegments = 0; batterySegments < batteryLevel; batterySegments++){
        display.fillRect(159 + (batterySegments * BATTERY_SEGMENT_SPACING), 78, BATTERY_SEGMENT_WIDTH, BATTERY_SEGMENT_HEIGHT, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    }
}

void WatchyMulti::drawWeather7SEG(){
    weatherData currentWeather = getWeatherData();
    int16_t weatherConditionCode = currentWeather.weatherConditionCode;

    if( WIFI_CONFIGURED ) {
        int8_t temperature = currentWeather.temperature;

        display.setFont(&DSEG7_Classic_Regular_39);
        int16_t  x1, y1;
        uint16_t w, h;
        display.getTextBounds(String(temperature), 0, 0, &x1, &y1, &w, &h);
        if(159 - w - x1 > 87){
            display.setCursor(159 - w - x1, 150);
        }else{
            display.setFont(&DSEG7_Classic_Bold_25);
            display.getTextBounds(String(temperature), 0, 0, &x1, &y1, &w, &h);
            display.setCursor(159 - w - x1, 136);
        }
        display.println(temperature);
        display.drawBitmap(165, 110, currentWeather.isMetric ? celsius : fahrenheit, 26, 20, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
    }

    const unsigned char* weatherIcon;

    //https://openweathermap.org/weather-conditions
    if(weatherConditionCode > 801){//Cloudy
      weatherIcon = cloudy;
    }else if(weatherConditionCode == 801){//Few Clouds
      weatherIcon = cloudsun;
    }else if(weatherConditionCode == 800){//Clear
      weatherIcon = sunny;
    }else if(weatherConditionCode >=700){//Atmosphere
      weatherIcon = atmosphere;
    }else if(weatherConditionCode >=600){//Snow
      weatherIcon = snow;
    }else if(weatherConditionCode >=500){//Rain
      weatherIcon = rain;
    }else if(weatherConditionCode >=300){//Drizzle
      weatherIcon = drizzle;
    }else if(weatherConditionCode >=200){//Thunderstorm
      weatherIcon = thunderstorm;
    } else {
      return;
    }
    display.drawBitmap(145, 158, weatherIcon, WEATHER_ICON_WIDTH, WEATHER_ICON_HEIGHT, DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
}


// Line-For-Watchy

// ---------------------- Line-for-Watchy 0.2 -------------------------------
/////////////////////////////////////////////////////////////////////////////
// Analog Watchface for Watchy E-Paper Watch
// Design inspired by Line Watch Face for Google Wear OS by Seahorsepip
// https://play.google.com/store/apps/details?id=com.seapip.thomas.line_watchface
//
// Copyright (c) 2021 Thomas Schaefer
// thomas@blinky.berlin
/////////////////////////////////////////////////////////////////////////////

void WatchyMulti::drawWatchFaceLine() {
  uint8_t myHour;
  uint8_t myMinute;
  uint8_t radius;
  float angle_hourScale;
  float angle_minuteScale;
  float angle_hourHand;
  float angle_minuteHand;
  float pi = 3.1415926535897932384626433832795;
  
  display.fillScreen(!DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
  display.setTextColor(!DARKMODE ? GxEPD_BLACK : GxEPD_WHITE);
  display.setFont(&DSEG7_Classic_Regular_39);
  
  // draw outer circle
  //display.drawCircle(100, 100, 99, !DARKMODE ? GxEPD_BLACK : GxEPD_WHITE);
  //display.drawCircle(100, 100, 98, !DARKMODE ? GxEPD_BLACK : GxEPD_WHITE);

  myHour   = currentTime.Hour > 12 ? currentTime.Hour - 12 : currentTime.Hour;
  myMinute = currentTime.Minute;
  
  angle_hourScale   = 2*pi/12;
  angle_minuteScale = 2*pi/60;
  angle_hourHand    = angle_hourScale*(myHour-3)+2*pi/720*myMinute;
  angle_minuteHand  = angle_minuteScale*(myMinute-15);
  
  // draw minute scale
  radius = 98;
  for (uint8_t i=0; i<60; i++) {
    display.drawLine(100, 100, 100+radius*cos(angle_minuteScale*i), 100+radius*sin(angle_minuteScale*i), !DARKMODE ? GxEPD_BLACK : GxEPD_WHITE);
  }
  display.fillCircle(100, 100, 93, !DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);

  // draw hour scale
  radius = 98;
  for (uint8_t i=0; i<12; i++) {
    drawHandLine(radius, angle_hourScale*i, !DARKMODE);
  }
  display.fillCircle(100, 100, 88, !DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);

  // draw hour hand
  //radius = 45;
  //drawHand (radius, angle_hourHand, !DARKMODE);

  // draw minute hand
  radius = 98;
  drawHandLine(radius, angle_minuteHand, !DARKMODE);

  // draw center point
  display.fillCircle(100, 100, 45, !DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);

  // draw helping lines
  //display.drawCircle(100, 100, 45, !DARKMODE ? GxEPD_BLACK : GxEPD_WHITE);
  //display.drawCircle(100, 100, 25, !DARKMODE ? GxEPD_BLACK : GxEPD_WHITE);

  // positioning of hour display for DSEG7_Classic_Regular_39 font
  if (currentTime.Hour > 9 && currentTime.Hour < 20) {
    display.setCursor(58, 120);
  } else {
    display.setCursor(68, 120);
  }
  
  // display hour (with a leading zero, if necessary)
  if(currentTime.Hour < 10){
      display.print("0");
  }
  display.print(currentTime.Hour);

  // draw helping lines
  //display.drawLine(100, 0, 100, 200, !DARKMODE ? GxEPD_BLACK : GxEPD_WHITE);
    //display.drawLine(0, 100, 200, 100, !DARKMODE ? GxEPD_BLACK : GxEPD_WHITE);
}

// helper function for hands drawing
void WatchyMulti::drawHandLine(uint8_t _radius_, float _angle_, bool _light_) {
  float x = _radius_*cos(_angle_);
  float y = _radius_*sin(_angle_);
  display.drawLine(99, 99, 99+x, 99+y, _light_ ? GxEPD_BLACK : GxEPD_WHITE);
  display.drawLine(99, 100, 99+x, 100+y, _light_ ? GxEPD_BLACK : GxEPD_WHITE);
  display.drawLine(99, 101, 99+x, 101+y, _light_ ? GxEPD_BLACK : GxEPD_WHITE);
  display.drawLine(100, 99, 100+x, 99+y, _light_ ? GxEPD_BLACK : GxEPD_WHITE);
  display.drawLine(100, 100, 100+x, 100+y, _light_ ? GxEPD_BLACK : GxEPD_WHITE);
  display.drawLine(100, 101, 100+x, 101+y, _light_ ? GxEPD_BLACK : GxEPD_WHITE);
  display.drawLine(101, 99, 101+x, 99+y, _light_ ? GxEPD_BLACK : GxEPD_WHITE);
  display.drawLine(101, 100, 101+x, 100+y, _light_ ? GxEPD_BLACK : GxEPD_WHITE);
  display.drawLine(101, 101, 101+x, 101+y, _light_ ? GxEPD_BLACK : GxEPD_WHITE);
}


// Bahn

void WatchyMulti::drawWatchFaceBahn() {
  int16_t  x1, y1, lasty;
  uint16_t w, h;
  String textstring;

  // ** UPDATE **
  //resets step counter at midnight everyday
  if(currentTime.Hour == 00 && currentTime.Minute == 00) {
    sensor.resetStepCounter();
  }

  // ** DRAW **
  
  //drawbg
  display.fillScreen(!DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
  display.fillRoundRect(2,2,196,196,8, !DARKMODE ? GxEPD_BLACK : GxEPD_WHITE);
  display.fillRoundRect(6,6,188,188,5, !DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
  
  display.setFont(&DIN_1451_Engschrift_Regular64pt7b);
  display.setTextColor(!DARKMODE ? GxEPD_BLACK : GxEPD_WHITE);
  display.setTextWrap(false);

  //draw hours
  textstring = currentTime.Hour;
  display.getTextBounds(textstring, 0, 0, &x1, &y1, &w, &h);
  display.setCursor(183-w, 100-5);
  display.print(textstring);
  
  //draw minutes
  if (currentTime.Minute < 10) {
    textstring = "0";
  } else {
    textstring = "";
  }
  textstring += currentTime.Minute;
  display.getTextBounds(textstring, 0, 0, &x1, &y1, &w, &h);
  display.setCursor(183-w, 100+3+h);
  display.print(textstring);

  // draw battery
  display.fillRoundRect(16,16,34,12,4, !DARKMODE ? GxEPD_BLACK : GxEPD_WHITE);
  display.fillRoundRect(49,20,3,4,2, !DARKMODE ? GxEPD_BLACK : GxEPD_WHITE);
  display.fillRoundRect(18,18,30,8,3, !DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
  float batt = (getBatteryVoltage()-3.3)/0.9;
  if (batt > 0) {
   display.fillRoundRect(20,20,26*batt,4,2, !DARKMODE ? GxEPD_BLACK : GxEPD_WHITE);
  }									

  display.setFont(&DIN_1451_Engschrift_Regular12pt7b);
  lasty = 200 - 16;

  //draw steps
  textstring = sensor.getCounter();
  textstring += " steps";
  display.getTextBounds(textstring, 0, 0, &x1, &y1, &w, &h);
  display.fillRoundRect(16,lasty-h-2,w + 7,h+4,2, !DARKMODE ? GxEPD_BLACK : GxEPD_WHITE);
  display.setCursor(19, lasty-3);
  display.setTextColor( !DARKMODE ? GxEPD_WHITE : GxEPD_BLACK);
  display.print(textstring);
  display.setTextColor( !DARKMODE ? GxEPD_BLACK : GxEPD_WHITE);
  lasty += -8-h;

  // draw year
  textstring = currentTime.Year + 1970;
  display.getTextBounds(textstring, 0, 0, &x1, &y1, &w, &h);
  display.setCursor(16, lasty);
  display.print(textstring);
  lasty += -20;

  // draw date
  textstring = monthShortStr(currentTime.Month);
  textstring += " ";
  textstring += currentTime.Day;
  display.getTextBounds(textstring, 0, 0, &x1, &y1, &w, &h);
  display.setCursor(16, lasty);
  display.print(textstring);
  lasty += -20;
  
  // draw day
  textstring = dayStr(currentTime.Wday);
  display.getTextBounds(textstring, 0, 0, &x1, &y1, &w, &h);
  display.setCursor(16, lasty);
  display.print(textstring);
  lasty += -40;

  // weather things
  if( !WIFI_CONFIGURED ) {
    return;
  }
  weatherData currentWeather = getWeatherData();
  int8_t temperature = currentWeather.temperature;
  int16_t weatherConditionCode = currentWeather.weatherConditionCode;

  // draw weather state
  if (weatherConditionCode >= 801) {
    textstring = "Cloudy";
  } else if (weatherConditionCode == 800) {
    textstring = "Clear";
  } else if (weatherConditionCode == 781) {
    textstring = "Tornado";
  } else if (weatherConditionCode == 771) {
    textstring = "Squall";
  } else if (weatherConditionCode == 762) {
    textstring = "Ash";
  } else if (weatherConditionCode == 761 || weatherConditionCode == 731) {
    textstring = "Dust";
  } else if (weatherConditionCode == 751) {
    textstring = "Sand";
  } else if (weatherConditionCode == 741) {
    textstring = "Fog";
  } else if (weatherConditionCode == 721) {
    textstring = "Haze";
  } else if (weatherConditionCode == 711) {
    textstring = "Smoke";
  } else if (weatherConditionCode == 701) {
    textstring = "Mist";
  } else if (weatherConditionCode >= 600) {
    textstring = "Snow";
  } else if (weatherConditionCode >= 500) {
    textstring = "Rain";
  } else if (weatherConditionCode >= 300) {
    textstring = "Drizzle";
  } else if (weatherConditionCode >= 200) {
    textstring = "Thunderstorm";
  } else {
    textstring = "";
  }
  display.getTextBounds(textstring, 0, 0, &x1, &y1, &w, &h);
  display.setCursor(16, lasty);
  display.print(textstring);
  lasty += -20;

  // draw temperature
  textstring = temperature;
  textstring += TEMP_UNIT_METRIC ? "C" : "F";
  display.getTextBounds(textstring, 0, 0, &x1, &y1, &w, &h);
  display.setCursor(16, lasty);
  display.print(textstring);
}
