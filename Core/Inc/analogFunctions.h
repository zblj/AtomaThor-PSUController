#ifndef ANALOGFUNCTIONS_H
#define ANALOGFUNCTIONS_H

#include <stdint.h>

typedef struct{
    // Actual reference voltage
    float Vref;
    // ADC gain & offset
    float Again[8];
    float Aoffset[8];
    // DAC gain & offset
    float Dgain[2];
    float Doffset[2];
    // Thermometer gain & offset
    float Tgain;
    float Toffset;
} AnalogTrimmingValues;

// Internal temperature sensor definitions
// #define TS_CAL1_TEMP    30.0f      // °C
// #define TS_CAL2_TEMP    130.0f     // °C
// #define TS_CAL1_ADDR    ((uint16_t *)0x1FFF75A8)
// #define TS_CAL2_ADDR    ((uint16_t *)0x1FFF75CA)
// #define VREFINT_CAL_ADDR    ((uint16_t *)0x1FFF75AA)
extern uint16_t ADCmeasurements[8]; // A placeholder for storing ADC readings
extern AnalogTrimmingValues atp;

void ADC1_COMP_IRQHandler(void);
void setupADCs(void);

#endif