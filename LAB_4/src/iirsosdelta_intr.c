// iirsosdelta_intr.c

#include "audio.h"
#include "elliptic.h"

#define BUFSIZE 256
#define AMPLITUDE 60000.0f

float32_t w[NUM_SECTIONS][2] = {0};
float32_t dimpulse[BUFSIZE];
float32_t response[BUFSIZE];
int16_t indexptr = 0;

void I2S_HANDLER(void) {  

int16_t audio_out_chL = 0;
int16_t audio_out_chR = 0;
int16_t section;   // index for section number
float32_t input;   // input to each section
float32_t wn,yn;   // intermediate and output values
	
audio_IN = i2s_rx();	

  input = dimpulse[indexptr];       // impulse input 
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
  response[indexptr++] = yn;
  if (indexptr >= BUFSIZE) indexptr = 0;

    audio_out_chL = (int16_t)(yn*AMPLITUDE);
    audio_out_chR = audio_out_chL;

audio_OUT = ((audio_out_chR<<16 & 0xFFFF0000)) + (audio_out_chL & 0x0000FFFF);	//Put the two channels toguether again
i2s_tx(audio_OUT);

}

int main(void)
{
int i;
  
  for (i=0 ; i< BUFSIZE ; i++) dimpulse[i] = 0.0;
  dimpulse[0] = 1.0;
	audio_init ( hz8000, line_in, intr, I2S_HANDLER); 
 
  while(1){}
}
