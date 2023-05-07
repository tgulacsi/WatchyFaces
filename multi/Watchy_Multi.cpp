#include "Watchy_Multi.h"

#define DARKMODE false

uint32_t faceIdx = 0;

void WatchyMulti::handleButtonPress() {
  uint64_t wakeupBit = esp_sleep_get_ext1_wakeup_status();                                                                                             
  if(guiState == WATCHFACE_STATE && wakeupBit & BACK_BTN_MASK && wakeupBit & UP_BTN_MASK) {
    faceIdx = (faceIdx+1) % FACE_COUNT;
  }
}

void WatchyMulti::drawWatchFace() {
    faces[faceIdx].drawWatchFace();
}

