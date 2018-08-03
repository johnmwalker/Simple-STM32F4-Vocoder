/**
 *   @mainpage Time-Domain Pitch Synchronous Overlap and Add Method (TD-PSOLA)
 *   @author Terry Kong
 *   @date Mar. 9, 2015
 *
 ***	EDITED BY JOHN WALKER 8 MAY 2018. Adapted to C rather than CPP for use on an STM32F4 board.
 *
 *   \section desc_sec Description
 *   This is a crude implementation of the well known Time-Domain Pitch 
 *      Synchronous and Add method used for pitch shifting. This implementation
 *      does not deal with the phase inconsistencies that are introduced when 
 *      pitch correcting window by window. If your application deals with
 *      very short windows relative to your sampling frequency, there will be a
 *      significant "talking-into-a-fan" effect. My advice would be to use this
 *      on a significantly long piece of audio. 
 *
 *  @n This implementation uses Q15 arithmetic, which on the C5535 is represented
 *      as an int.
 *
 *  @n Generally this algorithm involves locating all the time epochs in the
 *      analysis window and mapping them to the synthesis epochs. This
 *      implementation does not locate the time epochs in the analysis window.
 *      This makes the algorithm less robust, and generally causes a degredation
 *      in the signal quality as well as introducting some distortion. 
 *
 *  @n Future work includes implementing a peak finding algorithm. However,
 *      depending on your application this might not be desirable since 
 *      peak finding algorithms add a significant overhead to this algorithm.
 *
 *  \section contents_sec Table of Contents
 *    PSOLA.cpp
 *
 *    PSOLA.h
 *
 *
 */

/**
 *  @file PSOLA.cpp
 *  @brief Source file for PSOLA algorithm
 *  @file PSOLA.h
 *  @brief Header file for PSOLA algorithm
 */

#include "PSOLA.h"

#define DEFAULT_BUFFER_SIZE 512
#define FIXED_BITS        16
#define FIXED_WBITS       0
#define FIXED_FBITS       15
#define Q15_RESOLUTION   (1 << (FIXED_FBITS - 1))
#define LARGEST_Q15_NUM   32767

// Some helper functions ==================

// Q15 multiplication
int Q15mult(int x, int y) {
    long temp = (long)x * (long)y;
    temp += Q15_RESOLUTION;
    return temp >> FIXED_FBITS;
}

// Q15 wrapped addition
int Q15addWrap(int x, int y) {
    return x + y;
}

// Q15 saturation addition
int Q15addSat(int x, int y) {
    long temp = (long)x+(long)y;
    if (temp > 0x7FFFFFFF) temp = 0x7FFFFFFF;
    if (temp < -1*0x7FFFFFFF) temp = -1*0x7FFFFFFF;
    return (int)temp;
}

int _bufferLen = DEFAULT_BUFFER_SIZE;

// allow for twice the room to deal with the case when the end of the buffer may not be sufficient
int _workingBuffer[2*DEFAULT_BUFFER_SIZE];
// allow for twice the room so we can move new data into this buffer
int _storageBuffer[2*DEFAULT_BUFFER_SIZE];
// allocates maximum size for window to avoid reinitialization cost
int _window[DEFAULT_BUFFER_SIZE];

/** ====================================================
 * @brief       Corrects the pitch of the input
 *
 * @details     Uses an implementation of the TD-PSOLA method for pitch shifting. To obtain better results, lower the sampling frequency or decrease the difference between the pitch of the input and the desired pitch. Calculates the output in-place. The input must be in Q15 format. It assumes that the pitch over the input is relatively constant.
 *
 * @param       input           Pointer to array of Q15 data (bufferLen long)
 * @param       Fs              Sampling frequency of data
 * @param       inputPitch      Estimated pitch of input
 * @param       desiredPitch    Desired pitch
 *
 * @todo        Fast Hann window implementation to replace Bartlett window
 *
 * ======================================================
 */
