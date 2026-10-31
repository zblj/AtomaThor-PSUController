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

extern uint16_t ADCmeasurements[8]; // A placeholder for storing ADC readings
extern AnalogTrimmingValues atp;

void ADC1_COMP_IRQHandler(void);
void setupADCs(void);
float getVoltage(uint8_t channel);
float getTemperature(uint16_t msmnt);

#endif