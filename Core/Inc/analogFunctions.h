#ifndef ANALOGFUNCTIONS_H
#define ANALOGFUNCTIONS_H

#include <stdint.h>

extern uint16_t ADCmeasurements[8]; // A placeholder for storing ADC readings

// Internal temperature sensor definitions
// #define TS_CAL1_TEMP    30.0f      // °C
// #define TS_CAL2_TEMP    130.0f     // °C
// #define TS_CAL1_ADDR    ((uint16_t *)0x1FFF75A8)
// #define TS_CAL2_ADDR    ((uint16_t *)0x1FFF75CA)
// #define VREFINT_CAL_ADDR    ((uint16_t *)0x1FFF75AA)

void ADC1_COMP_IRQHandler(void);
void setupADCs(void);

#endif