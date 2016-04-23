// fft_CMSIS.c 
//
// N-point radix-2 FFT of sequence read from lookup table
// uses CMSIS FFT function
//


#include "audio.h"
#include "arm_const_structs.h"
#include "gpio.h"

#define N 128
#define TESTFREQ 800.0f
#define SAMPLING_FREQ 8000.0f

float32_t samples[2*N];

int main()
{    
  int n;
	gpio_set_mode(P2_10, Output);
	
  for(n=0 ; n<N ; n++)
  {
    samples[2*n] = arm_cos_f32(2*PI*TESTFREQ*n/SAMPLING_FREQ);
    samples[2*n+1] = 0.0;
  }
	gpio_set(P2_10, HIGH);
  arm_cfft_f32(&arm_cfft_sR_f32_len128, samples, 0, 1);
	gpio_set(P2_10, LOW);
  while(1){}
}	
