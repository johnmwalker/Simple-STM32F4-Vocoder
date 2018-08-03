/*
Vocoder Project for ENGR 331
John Walker and Andrew Whately
May 2018

The PSOLA.c file was edited from cpp to c. It is responsible for changing
the pitch of the input buffer.
*/
#ifndef PSOLA_H
#define PSOLA_H

#include "main.h"
#include <math.h>

void changePitch(int* input, int Fs, float inputPitch, float desiredPitch);
void bartlett(int* window, int length);


#endif

