1	// dft.c 
2	//
3	// N-point DFT of sequence read from lookup table
4	//
5	
6	#include <math.h> 
7	#include "platform.h"
8	#include "gpio.h"
9	
10	#define PI 3.14159265358979
11	#define N 100
12	#define TESTFREQ 800.0
13	#define SAMPLING_FREQ 8000.0
14	
15	typedef struct
16	{
17	  float real;
18	  float imag;
19	} COMPLEX;
20	
21	COMPLEX samples[N];
22	
23	void dft(COMPLEX *x)
24	{
25	  // write the dft() function here.
26	}
27	
28	int main()
29	{    
30	  int n;
31	  gpio_set_mode(P2_10, Output);
32	  for(n=0 ; n<N ; n++)
33	  {
34	    samples[n].real = cos(2*PI*TESTFREQ*n/SAMPLING_FREQ);
35	    samples[n].imag = 0.0;
36	  }
37	  gpio_set(P2_10, HIGH);
38	  dft(samples);          //call DFT function
39	  gpio_set(P2_10, LOW);
40	  while(1){}
41	}
