// fir_prbs_CMSIS_intr.c 

#include "audio.h"

volatile int16_t audio_chR=0;    
volatile int16_t audio_chL=0;    

#include "bp1750.h"

float32_t x, y, state[N];
arm_fir_instance_f32 S;


void I2S_HANDLER(void) {                 
	
audio_IN = i2s_rx();	
audio_chL = (audio_IN & 0x0000FFFF);       
audio_chR = ((audio_IN >>16)& 0x0000FFFF); 

	  gpio_set(P2_10, HIGH);
    x = (float32_t)(prbs());
 		arm_fir_f32(&S, &x, &y, 1);
 	  audio_chL = (short)(y);
    gpio_set(P2_10, LOW);
		
audio_OUT = ((audio_chL <<16 & 0xFFFF0000)) + (audio_chL & 0x0000FFFF);	
i2s_tx(audio_OUT);

}

int main(void)
{
  gpio_set_mode(P2_10, Output);
	arm_fir_init_f32(&S,N,h,state,1);
  audio_init ( hz8000, line_in, intr, I2S_HANDLER); 
 
  while(1){}
}

