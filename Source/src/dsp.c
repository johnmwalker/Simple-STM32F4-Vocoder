/**
 * @file         dsp.c
 * @version      1.0
 * @date         2015
 * @author       Christoph Lauer
 * @compiler     armcc
 * @copyright    Christoph Lauer engineering
 */
 
 // local includes
#include <dsp.h>
#include "dywapitchtrack.h"
#include "LCD.h"
#include "yin.h"
//#include "libpyinc.h"
#include <stdlib.h>
#include "stm32f4_discovery.h"
//#include "audiodata.h"
#include <math.h>
#include "PSOLA.h"


// arm cmsis library includes
//#define ARM_MATH_CM4
#include "stm32f4xx.h"
#include <arm_math.h>

// arm c library includes
#include <stdbool.h>

// the user button switch
extern volatile int user_mode;
int old_user_mode;

dywapitchtracker pitchTracker;
double dywaPitch;

Yin YinStruct;
float yinPitch;
float yinAccuracy;

float* ptr;

#define NUM_FIR_TAPS 56
#define BLOCKSIZE    512
#define TWOPI 3.14159*2

// allocate the buffer signals and the filter coefficients on the heap
arm_fir_instance_q15 FIR;
q15_t outSignal[BLOCKSIZE];
q15_t fir_coeffs_lp[NUM_FIR_TAPS] = { -217,   40,  120,  237,  366,  475,  527,  490,  346,
                                       100, -217, -548, -818, -947, -864, -522,   86,  922,
                                      1904, 2918, 3835, 4529, 4903, 4903, 4529, 3835, 2918,
                                      1904,  922,   86, -522, -864, -947, -818, -548, -217,
                                       100,  346,  490,  527,  475,  366,  237,  120,   40,
                                      -217,    0,    0,    0,    0,    0,    0,    0,    0, 
                                         0,    0};  // low pass at 1KHz with 40dB at 1.5KHz for SR=16KHz
q15_t fir_coeffs_hp[NUM_FIR_TAPS] = { -654,  483,  393,  321,  222,   76, -108, -299, -447,
                                      -501, -422, -200,  136,  520,  855, 1032,  953,  558,
                                      -160,-1148,-2290,-3432,-4406,-5060,27477,-5060,-4406,
                                     -3432,-2290,-1148, -160,  558,  953, 1032,  855,  520,
                                       136, -200, -422, -501, -447, -299, -108,   76,  222,
                                       321,  393,  483, -654,    0,    0,    0,    0,    0,
                                         0,    0,}; // high pass at 1.5KHz with 40dB at 1KHz for SR=16KHz
q15_t fir_state[NUM_FIR_TAPS + BLOCKSIZE];
bool firstStart = false;
int i=0;
// the core dsp function
void dsp(int16_t* buffer, int length)
{
	// only enable the filter if the user button is pressed
	//if (user_mode & 1)
	//{
	  // we initiate the filter only if needed to prevent clitches at the beginning of new buffers
		if (firstStart == false || old_user_mode != user_mode)
		{
			initFilter();
			old_user_mode = user_mode;
			firstStart = true;
		}
		  	// process with FIR
	  //arm_fir_fast_q15(&FIR, buffer, outSignal, BLOCKSIZE);
		
  	// copy the result
	  //arm_copy_q15(outSignal, buffer, length);
		
		//Only choose one of the following

		//if (i==100){
			//STM_EVAL_LEDOn(LED3);
			//LCD_place_cursor(0);
			
			//dywaPitch = dywapitch_computepitch(&pitchTracker, (double*)buffer, 0, length);
			//Yin_init(&YinStruct, 1024, YIN_DEFAULT_THRESHOLD);

			//yinPitch = Yin_getPitch(&YinStruct, audio);
			//yinAccuracy = Yin_getProbability(&YinStruct);
			
			//LCD_write_double(dywaPitch, 3, 1);
			//LCD_write_float(yinPitch, 3, 1);
			//STM_EVAL_LEDOff(LED3);
			//i=0;
		//}
		//i++;



		//struct pyinc_pitch_range pitches = pyinc_feed((float *)buffer, length);
		//const float* res_ptr = pitches.begin;
		//while (res_ptr != pitches.end)
		//{
		//		LCD_write_float(*res_ptr,5,1);
		//		res_ptr++;
		//}
		
		
		

		

  //}
}

