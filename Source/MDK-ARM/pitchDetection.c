/* Handles all the different types of pitch detection algorithms
*/

#include "pitchDetection.h"

static int _type;
static int _SR;
static int _n;
double pitch;

static Yin yin;
static dywapitchtracker dywa;
//static kiss_fftr_cfg kfftr;
//static kiss_fft_cpx kfftrData[1024];

void initPitchDetection(char type[], int samplingRate, int numSamples)
{
	if(strcmp(type, "dywa")==0 || strcmp(type,"dywapitch")==0 || strcmp(type,"dywapitchtrack")==0 || strcmp(type,"dywapitchtracker")==0)
	{
		dywapitch_inittracking(&dywa);
		_type = 0;
	}
	
	else if(strcmp(type, "yin")==0 || strcmp(type,"Yin")==0)
	{
		Yin_init(&yin, numSamples, YIN_DEFAULT_THRESHOLD);
		_type = 1;
	}
	
	else if(strcmp(type, "kfft")==0)
	{
		//kfftr = kiss_fftr_alloc(numSamples, 0, NULL, NULL);
		_type = 2;
	}
	else
	{
		_type = 3;
	}
	
	_SR = samplingRate;
	_n = numSamples;
}

double detectPitch(audioDataType *buffer)
{
	if(_type==0)
	{
		pitch = dywapitch_computepitch(&dywa, buffer, 0, _n);
		//pitch = 100.0;
	}
	else if(_type==1)
	{
		int i=0;

		//pitch = Yin_getPitch(&yin, (int16_t*)buffer);
		//pitch = 200.0;
	}
	else if(_type == 2)
	{
		//kiss_fftr(kfftr,(float*)buffer,kfftrData);
		//pitch = maxValue((float*)kfftrData,_n);
		//pitch = 300.0;
	}
	else if(_type == 3)
	{
		pitch = 3.0;
	}
	else
	{
		pitch = -1.0;
	}
	
	return pitch;
}

float maxValue(float myArray[], int size)
{
    int i;
    int maxValue = myArray[0];

    for (i = 1; i < size; ++i) {
        if ( myArray[i] > maxValue ) {
            maxValue = myArray[i];
        }
    }
    return maxValue;
}

