#ifndef ADC_H
#define ADC_H

#include "main.h"

void initAudioIn(uint16_t* buffer, uint32_t size);
void initAudioInGPIO(void);
void initAudioInTIM(void);
void initAudioInADC(void);
void initAudioInNVIC(void);
void initAudioInDMA(uint16_t* buffer, uint32_t size);

#endif

