/*
Vocoder Project for ENGR 331
John Walker and Andrew Whately
May 2018

This file is responsible for handling headphone output and 
all related processing. The application spends most of its
time in headphone.c.
*/
#ifndef __HEADPHONE_H
#define __HEADPHONE_H

#include "main.h"

/*
 * Simple reinitializes the ADC interrupt everytime this gets called. Not sure
 * why the reinitialization is necessary. The ADC only converts a single buffer
 * without reinitialization.
 */
void ADCCallback(void);

/*
 * This function is called by the wav recorder (i.e. microphone sampler) when PCM samples from the
 * microphone are ready. Here we store them into the circular microphone data buffer and check for
 * possibly clipped values (which we use to flash the red LED as a warning). Note that we assume
 * that the data will be removed from the buffer in time, so we do not check the mic_tail pointer
 * for buffer overrun.
 */
void WaveRecorderCallback (uint16_t *buffer, int num_samples);

// Called in main.c. This function initializes everything and begins the
// streaming process.
void WavePlayBack(void);

// Initializes the headphone jack and DAC
int WavePlayerInit(uint32_t AudioFreq);

/*
 * Callbacks implementation:
 * the callbacks prototypes are defined in the stm324xg_eval_audio_codec.h file
 * and their implementation should be done in the user code if they are needed.
 * Below some examples of callback implementations.
 */
 
/*
 * Calculates the remaining file size and new position of the pointer.
 * Called when the previous DMA playback buffer is completed. Here we simply
 * start playing the other buffer and signal the main loop that it can refill
 * the buffer we just played.
 */
void EVAL_AUDIO_TransferComplete_CallBack(uint32_t pBuffer, uint32_t Size);

/*
 * Manages the DMA Half Transfer complete interrupt.
 * Generally this interrupt routine is used to load the buffer when 
 * a streaming scheme is used: When first Half buffer is already transferred load 
 * the new data to the first half of buffer while DMA is transferring data from 
 * the second half. And when Transfer complete occurs, load the second half of 
 * the buffer while the DMA is transferring from the first half
 */
void EVAL_AUDIO_HalfTransfer_CallBack(uint32_t pBuffer, uint32_t Size);

void EVAL_AUDIO_Error_CallBack(void* pData);
uint16_t EVAL_AUDIO_GetSampleCallBack(void);

/* 
 * This version of the fill functions just echoes the microphone output back
 * to the headphones. This was created to verify clean microphone sampling
 * (ST's recording demo was full of gaps while the mass storage was writing)
 * and to make sure we could really do full-duplex audio (which I needed for
 * another project). Note that you can get a pretty long delay by increasing
 * the size of the buffers (and these don't count toward the 32K max). There
 * are some delay values that, when played back in headphones, make it almost
 * impossible to talk. Big fun!
 */
static void fill_buffer (int16_t *buffer, int num_samples);

// An attempt to allow printing to the LCD with a press of the userbutton.
void printPitch(void);

#endif