void changePitch(int* input, int Fs, float inputPitch, float desiredPitch) 
{
		//Make sure we won't get an infinite while loop
		if(inputPitch<100){inputPitch=100;}
		if(desiredPitch<100){desiredPitch=100;}
		
    // Move things into the storage buffer
    for (int i = 0; i < _bufferLen; i++) {
        //slide the past data into the front
        _storageBuffer[i] = _storageBuffer[i + _bufferLen];
        //load up next set of data
        _storageBuffer[i + _bufferLen] = input[i];
    }
    // Percent change of frequency
    float scalingFactor = 1 + (inputPitch - desiredPitch)/desiredPitch;
    // PSOLA constants
    int analysisShift = ceil(Fs/inputPitch);
    int analysisShiftHalfed = round(analysisShift/2);
    int synthesisShift = round(analysisShift*scalingFactor);
    int analysisIndex = -1;
    int synthesisIndex = 0;
    int analysisBlockStart;
    int analysisBlockEnd;
    int synthesisBlockEnd;
    int analysisLimit = _bufferLen - analysisShift - 1;
    // Window declaration
    int winLength = analysisShift + analysisShiftHalfed + 1;
    int windowIndex;
    bartlett(_window,winLength);
    // PSOLA Algorithm
    while (analysisIndex < analysisLimit) {
        // Analysis blocks are two pitch periods long
        analysisBlockStart = (analysisIndex + 1) - analysisShiftHalfed;
        if (analysisBlockStart < 0) {
            analysisBlockStart = 0;
        }
        analysisBlockEnd = analysisBlockStart + analysisShift + analysisShiftHalfed;
        if (analysisBlockEnd > _bufferLen - 1) {
            analysisBlockEnd = _bufferLen - 1;
        }
        // Overlap and add
        synthesisBlockEnd = synthesisIndex + analysisBlockEnd - analysisBlockStart;
        int inputIndex = analysisBlockStart;
        int windowIndex = 0;
        for (int j = synthesisIndex; j <= synthesisBlockEnd; j++) {
            _workingBuffer[j] = Q15addWrap(_workingBuffer[j], Q15mult(input[inputIndex],_window[windowIndex]) );
            inputIndex++;
            windowIndex++;
        }
        // Update pointers
        analysisIndex += analysisShift;
        synthesisIndex += synthesisShift;
    }
    // Write back to input
    for (int i = 0; i < _bufferLen; i++) {
        input[i] = _workingBuffer[i];
        // clean out the buffer
        _workingBuffer[i] = 0;
    }
}

/** ====================================================
 * @brief       Computes a bartlett window
 *
 * @details     Computes a bartlett window in-place with Q15 coefficients quickly
 *
 * @param       window           Pointer to array of Q15 data (bufferLen long)
 * @param       length           Length of the window
 *
 * @todo        More accurate implementation of bartlett window
 *
 * @todo        Fast Hann window implementation to replace Bartlett window
 *
 * ======================================================
 */
void bartlett(int* window, int length) 
{
    if (length < 1) return;
    if (length == 1) {
        window[0] = 1;
        return;
    }
    
    int N = length - 1;
    int middle = N >> 1;
    int slope = round( ((float)(1<<(FIXED_FBITS-1)))/N*4 );
    if (N%2 == 0) {
        // N even = L odd
        window[0] = 0;
        for (int i = 1; i <= middle; i++) {
            window[i] = window[i-1] + slope;
        }
        for (int i = middle+1; i <= N; i++) {
            window[i] = window[N - i];
        }
        // double check that the middle value is the maximum Q15 number
        window[middle] = LARGEST_Q15_NUM;
    } else {
        // N odd = L even
        window[0] = 0;
        for (int i = 1; i <= middle; i++) {
            window[i] = window[i-1] + slope;
        }
        window[middle + 1] = window[middle];
        for (int i = middle+1; i <= N; i++) {
            window[i] = window[N - i];
        }
    }
}

