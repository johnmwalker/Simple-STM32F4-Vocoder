/*
Vocoder Project for ENGR 331
John Walker and Andrew Whately
May 2018

This file is responsible for handling headphone output and 
all related processing. The application spends most of its
time in here.
*/
 
#include <main.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "PSOLA.h"
#include "ADC.h"

static uint8_t volume = 70;
extern volatile int user_mode;

/*
 * We have three buffers: two output buffers used in a ping-pong arrangement, and an input
 * (microphone) circular buffer. Because the output buffers are written directly to the I2S
 * interface with DMA, they must be stereo. The microphone buffer is mono and its duration
 * is exactly 3 times the length of one of the ping-pong buffers. The idea is that during
 * normal operation, the microphone buffer will vary between about 1/3 full and 2/3 full
 * (with a 1/3 buffer margin on either side).
 */
#define SAMPLE_RATE 16000        // sampling rate
#define OUT_BUFFER_SAMPLES 1024  // number of samples per output ping-pong buffer
#define MIC_BUFFER_SAMPLES (OUT_BUFFER_SAMPLES * 3 / 2) //1536

// These are the two ping-pong buffers
static int16_t buff0 [OUT_BUFFER_SAMPLES], buff1 [OUT_BUFFER_SAMPLES], micbuff [MIC_BUFFER_SAMPLES];
static volatile uint16_t mic_head, mic_tail;    // These pointers indicate which samples are fresh/haven't been played yet
static volatile uint8_t next_buff;              // Indicates whether buff0 or buff1 should be written to next
 
// Local function
static void fill_buffer (int16_t *buffer, int num_samples);

// Arrays and other things necessary for pitch tracking
dywapitchtracker dywaMic;
float myPitch;
int micBuffInts[OUT_BUFFER_SAMPLES];
double micDoubleBuffer[OUT_BUFFER_SAMPLES];

dywapitchtracker dywaADC;
float modulatorPitch;
uint16_t modulatorBuffer[OUT_BUFFER_SAMPLES];
double modulatorDoubleBuffer[OUT_BUFFER_SAMPLES];

/*
 * Reinitializes the ADC interrupt everytime this gets called. Not sure
 * why the reinitialization is necessary. The ADC only converts a single
 * buffer without reinitialization.
 */
void ADCCallback(void)
{
	initAudioIn(modulatorBuffer, 1024);
}

// Transfers microphone PCM samples into a local buffer for processing
// and checks for clipping (when audio is too loud going into microphone).
void WaveRecorderCallback (uint16_t *buffer, int num_samples)
{
    static int clip_timer;
    int clip = 0, i;
		
		//Move samples into micbuff and check for clipping.
    for (i = 0; i < num_samples; ++i) {
        int16_t sample = *buffer++;
        if (sample >= 32700 || sample <= -32700)
            clip = 1;
        micbuff [mic_head + i] = sample;
    } 

		//Update the microphone head to zero if we've gone too far, or put it at the end of the samples we just updated
    mic_head = (mic_head + num_samples >= MIC_BUFFER_SAMPLES) ? 0 : mic_head + num_samples;
		
		//Flash the red LED if there was a clip
    if (clip_timer) {
        if (!--clip_timer)
            STM_EVAL_LEDOff(LED5);
    }
    else if (clip)
        STM_EVAL_LEDOn(LED5);
    if (clip)
        clip_timer = 50;
}

