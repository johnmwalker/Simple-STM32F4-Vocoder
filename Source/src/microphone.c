/*
Vocoder Project for ENGR 331
John Walker and Andrew Whately
May 2018

This file is responsible for handling microphone input and 
basic processing.
*/
 
#include "main.h"
#include "pdm_filter.h"
#include "microphone.h"
#include "headphone.h"

/* SPI Configuration defines */
#define SPI_SCK_PIN                       GPIO_Pin_10
#define SPI_SCK_GPIO_PORT                 GPIOB
#define SPI_SCK_GPIO_CLK                  RCC_AHB1Periph_GPIOB
#define SPI_SCK_SOURCE                    GPIO_PinSource10
#define SPI_SCK_AF                        GPIO_AF_SPI2
#define SPI_MOSI_PIN                      GPIO_Pin_3
#define SPI_MOSI_GPIO_PORT                GPIOC
#define SPI_MOSI_GPIO_CLK                 RCC_AHB1Periph_GPIOC
#define SPI_MOSI_SOURCE                   GPIO_PinSource3
#define SPI_MOSI_AF                       GPIO_AF_SPI2

#define AUDIO_REC_SPI_IRQHANDLER          SPI2_IRQHandler

// Not sure why the frequency isn't set to 16000. 16000, 32000, and 64000 all work
// but 32000 certainly seems best. I assume that there's a relationship between the
// sampling frequency and the ratio of the PDM and PCM buffer sizes, but I didn't bother
// to test this hunch since 32000 seemed to work just fine.
#define REC_FREQ                32000     // Audio recording frequency in Hz
#define INTERNAL_BUFF_SIZE      64        // PDM buffer input size
#define PCM_OUT_SIZE            16        // PCM buffer output size
#define recVolume               100       // Maximum recording volume

static uint32_t AudioRecInited = 0;       // Prevent redundant initializations
PDMFilter_InitStruct Filter;              // PDM to PCM filter
uint32_t AudioRecBitRes;                  // The audio sample amplitude resolution
uint16_t RecBuf[PCM_OUT_SIZE];            // PCM output buffer
uint32_t AudioRecChnlNbr;                 // Audio recording number of channels (1 for Mono or 2 for Stereo)
uint32_t AudioRecCurrSize;                // Current size of the recorded buffer

static uint16_t InternalBuffer[INTERNAL_BUFF_SIZE]; // PDM input buffer
static uint32_t InternalBufferSize = 0;

/* Function Prototypes */
static void WaveRecorder_GPIO_Init(void);
static void WaveRecorder_SPI_Init(uint32_t Freq);
static void WaveRecorder_NVIC_Init(void);

/* Call this to initialize the microphone and begin recording */
void WaveRecorderBeginSampling (void)
{
  WaveRecorderInit(REC_FREQ, AudioRecBitRes, AudioRecChnlNbr);
  WaveRecorderStart(RecBuf, PCM_OUT_SIZE);
}

/* Initializes onbaord microphone */
uint32_t WaveRecorderInit(uint32_t AudioFreq, uint32_t BitRes, uint32_t ChnlNbr)
{ 
  /* Check if the interface is already initialized */
  if (AudioRecInited)
  {
    /* No need for initialization */
    return 0;
  }
  else
  {
    /* Enable CRC (cyclic redundancy check) module */
		// This appears to be necessary for SPI functionality
    RCC->AHB1ENR |= RCC_AHB1ENR_CRCEN;
    
    /* Initialize PDM filter */
    Filter.LP_HZ = 8000;
    Filter.HP_HZ = 0;
    Filter.Fs = 16000; // Not sure why this is hard coded
    Filter.Out_MicChannels = 1;
    Filter.In_MicChannels = 1;
    
    PDM_Filter_Init((PDMFilter_InitStruct *)&Filter);
    
    /* Configure the GPIOs */
    WaveRecorder_GPIO_Init();
    
    /* Configure the interrupts (for timer) */
    WaveRecorder_NVIC_Init();
    
    /* Configure the SPI */
    WaveRecorder_SPI_Init(AudioFreq);
    
    /* Set the local parameters */
    AudioRecBitRes = BitRes;
    AudioRecChnlNbr = ChnlNbr;
    
    /* Set state of the audio recorder to initialized */
    AudioRecInited = 1;
    
    /* Return 0 if all operations are OK */
    return 0;
  }  
}

