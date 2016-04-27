// fft128_dma.c

#include "audio.h"
#include "arm_math.h"                   // ARM::CMSIS:DSP

#include "hamm128.h"

//#include "hamm128.h"
#define N (DMA_BUFFER_SIZE)

#define TRIGGER 12000
#define MAGNITUDE_SCALING_FACTOR 32

#include <math.h>
#include <stdio.h>



typedef struct
{
  float32_t real;
  float32_t imag;
} COMPLEX;

#include "fft.h"


// twiddle is used for the fft,
COMPLEX twiddle[N];
// itwiddle is used for the ifft
COMPLEX itwiddle[N];
// we setup buffers for the L and R audio
COMPLEX cbufL[N];
COMPLEX cbufR[N];
//the cross buffers are used to add the channels
COMPLEX cbufLcross[N];
COMPLEX cbufRcross[N];



// This DMA Handler is provided by the labs
void DMA_HANDLER (void)  /****** DMA Interruption Handler*****/
{ 

    if (dma_state(0)){ //check interrupt status on channel 0

				dma_clean(0);         // Clear terminate counter Interrupt pending
	
					if(tx_proc_buffer == (PONG))
						{
						dma_src_memory (0,(uint32_t)&(dma_tx_buffer_pong));    //Ping-pong the source address                 
						tx_proc_buffer = PING; 
						}
					else
						{
						dma_src_memory (0,(uint32_t)&(dma_tx_buffer_ping));                
						tx_proc_buffer = PONG; 
						}
				tx_buffer_empty = 1;                // signal to main() that tx buffer empty					
       
				//Enable channel 0 for the next transmission
				dma_transfersize(0,DMA_BUFFER_SIZE);
				dma_enable(0);			
    }
    if (dma_state(1)){ //check interrupt status on channel 1

        // Clear terminate counter Interrupt pending
				dma_clean(1);
					if(rx_proc_buffer == PONG)
					  {
						dma_dest_memory (1,(uint32_t)&(dma_rx_buffer_pong));   //Ping-pong the destination address
						rx_proc_buffer = PING;
						}
					else
						{
						dma_dest_memory (1,(uint32_t)&(dma_rx_buffer_ping));   
						rx_proc_buffer = PONG;
						}
					rx_buffer_full = 1;   					

				//Enable channel 1 for the next transmission
				dma_transfersize(1,DMA_BUFFER_SIZE);
				dma_enable(1);		
    }
	}

