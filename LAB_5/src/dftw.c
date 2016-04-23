// dftw.c 
//
// N-point DFTW
//

#include <math.h> 
#include "platform.h"
#include "gpio.h"

#define PI 3.14159265358979
#define N 100
#define TESTFREQ 800.0
#define SAMPLING_FREQ 8000.0

typedef struct
{
  float real;
  float imag;
} COMPLEX;

COMPLEX samples[N];
COMPLEX twiddle[N];

void dftw(COMPLEX *x, COMPLEX *w)
{
}

int main()
{    
  int n;
  gpio_set_mode(P2_10, Output);
  for(n=0 ; n<N ; n++)
  {
    samples[n].real = cos(2*PI*TESTFREQ*n/SAMPLING_FREQ);
    samples[n].imag = 0.0;
  }
	gpio_set(P2_10, HIGH);
  dftw(samples,twiddle);    //call DFTW function
	gpio_set(P2_10, LOW);
  while(1){}
}	


