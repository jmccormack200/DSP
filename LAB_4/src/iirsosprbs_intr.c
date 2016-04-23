// iirsosprbs_intr.c

#include "audio.h"
#include "elliptic.h"

volatile int16_t audio_chR=0;    
volatile int16_t audio_chL=0;  

float w[NUM_SECTIONS][2] = {0};

void I2S_HANDLER(void) {  

int16_t audio_out_chL = 0;
int16_t audio_out_chR = 0;
int16_t section;   // index for section number
float32_t input;   // input to each section
float32_t wn,yn;   // intermediate and output values
	
audio_IN = i2s_rx();	
audio_chL = (audio_IN & 0x0000FFFF);       //Separate 16 bits channel left
audio_chR = ((audio_IN >>16)& 0x0000FFFF); //Separate 16 bits channel right
	
  input =((float32_t)prbs());       // pseudo random input
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

audio_out_chL = (int16_t)(yn);
audio_out_chR = audio_out_chR;	
	
audio_OUT = ((audio_out_chR<<16 & 0xFFFF0000)) + (audio_out_chL & 0x0000FFFF);	//Put the two channels toguether again
i2s_tx(audio_OUT);

}

int main(void)
{
	audio_init ( hz8000, line_in, intr, I2S_HANDLER); 
  while(1){}
}
