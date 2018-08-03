/*
Vocoder Project for ENGR 331
John Walker and Andrew Whately
May 2018

See main.c for full description
*/

#ifndef __MAIN_H
#define __MAIN_H

// This was used in conjunction with the pitchDetection file to
// manage moving from ints to doubles depending on what type of
// data the selected pitch detection algorithm wanted.
//#define audioDataType double

#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "stm32f4_discovery_audio_codec.h"
#include <stdio.h>
#include "stm32f4xx_it.h"
#include "headphone.h"
#include "microphone.h"
#include "pdm_filter.h"
#include "LCD.h"
#include "delay.h"
#include "dywapitchtrack.h"
//#include "pitchDetection.h"


#endif
