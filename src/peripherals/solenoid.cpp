/* 09:32 15/03/2023 - change triggering comment */
#include "Solenoid.h"
#include "pump.h"
#include "pindef.h"
#include "utils.h"
#include "peripherals.h"

void setSolenoidToRawValue(const uint8_t val) {
 setPumpToRawValue(100);
  if (millis() % 1000 > val*2.5f) {
    setSteamValveRelayOff(); //H2
  } else{
    setSteamValveRelayOn(); //H2
  }
}
