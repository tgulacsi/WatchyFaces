#include "Watchy_Multi.h"
#include "BadForEye.h"
#include "Seven_Segment10pt7b.h"
#include "DSEG7_Classic_Regular_15.h"
#include "DSEG7_Classic_Bold_25.h"
#include "DSEG7_Classic_Regular_39.h"
#include "7_SEG_icons.h"

#define DARKMODE false
#define FACE_COUNT 2

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
