#include "analogFunctions.h"
#include "main.h"
#include "stm32g051xx.h"
#include "stm32g0xx_hal.h"
#include "supportingFunctions.h"
#include "stm32g0xx_ll_adc.h"
#include <math.h>

uint16_t ADCmeasurements[8]; // A placeholder for storing ADC readings

AnalogTrimmingValues atp =
{
    .Vref = 2.5f,
    .Again       = { 1,1,1,1,1,1,1,1},
    .Aoffset     = { 0,0,0,0,0,0,0,0},
    .Dgain       = { 1,1},
    .Doffset     = { 0,0},
    .Tgain       = 1,
    .Toffset     = 0,
};

void ADC1_COMP_IRQHandler(void){
    static uint8_t chIdx = 0;
    uint16_t isr = ADC1->ISR;
    ADC1->ISR = ADC_ISR_EOS;    // Clear EOS flag, (do it ASAP to prevent trigger stacking when debugging)

    // Store the measurement to the appropriate slot in the array
    ADCmeasurements[chIdx%8] = ADC1->DR;

    // If the current sample is the last in sequence, set idx to 0 as the next one is the first in sequence, otherwise incremetn the index
    if (isr & ADC_ISR_EOS){
        chIdx=0;
        return;
    }
    chIdx++;
}

void setupADCs(void){
    // Perform self calibration; should be done at startup and after significant temperature shange (eg. 10 °C)
    HAL_ADCEx_Calibration_Start(&hadc1);
  
    // Set ADC sample time
    ADC1->SMPR =
        (ADC_SAMPLETIME_160CYCLES_5 << ADC_SMPR_SMP1_Pos) |    // SMP1 set sampling time, will be used for all cahnnels
        (ADC_SAMPLETIME_160CYCLES_5 << ADC_SMPR_SMP2_Pos) | // Excpet for those using a longer sampling time, listed in next line
        (1U << ADC_SMPR_SMPSEL12_Pos);                      // Required at least for internal temperature sensor
  
    // Internal temperature sensor calibration parameters
    // #define TS_CAL1_TEMP    30.0f      // °C
    // #define TS_CAL2_TEMP    130.0f     // °C
    // #define TS_CAL1_ADDR    ((uint16_t *)0x1FFF75A8)
    // #define TS_CAL2_ADDR    ((uint16_t *)0x1FFF75CA)
    // #define VREFINT_CAL_ADDR    ((uint16_t *)0x1FFF75AA)
    // const float VrefCal = *VREFINT_CAL_ADDR *(3./(1<<12));  // Factory calibration of internal Vref [V]. Not really sure if it is helpful for anything.
    
    // Set sequence for automatic measurement
    ADC1->CHSELR = 
         0 << (4*0) |   // Ch0 = Vsns1
         1 << (4*1) |   // Ch1 = Vsns2
         2 << (4*2) |   // Ch2 = Psns2
         3 << (4*3) |   // Ch3 = id2
         6 << (4*4) |   // Ch6 = Psns1
         7 << (4*5) |   // Ch7 = id1
        12 << (4*6) |   // Ch12 = internal thermometer
        15 << (4*7);    // 0xf = end of sequence

    // Enable continuous conversions
    ADC1->CFGR1 |= ADC_CFGR1_CONT;

    // enable ADC interrupts in NVIC and set priority
    HAL_NVIC_SetPriority(ADC1_COMP_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(ADC1_COMP_IRQn);

    // enable ADC interrupts as well
    ADC1->IER = ADC_IER_EOCIE;  // Interrupt on conversion complete

    // Start the ADC
    ADC1->CR |= ADC_CR_ADEN | ADC_CR_ADSTART;
}

float getVoltage(uint8_t channel){
    if (channel >= sizeof(ADCmeasurements)/sizeof(ADCmeasurements[0])){
        return NAN;
    }
    float Vsns = (ADCmeasurements[channel] * atp.Vref / (1<<16)) * atp.Again[channel] + atp.Aoffset[channel];
    return Vsns;
}

float getTemperature(uint16_t msmnt){
    float temperature, Vsns;
    
    Vsns = msmnt*(atp.Vref/(1<<16));
    temperature = (Vsns - (*TEMPSENSOR_CAL1_ADDR * (TEMPSENSOR_CAL_VREFANALOG / 1000.f / 4096.f))) *
    (TEMPSENSOR_CAL2_TEMP - TEMPSENSOR_CAL1_TEMP) / 
    ((*TEMPSENSOR_CAL2_ADDR - *TEMPSENSOR_CAL1_ADDR) * (TEMPSENSOR_CAL_VREFANALOG / 1000.f / 4096.f)) + 
    TEMPSENSOR_CAL1_TEMP;

    // Apply trimming correction
    temperature = temperature * atp.Tgain + atp.Toffset;

    return temperature;
}