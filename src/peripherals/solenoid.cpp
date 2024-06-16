/* 09:32 15/03/2023 - change triggering comment */
#include "Solenoid.h"
#include "pindef.h"
#include <PSM.h>
#include "utils.h"
#include "internal_watchdog.h"
#include "peripherals.h"

//PSM Solenoid(zcPin, dimmerPin, Solenoid_RANGE, ZC_MODE, 1, 6);

float flowPerClickAtZeroBar2 = 0.27f;
int maxSolenoidClicksPerSecond = 25;
float fpc_multiplier2 = 1.2f;
//https://www.desmos.com/calculator/axyl70gjae  - blue curve
constexpr std::array<float, 7> pressureInefficiencyCoefficient {{
  0.045f,
  0.015f,
  0.0033f,
  0.000685f,
  0.000045f,
  0.009f,
  -0.0018f
}};

// Initialising some Solenoid specific specs, mainly:
// - max Solenoid clicks(dependant on region power grid spec)
// - Solenoid clicks at 0 pressure in the system
void SolenoidInit(const int powerLineFrequency, const float SolenoidFlowAtZero) {
  // Solenoid.freq = powerLineFrequency;
  maxSolenoidClicksPerSecond = powerLineFrequency;
  flowPerClickAtZeroBar2 = SolenoidFlowAtZero;
  fpc_multiplier2 = 60.f / (float)maxSolenoidClicksPerSecond;
}

// Function that returns the percentage of clicks the Solenoid makes in it's current phase
inline float getSolenoidPct(const float targetPressure, const float flowRestriction, const SensorState &currentState) {
  if (targetPressure == 0.f) {
      return 0.f;
  }

  float diff = targetPressure - currentState.smoothedPressure;
  float maxSolenoidPct = flowRestriction <= 0.f ? 1.f : getClicksPerSecondForFlow2(flowRestriction, currentState.smoothedPressure) / (float) maxSolenoidClicksPerSecond;
  float SolenoidPctToMaintainFlow = 1; //getClicksPerSecondForFlow2(currentState.smoothedSolenoidFlow, currentState.smoothedPressure) / (float) maxSolenoidClicksPerSecond;

  if (diff > 2.f) {
    return fminf(maxSolenoidPct, 0.25f + 0.2f * diff);
  }

  if (diff > 0.f) {
    return fminf(maxSolenoidPct, SolenoidPctToMaintainFlow * 0.95f + 0.1f + 0.2f * diff);
  }

  if (currentState.pressureChangeSpeed < 0) {
    return fminf(maxSolenoidPct, SolenoidPctToMaintainFlow * 0.2f);
  }

  return 0;
}

// Sets the Solenoid output based on a couple input params:
// - live system pressure
// - expected target
// - flow
// - pressure direction
void setSolenoidPressure(const float targetPressure, const float flowRestriction, const SensorState &currentState) {
  float SolenoidPct = getSolenoidPct(targetPressure, flowRestriction, currentState);
 //setSolenoidToRawValue((uint8_t)(SolenoidPct * Solenoid_RANGE));

        if (millis() % 1000 > SolenoidPct * Solenoid_RANGE) {
        setSteamValveRelayOn(); //H2
      } else{
        setSteamValveRelayOff(); //H2
      }

}

void setSolenoidOff(void) {
 // Solenoid.set(0);
}

void setSolenoidFullOn(void) {
 // Solenoid.set(Solenoid_RANGE);
}

void setSolenoidToRawValue(const uint8_t val) {
 // Solenoid.set(val);
}

void SolenoidStopAfter(const uint8_t val) {
 // Solenoid.stopAfter(val);
}

long getAndResetClickCounter2(void) {
  //long counter = Solenoid.getCounter();
  //Solenoid.resetCounter();
  //return counter;
}

int getCPS2(void) {
  watchdogReload();
  //unsigned int cps = Solenoid.cps();
  watchdogReload();
  //if (cps > 80u) {
  //  Solenoid.setDivider(2);
  //  Solenoid.initTimer(cps > 110u ? 5000u : 6000u, TIM9);
  //}
 // else {
   // Solenoid.initTimer(cps > 55u ? 5000u : 6000u, TIM9);
  //}
 // return cps;
}

void SolenoidPhaseShift(void) {
 // Solenoid.shiftDividerCounter();
}

// Models the flow per click, follows a compromise between the schematic and recorded findings
// plotted: https://www.desmos.com/calculator/eqynzclagu
float getSolenoidFlowPerClick(const float pressure) {
  float fpc = 0.f;
  fpc = (pressureInefficiencyCoefficient[5] / pressure + pressureInefficiencyCoefficient[6]) * ( -pressure * pressure ) + ( flowPerClickAtZeroBar2 - pressureInefficiencyCoefficient[0]) - (pressureInefficiencyCoefficient[1] + (pressureInefficiencyCoefficient[2] - (pressureInefficiencyCoefficient[3] - pressureInefficiencyCoefficient[4] * pressure) * pressure) * pressure) * pressure;
  return fpc * fpc_multiplier2;
}

// Follows the schematic from https://www.cemegroup.com/solenoid-Solenoid/e5-60 modified to per-click
float getSolenoidFlow(const float cps, const float pressure) {
  return cps * getSolenoidFlowPerClick(pressure);
}

// Currently there is no compensation for pressure measured at the puck, resulting in incorrect estimates
float getClicksPerSecondForFlow2(const float flow, const float pressure) {
  if (flow == 0.f) return 0;
  float flowPerClick = getSolenoidFlowPerClick(pressure);
  float cps = flow / flowPerClick;
  return fminf(cps, (float)maxSolenoidClicksPerSecond);
}

// Calculates Solenoid percentage for the requested flow and updates the Solenoid raw value
void setSolenoidFlow(const float targetFlow, const float pressureRestriction, const SensorState &currentState) {
  // If a pressure restriction exists then the we go into pressure profile with a flowRestriction
  // which is equivalent but will achieve smoother pressure management
  //if (pressureRestriction > 0.f && currentState.smoothedPressure > pressureRestriction * 0.5f) {
    setSolenoidPressure(pressureRestriction, targetFlow, currentState);
  //}
  //else {
  //  float SolenoidPct = getClicksPerSecondForFlow2(targetFlow, currentState.smoothedPressure) / (float)maxSolenoidClicksPerSecond;
  //  setSolenoidToRawValue(SolenoidPct * Solenoid_RANGE);
  //}
}
