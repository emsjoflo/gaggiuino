/* 09:32 15/03/2023 - change triggering comment */
#ifndef Solenoid_H
#define Solenoid_H

#include <Arduino.h>
#include "sensors_state.h"

#define ZC_MODE    FALLING

constexpr uint8_t Solenoid_RANGE = 100;

void SolenoidInit(const int powerLineFrequency, const float SolenoidFlowAtZero);
void setSolenoidPressure(const float targetPressure, const float flowRestriction, const SensorState &currentState);
void setSolenoidOff(void);
void setSolenoidFullOn(void);
void setSolenoidToRawValue(const uint8_t val);
long  getAndResetClickCounter2(void);
int getCPS2(void);
void SolenoidPhaseShift(void);
void SolenoidStopAfter(const uint8_t val);
float getSolenoidFlow(const float cps, const float pressure);
float getSolenoidFlowPerClick(const float pressure);
float getClicksPerSecondForFlow2(const float flow, const float pressure);
void setSolenoidFlow(const float targetFlow, const float pressureRestriction, const SensorState &currentState);
#endif
