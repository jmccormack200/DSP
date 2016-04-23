// iirsosadapt_intr.c

#include "audio.h"
#include "elliptic.h"

volatile int16_t audio_chR=0;    //16 bits audio data channel right
volatile int16_t audio_chL=0;    //16 bits audio data channel left

float32_t w[NUM_SECTIONS][2] = {0};
#define BLOCK_SIZE 1
#define NUM_TAPS 256

float32_t beta = 1E-12;

float32_t firStateF32[BLOCK_SIZE + NUM_TAPS - 1];

float32_t firCoeffs32[NUM_TAPS] = { 0.0f };

arm_lms_instance_f32 S;

float32_t yn, adapt_in, adaptfir_out, error,input,wn;

void I2S_HANDLER(void) {   /****** I2S Interruption Handler *****/

	int16_t section;   // index for section number
  float32_t input;   // input to each section
  float32_t wn,yn;   // intermediate and output values

  audio_IN = i2s_rx();	

    input = (float32_t)(prbs()); 
    adapt_in = input;
    gpio_set(P2_10, HIGH); // algorithm timing
// IIR routine
    for (section=0 ; section< NUM_SECTIONS ; section++)
    {
      wn = input - a[section][1]*w[section][0]
           - a[section][2]*w[section][1];
      yn = b[section][0]*wn + b[section][1]*w[section][0]
           + b[section][2]*w[section][1];
      w[section][1] = w[section][0];
      w[section][0] = wn;
      input = yn; // output of current section is input to next
    }
// IIR output now in yn
// could choose from a range of signals to output
audio_chL = (int16_t)(error);
audio_chR = (int16_t)(adaptfir_out);
	
    arm_lms_f32(&S, &adapt_in, &yn, &adaptfir_out, &error, BLOCK_SIZE);
    gpio_set(P2_10, LOW);
	
audio_OUT = ((audio_chR<<16 & 0xFFFF0000)) + (audio_chL & 0x0000FFFF);	//Put the two channels toguether again
i2s_tx(audio_OUT);
}

int main(void)
{
	gpio_set_mode(P2_10,Output);
	arm_lms_init_f32(&S, NUM_TAPS, (float32_t *)&firCoeffs32[0], &firStateF32[0], beta, BLOCK_SIZE);
  audio_init ( hz8000, line_in, intr, I2S_HANDLER); 
 
  while(1){}
}