uint8_t WaveRecorderStart(uint16_t* pbuf, uint32_t size)
{
/* Check if the interface has been initialized */
  if (AudioRecInited)
  {
    /* Store the location and size of the audio buffer */
    AudioRecCurrSize = size;
    
    /* Enable the Rx-buffer-not-empty interrupt */
    SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, ENABLE);
		
    /* The Data transfer is performed in the SPI interrupt routine */
    /* Enable the SPI peripheral */
    I2S_Cmd(SPI2, ENABLE); 
   
    /* Return 0 if all operations are OK */
    return 0;
  }
  else
  {
    /* Cannot perform operation */
    return 1;
  }
}

void AUDIO_REC_SPI_IRQHANDLER(void)
{  
   u16 app;

  /* Check if data are available in SPI Data register */
  if (SPI_GetITStatus(SPI2, SPI_I2S_IT_RXNE) != RESET)
  {
		//This appears to transfer samples in the I2S buffer to our local buffer
    app = SPI_I2S_ReceiveData(SPI2);
    InternalBuffer[InternalBufferSize++] = HTONS(app);
    
    /* Once there are enough PDM samples, put them through the filter */
    if (InternalBufferSize >= INTERNAL_BUFF_SIZE)
    {
      InternalBufferSize = 0;
			
			//Note that InternalBuffer has to be uint16_t for the HTONS line above
			//Recasting it as uint8_t is necessary here (breaks otherwise).
      PDM_Filter_64_LSB((uint8_t *)InternalBuffer, RecBuf, recVolume , &Filter);
			
			// Call the callback function in the headphone
      WaveRecorderCallback (RecBuf, PCM_OUT_SIZE);
    }
  }
}

static void WaveRecorder_GPIO_Init(void)
{  
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable GPIO clocks */
  RCC_AHB1PeriphClockCmd(SPI_SCK_GPIO_CLK | SPI_MOSI_GPIO_CLK, ENABLE);

	/* Set GPIO PB10 (SPI CLK) to alternate function mode, AF5 */
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Pin   = SPI_SCK_PIN;
  GPIO_Init(SPI_SCK_GPIO_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(SPI_SCK_GPIO_PORT, SPI_SCK_SOURCE, SPI_SCK_AF);
  
  /* SPI MOSI (master output, slave input) pin configuration */
	// Note that the first four settings are reused from above (also AF5)
  GPIO_InitStructure.GPIO_Pin =  SPI_MOSI_PIN;
  GPIO_Init(SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);
  GPIO_PinAFConfig(SPI_MOSI_GPIO_PORT, SPI_MOSI_SOURCE, SPI_MOSI_AF);
}

static void WaveRecorder_SPI_Init(uint32_t Freq)
{
  I2S_InitTypeDef I2S_InitStructure;

  /* Enable the SPI clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,ENABLE);
  
  /* SPI configuration */
  SPI_I2S_DeInit(SPI2);
  I2S_InitStructure.I2S_AudioFreq = Freq;
  I2S_InitStructure.I2S_Standard = I2S_Standard_LSB;
  I2S_InitStructure.I2S_DataFormat = I2S_DataFormat_16b;
  I2S_InitStructure.I2S_CPOL = I2S_CPOL_High;
  I2S_InitStructure.I2S_Mode = I2S_Mode_MasterRx;
  I2S_InitStructure.I2S_MCLKOutput = I2S_MCLKOutput_Disable;
	
  /* Initialize the I2S peripheral with the structure above */
  I2S_Init(SPI2, &I2S_InitStructure);

  /* Enable the Rx buffer not empty interrupt */
  SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, ENABLE);
}

static void WaveRecorder_NVIC_Init(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3); 
	
  /* Configure the SPI interrupt priority */
  NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