// we initialize and switch the filter here
void initFilter()
{
  // apply the low pass filter
  //if (user_mode & 1)
    arm_fir_init_q15(&FIR, NUM_FIR_TAPS, fir_coeffs_lp, fir_state, BLOCKSIZE);
  // or applay the high pass filter depending on the user button switch mode
  //if (user_mode & 2)
  //  arm_fir_init_q15(&FIR, NUM_FIR_TAPS, fir_coeffs_hp, fir_state, BLOCKSIZE);
}
	

//-------------------------------------------------------------------------------------------------------

/* below is a test environment for a noice cancelation with adaptive filters
 // local includes
#include <dsp.h>

// arm cmsis library includes
#define ARM_MATH_CM4
#include "stm32f4xx.h"
#include <arm_math.h>

// arm c library includes
#include <stdbool.h>

// the user button switch
extern volatile int user_mode;

#define NUM_FIR_TAPS 128
#define BLOCKSIZE    512
#define MU           1

// allocate the buffer signals and the filter coefficients on the heap
arm_lms_instance_q15 LMS; 
q15_t outSignal[BLOCKSIZE];
q15_t refSignal[BLOCKSIZE];
q15_t errSignal[BLOCKSIZE];
q15_t fir_coeffs[NUM_FIR_TAPS];
q15_t state[NUM_FIR_TAPS + BLOCKSIZE];

bool firstStart = false;

// the core dsp function
void dsp(int16_t* buffer, int length)
{
	// this block is only processed one time while startup
  if (firstStart == false)
  {
    // set the filter to an ideal filter
		arm_fill_q15(1, fir_coeffs, NUM_FIR_TAPS);fir_coeffs[0] = 32767;
		// set the reference random signal
		arm_fill_q15(0, refSignal, BLOCKSIZE);
		// initialize the adaptive filter 
		arm_lms_init_q15(&LMS, NUM_FIR_TAPS, fir_coeffs, state, MU, BLOCKSIZE, 0);
		// initialize the FIR filter 
    firstStart = true;
  }
	
	// store the reference silent signa in the train mode and switch back to normal mode
  if (user_mode & 1)
	{
		// store the silent noise
    arm_copy_q15(buffer, refSignal, BLOCKSIZE);
    // set the train mode back
    user_mode ++;
	}
	else 
    // aplly the adaptive filter
		arm_lms_q15(&LMS, buffer, refSignal, outSignal, errSignal, BLOCKSIZE);

  // copy the result
  arm_copy_q15(errSignal, buffer, BLOCKSIZE);
}*/
 
//--------------------------------------------------------------------------------------------------------- 

/* --> Below the implementation of an iir filter function without the arm cmsis library
// local includes
#include "dsp.h"
 
// the user button switch
extern volatile int user_mode;
 
// our core dsp function
void dsp(int16_t* buffer, int length)
{
  // initialize some values
  static float previous;
  int i;
     
  // if switched on, apply the filter
  if (user_mode & 1)
  {     
    // perform an simple first order high pass with 12dB/octave
    for (i=0; i<length; i++)
    {
      buffer[i] = (int16_t)( (float)buffer[i] -(float)previous * 0.75f );
      previous = buffer[i];
    }
  }
}
*/


/* Processing and resynthesis: pitch-shifting
Input: magnitude, phase, fftsize, number_of_frame, window_type,
synthesis_hopsize, analysis_hopsize, pitchshift_ratio
Output: new_sound
-----------------------------------------------------------------------------
*/