// Called in main.c. This function initializes everything and begins the
// streaming process.
void WavePlayBack(void)
{ 
	//mic_head and mic_tail start at 0
	
	// Initialize the dywapitch structures
	dywapitch_inittracking(&dywaMic);
	dywapitch_inittracking(&dywaADC);
	
	// Initialize the ADC audio input
	initAudioIn(modulatorBuffer, OUT_BUFFER_SAMPLES);

  /* First, we start sampling internal microphone */
  WaveRecorderBeginSampling ();

  /* Initialize wave player (Codec, DMA, I2C) */
  WavePlayerInit(SAMPLE_RATE);

  /* Let the microphone data buffer get 2/3 full (which is 2 playback buffers) */ 
	// Is it actually 2? I believe it's only 1.
	// mic_head is updated in WaveRecorderCallback interrupt
  while (mic_head < MIC_BUFFER_SAMPLES * 2 / 3); 

  /* Fill the second playback buffer (the first will just be zeros to start) */
  fill_buffer (buff1, OUT_BUFFER_SAMPLES);
  
  /* Start audio playback on the first buffer (which is all zeros now) */
  Audio_MAL_Play((uint32_t)buff0, OUT_BUFFER_SAMPLES * 2);
  next_buff = 1; 
  
  /* This is the main loop of the program. We simply wait for a buffer to be exhausted
   * and then we refill it. The callback (which is triggered by DMA completion) actually
   * handles starting the next buffer playing, so we don't need to be worried about that
   * latency here. The functionality of the fill_buffer() function determines what it is
   * that we are doing (e.g., playing tones, echoing the mic, being a nervous dog, etc.)
   */
	 
	/* The above comment is referring to the loop below. Note that we abandoned all FFT-related
	processing due to its memory intensive nature. One or two FFTs is fine, but in the context
	of a phase vocoder, we were having troubles with heap size allocation. This code has been
	left for future reference. Note that we also began moving toward using the ARM fast FFt
	provided with the SPL rather than KISSFFT
	*/
	/*
	kiss_fftr_cfg kfftr;
	kiss_fftr_cfg kfftri;
	kiss_fft_scalar kfftrIn[OUT_BUFFER_SAMPLES];
	kiss_fft_cpx kfftrData[OUT_BUFFER_SAMPLES];
	//kiss_fft_cpx actualData[OUT_BUFFER_SAMPLES];
	kfftr = kiss_fftr_alloc(OUT_BUFFER_SAMPLES, 0, 0, 0);
	kfftri = kiss_fftr_alloc(OUT_BUFFER_SAMPLES, OUT_BUFFER_SAMPLES/2 + 1, 0, 0);
	float sineWaveFloat[OUT_BUFFER_SAMPLES];
	*/
	
	// Previous implementations of pitch detection used an external file to manage
	// multiple pitch detection libraries. However, dywapitch was found to be the
	// most accurate and efficient, so we abandoned the other approaches.
	//initPitchDetection("dywa", SAMPLE_RATE, OUT_BUFFER_SAMPLES);
	
	int l=0;
	
  while (1) {
		
		// As soon as one buffer is finished playing, the next is triggered to play
		// in the interrupt below and the old one is filled.
    while (next_buff == 1);
		fill_buffer (buff0, OUT_BUFFER_SAMPLES);
		
    while (next_buff == 0);
    fill_buffer (buff1, OUT_BUFFER_SAMPLES);
		
		// If statement prevents gratuitous 
		if(l==10)
		{
			GPIOD->ODR ^= (1<<13);
			
			// Currently, printing to the LCD interferes with (essentially turns off)
			// output to the headphones. 
			//LCD_place_cursor(0);
			//LCD_write_float(myPitch, 6, 3);
			//LCD_write_int(time,7);
			//LCD_write_int(modulatorBuffer[0],5);
			//LCD_write_float(modulatorPitch,5,1);
			//LCD_write_double(modulatorDoubleBuffer[0],5,1);
			l=0;
		}
		l++;
  }
}
 
// Initializes the headphone jack and DAC
int WavePlayerInit(uint32_t AudioFreq)
{ 
  /* Initialize I2S interface */  
  EVAL_AUDIO_SetAudioInterface(AUDIO_INTERFACE_I2S);
  
  /* Initialize the Audio codec and all related peripherals (I2S, I2C, IOExpander, IOs...) */  
  EVAL_AUDIO_Init(OUTPUT_DEVICE_AUTO, volume, AudioFreq );  
  
  return 0;
}

//Whenever the buffer gets emptied, begin playing the next buffer
void EVAL_AUDIO_TransferComplete_CallBack(uint32_t pBuffer, uint32_t Size)
{
  if (next_buff == 0) {
		// Number of samples is multiplied by 2 because we're now in stereo
    Audio_MAL_Play((uint32_t)buff0, OUT_BUFFER_SAMPLES * 2);
    next_buff = 1; 
  }
  else {
    Audio_MAL_Play((uint32_t)buff1, OUT_BUFFER_SAMPLES * 2);
    next_buff = 0; 
  }
}
 
