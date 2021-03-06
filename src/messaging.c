#include "weather.h"
#include "messaging.h"
#include "settings.h"

void messaging_requestNewWeatherData() {
  // just send an empty message for now
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  dict_write_uint32(iter, 0, 0);
  app_message_outbox_send();
}


void messaging_init(void (*processed_callback)(void)) {
  // register my custom callback
  message_processed_callback = processed_callback;

  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);

  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Watch messaging is started!");
  app_message_register_inbox_received(inbox_received_callback);
}

void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // does this message contain weather information?
  Tuple *weatherTemp_tuple = dict_find(iterator, KEY_TEMPERATURE);
  Tuple *weatherConditions_tuple = dict_find(iterator, KEY_CONDITION_CODE);
  Tuple *weatherIsNight_tuple = dict_find(iterator, KEY_USE_NIGHT_ICON);

  if(weatherTemp_tuple != NULL && weatherConditions_tuple != NULL && weatherIsNight_tuple != NULL) {
    bool isNight = (bool)weatherIsNight_tuple->value->int32;

    // now set the weather conditions properly
    Weather_weatherInfo.currentTemp = (int)weatherTemp_tuple->value->int32;
    Weather_setCondition(weatherConditions_tuple->value->int32, isNight);
  }

  // does this message contain new config information?
  Tuple *timeColor_tuple = dict_find(iterator, KEY_SETTING_COLOR_TIME);
  Tuple *bgColor_tuple = dict_find(iterator, KEY_SETTING_COLOR_BG);
  Tuple *sidebarColor_tuple = dict_find(iterator, KEY_SETTING_COLOR_SIDEBAR);
  Tuple *sidebarPos_tuple = dict_find(iterator, KEY_SETTING_SIDEBAR_RIGHT);
  Tuple *sidebarTextColor_tuple = dict_find(iterator, KEY_SETTING_SIDEBAR_TEXT_COLOR);
  Tuple *useMetric_tuple = dict_find(iterator, KEY_SETTING_USE_METRIC);
  Tuple *btVibe_tuple = dict_find(iterator, KEY_SETTING_BT_VIBE);
  Tuple *language_tuple = dict_find(iterator, KEY_SETTING_LANGUAGE_ID);
  Tuple *batteryMeter_tuple = dict_find(iterator, KEY_SETTING_SHOW_BATTERY_METER);
  Tuple *leadingZero_tuple = dict_find(iterator, KEY_SETTING_SHOW_LEADING_ZERO);
  Tuple *batteryPct_tuple = dict_find(iterator, KEY_SETTING_SHOW_BATTERY_PCT);
  Tuple *disableWeather_tuple = dict_find(iterator, KEY_SETTING_DISABLE_WEATHER);
  Tuple *clockFont_tuple = dict_find(iterator, KEY_SETTING_CLOCK_FONT_ID);
  Tuple *hourlyVibe_tuple = dict_find(iterator, KEY_SETTING_HOURLY_VIBE);

  if(timeColor_tuple != NULL) {
    #ifdef PBL_COLOR
      globalSettings.timeColor = GColorFromHEX(timeColor_tuple->value->int32);
    #else
      globalSettings.timeColor = (timeColor_tuple->value->int32 == 0) ? GColorBlack : GColorWhite;
    #endif
  }

  if(bgColor_tuple != NULL) {
    #ifdef PBL_COLOR
      globalSettings.timeBgColor = GColorFromHEX(bgColor_tuple->value->int32);
    #else
      globalSettings.timeBgColor = (bgColor_tuple->value->int32 == 0) ? GColorBlack : GColorWhite;
    #endif
  }

  if(sidebarColor_tuple != NULL) {
    #ifdef PBL_COLOR
      globalSettings.sidebarColor = GColorFromHEX(sidebarColor_tuple->value->int32);
    #else
      globalSettings.sidebarColor = (sidebarColor_tuple->value->int32 == 0) ? GColorBlack : GColorWhite;
    #endif
  }

  if(sidebarTextColor_tuple != NULL) {
    #ifdef PBL_COLOR
      globalSettings.sidebarTextColor = GColorFromHEX(sidebarTextColor_tuple->value->int32);
    #else
      globalSettings.sidebarTextColor = (sidebarTextColor_tuple->value->int32 == 0) ? GColorBlack : GColorWhite;
    #endif
  }

  if(sidebarPos_tuple != NULL) {
    globalSettings.sidebarOnRight = (bool)sidebarPos_tuple->value->int8;
  }

  if(useMetric_tuple != NULL) {
    globalSettings.useMetric = (bool)useMetric_tuple->value->int8;
  }

  if(btVibe_tuple != NULL) {
    globalSettings.btVibe = (bool)btVibe_tuple->value->int8;
  }

  if(batteryMeter_tuple != NULL) {
    globalSettings.showBatteryLevel = (bool)batteryMeter_tuple->value->int8;
  }

  if(leadingZero_tuple != NULL) {
    Settings_showLeadingZero = (bool)leadingZero_tuple->value->int8;
  }

  if(batteryPct_tuple != NULL) {
    Settings_showBatteryPct = (bool)batteryPct_tuple->value->int8;
  }

  if(disableWeather_tuple != NULL) {
    Settings_disableWeather = (bool)disableWeather_tuple->value->int8;
  }

  if(clockFont_tuple != NULL) {
    Settings_clockFontId = (bool)clockFont_tuple->value->int8;
  }

  if(hourlyVibe_tuple != NULL) {
    Settings_hourlyVibe = hourlyVibe_tuple->value->int8;
  }

  if(language_tuple != NULL) {
    globalSettings.languageId = language_tuple->value->int8;
  }

  // notify the main screen, in case something changed
  message_processed_callback();
}

void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed! %d %d %d", reason, APP_MSG_SEND_TIMEOUT, APP_MSG_SEND_REJECTED);

}

void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}
