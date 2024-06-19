/* 09:32 15/03/2023 - change triggering comment */
#include "Solenoid.h"
#include "pindef.h"
#include "utils.h"
#include "peripherals.h"

void setSolenoidToRawValue(const uint8_t val) {
  if (millis() % 1000 > val) {
    setSteamValveRelayOn(); //H2
  } else{
    setSteamValveRelayOff(); //H2
  }
}