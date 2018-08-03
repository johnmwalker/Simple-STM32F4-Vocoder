#ifndef PITCHDETECTION_H
#define PITCHDETECTION_H

#include <string.h>
#include "dywapitchtrack.h"
#include "Yin.h"
#include "stdio.h"
#include "stdint.h"
#include "kiss_fftr.h"
#include "main.h"

void initPitchDetection(char type[], int samplingRate, int numSamples);
double detectPitch(audioDataType buffer[]);
float maxValue(float myArray[], int size);


#endif