//float omega[fftsize]; // ideal(?) phase increment for each bin in frequency domain
//float delta_phi[number_of_frame-1][fftsize]; //phase difference between two frames
// phase unwrapping
/*
double shiftPitch(double* soundbuffer, int fftsize, int number_of_frame, char window_type[], int synthesis_hopsize, int analysis_hopsize, float pitchshift_ratio)
{
	float newphase[number_of_frame][fftsize];
	float delta_phi[number_of_frame-1][fftsize]; //phase difference between two frames
	int windowbuffer = window(window_type, fftsize);
	float omega[fftsize]; // ideal(?) phase increment for each bin in frequency domain
	float real[fftsize];
	float imag[fftsize];
	//double soundbuffer[fftsize];
	double windowbuffer[fftsize];
	double intpbuffer[interpolated_len]; //store sound data after interpolation
	int newsound_index = 0;
	float fftbuffer[fftsize];
	float magnitude[number_of_frame][fftsize];
	float phase[number_of_frame][fftsize];
	

	
	int j;
	//fft data are interleaved. We want to skip the second element I think.
	for(i=0; i<number_of_frame;i++)
	{
		arm_rfft_fast_instance_f32 fftStruct;
		arm_rfft_fast_init_f32(&fftStruct, fftsize);
		arm_rfft_fast_f32(&fftStruct,soundbuffer,fftbuffer,0);
		
		magnitude[i][0]=fftbuffer[0];
		phase[i][0] = 0;
		for(j=1; j<fftsize; j++)
		{
			if (j<fftsize/2)
			{
				arm_sqrt_f32((fftbuffer[2*j]*fftbuffer[2*j])+(fftbuffer[2*j+1]*fftbuffer[2*j+1]),&magnitude[i][j]);
				phase[i][j] = 
			}else if(j>=fftsize/2)
			{
				arm_sqrt_f32((fftbuffer[fftsize-2*j]*fftbuffer[fftsize-2*j])+(fftbuffer[fftsize-(2*j+1)]*fftbuffer[fftsize-(2*j+1)]),&magnitude[j][i]);
			}
		}
	}
	
	for(j=0; j<fftsize; j++)
	{
		omega[j] = (TWOPI*analysis_hopsize*j)/fftsize;
	}
	for(i=0; i<(number_of_frame-1); i++)
	{
		for(j=0; j<fftsize; j++)
		{
			if (i == 0)
			{
				delta_phi[i][j] = omega[j] + princarg(phase[i][j] - omega[j]);
			}
			else 
			{
				delta_phi[i][j] = omega[j] + princarg(phase[i][j] - phase[i-1][j] - omega[j]);
			}
		}
	}
	// process
	for(j=0; j<fftsize; j++)
	{
		newphase[0][j] = phase[0][j];
	}
	for(i=1; i<number_of_frame; i++)
	{
		for(j=0; j<fftsize; j++)
		{
			newphase[i][j] = princarg(newphase[i-1][j] + delta_phi[i-1][j]*pitchshift_ratio);
		}
	}
	// resynthesis 
	for(i=0; i<number_of_frame; i++)
	{
		for(j=0; j<fftsize; j++)
		{
			real[j] = magnitude[i][j] * cos(newphase[i][j]);
			imag[j] = magnitude[i][j] * sin(newphase[i][j]);
		}
		ifft(real, imag, soundbuffer);
		fftshift(soundbuffer);
		for(j=0; j<framesize; j++)
		{
			soundbuffer[j] = soundbuffer[j]*windowbuffer[j];
		}
		interpolation(fftsize, soundbuffer, intpbuffer, pitchshift_ratio);
		for(j=0; j<interpolated_len; j++)
		{
			newsound[newsound_index + j] = newsound[newsound_index + j] + intpbuffer[j];
		}
		newsound_index += analysis_hopsize;
	}
	return newsound;
}


Subfunction 1: princarg() (phase unwrapping)
Input: phasein
Output: phaseout
---------------------------------------------*/
/*
float princarg(float phasein)
{
	a = phasein/TWOPI;
	k = round(a);
	phaseout = phasein - k*TWOPI;
}

Subfunction 2: interpolation()
Input: fftsize, soundbuffer, pitchshift_ratio
Output: intpbuffer
---------------------------------------------*/
/*
double* interpolation(int fftsize, double* soundbuffer, float pitchshift_ratio)
{
	factor = 1/pitchshift_ratio;
	x1 = 0;
	for(i=0; i<fftsize; i++)
	{
		y1 = input_buffer[i];
		x2 = x1 + factor;
		y2 = input_buffer[i+1];
		for(j=0; j<(floor(factor)+1); j++)
		{
			xt = x1 + j;
			yt = (y2-y1)/(x2-x1)*(xt-x1) + y1;
			output_buffer[xt] = yt;
		}
	}
}*/