// This is the main loop where we process data	
void proces_buffer(void) 
{
 int i;

  int16_t audio_chL;
	int16_t audio_chR;
  uint32_t *txbuf, *rxbuf;

  if(tx_proc_buffer == PING) txbuf = dma_tx_buffer_ping; 
  else txbuf = dma_tx_buffer_pong; 
  if(rx_proc_buffer == PING) rxbuf = dma_rx_buffer_ping; 
  else rxbuf = dma_rx_buffer_pong; 
	
	// First we separate audio_IN into the L and R Audio
	// We do this by anding the audio_IN with a bit mask.
  for(i=0; i<DMA_BUFFER_SIZE ; i++)
  {
	audio_IN =   rxbuf[i];	
	audio_chL = (uint16_t)(audio_IN & 0x0000FFFF); 
		//We divide into real and imaginary for the fft, but there is not an imaginary component yet
    cbufL[i].real = (float)audio_chL; // * hamming[i];
    cbufL[i].imag = 0.0f;
		
	audio_chR = (uint16_t)((audio_IN & 0xFFFF0000) >> 16);
		cbufR[i].real = (float)audio_chR; // * hamming[i];
    cbufR[i].imag = 0.0f;
		
		//txbuf[i] = rxbuf[i];
		
  }
	
	
	//We fft each channel
	fft(cbufL, DMA_BUFFER_SIZE, twiddle);
	fft(cbufR, DMA_BUFFER_SIZE, twiddle);


	//process data here

  for(i=0; i<DMA_BUFFER_SIZE ; i++)
  {
		float magL;
		float phaseL;
		float magR;
		float phaseR;
		int noVocal = 1; //0 for vocals, 1 for no vocals;
		int fullWide = 0; //1 for full wide, 0 for subtle. noVocal must be set to 0.
		
		/*
			These settings adjust the effects you hear. To get a karaoke style sound, set the ''adjust'' parameter to 1. 
			To hear a deeply wide circuit I recommend adjust at 0.8 and upgain set to 16.0f. For a subtle wide sound,
			like the kind you would want to hear in a final product, I recommend an adjust value of 0.8 and an
			upgain value of 1.0f~2.0f. 
		*/
		
		// Set to 1.0f for karaoke mode, 0.8f works better for wide effect.
		float adjust;
		//Increase upgain to increase the wide effect, it also makes the general audio louder so we
		// use downgain to adjust the volume lower. 
		float upgain;
		float downgain;
		
		if (fullWide == 0){
			upgain = 16.0f;
		} else {
			upgain = 1.0f;
		}
		
		if (noVocal == 1){
			adjust = 1.0f;
		} else {
			adjust = 0.8f;
		}
		
		downgain = upgain * 16.0f;
		
		// we change the real and imaginary into polar and shift the polar by pi
		// Then we convert back to rectangular. 
		// We do this separately for each channel. 
		magL = (sqrtf((cbufL[i].real * cbufL[i].real) + (cbufL[i].imag * cbufL[i].imag ))) * adjust;
		phaseL = atan2f(cbufL[i].imag, cbufL[i].real) + PI;
		cbufLcross[i].real = (float32_t)(magL * cosf(phaseL));
		cbufLcross[i].imag = (float32_t)(magL * sinf(phaseL));
		
				
		magR = (sqrtf(cbufR[i].real * cbufR[i].real + (cbufR[i].imag * cbufR[i].imag ))) * adjust;
		phaseR = atan2f(cbufR[i].imag, cbufR[i].real) + PI;
		cbufRcross[i].real = (float32_t)(magR * cosf(phaseR));
		cbufRcross[i].imag = (float32_t)(magR * sinf(phaseR));
		
		// We add the out of phase of R to in phase of L, then add this to the original L. This emphasis whatever is not 
		// found in the center channel. We then repeat this process for R. 
		//Remove the first instance of each cbuf to create a deeper karaoke effect. 
		
		if (noVocal == 0){
			cbufL[i].real = (cbufL[i].real + ((cbufL[i].real + cbufRcross[i].real))*upgain)/downgain;
			cbufL[i].imag = (cbufL[i].imag + ((cbufL[i].imag + cbufRcross[i].imag))*upgain)/downgain;
			
			cbufR[i].real = (cbufR[i].real + ((cbufR[i].real + cbufLcross[i].real))*upgain)/downgain;
			cbufR[i].imag = (cbufR[i].imag + ((cbufR[i].imag + cbufLcross[i].imag))*upgain)/downgain;	
		} else {
			cbufL[i].real = (cbufL[i].real + cbufRcross[i].real)/16.0f;
			cbufL[i].imag = (cbufL[i].imag + cbufRcross[i].imag)/16.0f;
			
			cbufR[i].real = (cbufR[i].real + cbufLcross[i].real)/16.0f;
			cbufR[i].imag = (cbufR[i].imag + cbufLcross[i].imag)/16.0f;	
		}
	}
	
	// We then take the inverse FFT
	//remember to change below too!!!!!!
	fft(cbufL, DMA_BUFFER_SIZE, itwiddle);
	fft(cbufR, DMA_BUFFER_SIZE, itwiddle);
	
	for(i=0; i<DMA_BUFFER_SIZE; i++)
	{
		//After the fft we should only have a real component so that is all we will output. 
		audio_chL = (int16_t)(cbufL[i].real);
		audio_chR = (int16_t)(cbufR[i].real);
		
		
		// We fill the txbuf with the L and R audio. The channels must be shifted and masked to be
		// heard properly. 
		txbuf[i]= (-(audio_chL<<16 & 0xFFFF0000) + (-(audio_chR) & 0x0000FFFF));
  }

	tx_buffer_empty = 0;
  rx_buffer_full = 0;
	}


int main (void) {    //Main function
	
  int n;
  for (n=0 ; n< N ; n++)
  {
    twiddle[n].real = (float32_t) (cos(PI*n/N));
    twiddle[n].imag = (float32_t) (-sin(PI*n/N));
  }	
	
	for (n=0 ; n< N ; n++)
  {
    itwiddle[n].real = (float32_t) (cos(PI*n/N));
    itwiddle[n].imag = (float32_t) (sin(PI*n/N));
  }	
	
	
	gpio_set_mode(P2_10,Output);
  audio_init ( hz48000, line_in, dma, DMA_HANDLER);

while(1){
	while (!(rx_buffer_full && tx_buffer_empty)){};
	  gpio_set(P2_10, HIGH);
		proces_buffer();
	  gpio_set(P2_10, LOW);
	}
}