void EVAL_AUDIO_HalfTransfer_CallBack(uint32_t pBuffer, uint32_t Size)
{  
}

void EVAL_AUDIO_Error_CallBack(void* pData)
{
  while (1) /* Stop the program with an infinite loop */
  {STM_EVAL_LEDOn(LED5);} /* could also generate a system reset to recover from the error */
}

uint16_t EVAL_AUDIO_GetSampleCallBack(void)
{
  return 0;
}

// Fills buff0 and buff1 with the processed samples
static void fill_buffer (int16_t *buffer, int num_samples)
{
	int k;
	int count = num_samples / 2;

	// Translate samples to appropriate formats for processing
	for(k=0;k<OUT_BUFFER_SAMPLES;k++)
	{
		micDoubleBuffer[k]=(double)*(&micbuff[k] + mic_tail);
		micBuffInts[k] = (int)*(&micbuff[k] + mic_tail);
		modulatorDoubleBuffer[k] = (double)modulatorBuffer[k];
	}

	// Perform processing
	myPitch        = dywapitch_computepitch(&dywaMic,micDoubleBuffer, 0, OUT_BUFFER_SAMPLES )/(44100.0/SAMPLE_RATE);
	modulatorPitch = dywapitch_computepitch(&dywaADC, modulatorDoubleBuffer, 0, OUT_BUFFER_SAMPLES)/(44100.0/SAMPLE_RATE);
	changePitch(micBuffInts, SAMPLE_RATE, myPitch, modulatorPitch);
	
	// Place new samples in the microphone buffer
	for(k=0;k<OUT_BUFFER_SAMPLES;k++)
	{
		*(&micbuff[k] + mic_tail) = (int16_t)micBuffInts[k];
	}
	
	// Turn the mono recording into stereo output
  while (count--) 
	{
    *buffer++ = micbuff [mic_tail];
    *buffer++ = micbuff [mic_tail];
    mic_tail = (mic_tail + 1 >= MIC_BUFFER_SAMPLES) ? 0 : mic_tail + 1;
  }
}

int modOrMic = 0;
// This is an attempt to allow the user to print pitch values to the LCD at will
// by pressing the userbutton. Currently, pressing the userbutton stops the
// headphone output and fails to print.
void printPitch(void)
{
	STM_EVAL_LEDOn(LED6);
	if (modOrMic==0){
		LCD_place_cursor(0);
		STM_EVAL_LEDOff(LED6);

		LCD_write_string("MOD Pitch = ");
		LCD_write_float(modulatorPitch,4,1);
		modOrMic = 1;
	} else 	if (modOrMic==1){
		LCD_place_cursor(0);
		LCD_write_string("MIC Pitch = ");
		LCD_write_float(myPitch,4,1);
		modOrMic = 0;
	}
	// My hope is that reinitializing the playback here will allow us to resume
	// playback, but I'm not sure.
	WavePlayBack();
}

/* No longer needed. This was used to test pitch detection prior to discovering that
   our issue was memory related, not microphone-related.
void genSineBuffer(int freq, int samplingRate, int numSamples, double* sineBuffer)
{
	int i;
	for(i = 0; i < numSamples; i++)
	{
		// i is the sample index
		// Straight sine function means one cycle every 2*pi samples:
		// buffer[i] = sin(i); 
		// Multiply by 2*pi--now it's one cycle per sample:
		// buffer[i] = sin((2 * pi) * i); 
		// Multiply by 1,000 samples per second--now it's 1,000 cycles per second:
		// buffer[i] = sin(1000 * (2 * pi) * i);
		// Divide by 44,100 samples per second--now it's 1,000 cycles per 44,100
		// samples, which is just what we needed:
		sineBuffer[i] = (1<<14)*sin(freq * (2 * 3.14159) * i / (double)samplingRate);
	}
}
*/
