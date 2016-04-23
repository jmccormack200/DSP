// impinviir_intr.c

#include "audio.h"
volatile int16_t audio_chR=0;    
volatile int16_t audio_chL=0;    

void I2S_HANDLER(void) {  

int16_t audio_out_chL = 0;
int16_t audio_out_chR = 0;
float32_t xn, yn;
	
audio_IN = i2s_rx();	
audio_chL = (audio_IN & 0x0000FFFF);       //Separate 16 bits channel left
audio_chR = ((audio_IN >>16)& 0x0000FFFF); //Separate 16 bits channel right
	
xn = (float32_t)(audio_chL);  // Compute only the left channel

  //***********************************************************************
  // insert code to compute new output sample here, i.e.
  // y(n) = 0.48255x(n-1) + 0.71624315y(n-1) - 0.38791310y(n-2)
  // also update stored previous sample values, i.e.
  // y(n-2), y(n-1), and x(n-1)
  //***********************************************************************

audio_out_chL = (int16_t)(yn);
	
audio_OUT = ((audio_out_chR<<16 & 0xFFFF0000)) + (audio_out_chL & 0x0000FFFF);	//Put the two channels toguether again
i2s_tx(audio_OUT);

}


int main(void)
{
	audio_init ( hz8000, line_in, intr, I2S_HANDLER); 
 
  while(1){}
}