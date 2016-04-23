// fft128_dma.c

#include "audio.h"
#include "math.h"
//#include "hamm128.h"
#define N (DMA_BUFFER_SIZE)

#define TRIGGER 12000
#define MAGNITUDE_SCALING_FACTOR 32

typedef struct
{
  float32_t real;
  float32_t imag;
} COMPLEX;

#include "fft.h"

COMPLEX twiddle[N];
COMPLEX cbufL[N];
COMPLEX cbufR[N];
COMPLEX cbufLcross[N];
COMPLEX cbufRcross[N];
int16_t sinebuf[N];
int16_t outbuffer[N];

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
	
  for(i=0; i<DMA_BUFFER_SIZE ; i++)
  {
	audio_IN =   rxbuf[i];	
	audio_chL = (uint16_t)(audio_IN & 0x0000FFFF); 
    cbufL[i].real = (float)audio_chL; //*hamming[i];
    cbufL[i].imag = 0.0f;
    sinebuf[i] = audio_chL;
		
	audio_chR = (uint16_t)((audio_IN & 0xFFFF0000) >> 16);
		cbufR[i].real = (float)audio_chR; //*hamming[i];
    cbufR[i].imag = 0.0f;
		
		//txbuf[i] = rxbuf[i];
		
  }
	
	
	
	fft(cbufL,DMA_BUFFER_SIZE,twiddle);
	fft(cbufR, DMA_BUFFER_SIZE, twiddle);
	fft(cbufL, DMA_BUFFER_SIZE, twiddle);
	fft(cbufR, DMA_BUFFER_SIZE, twiddle);

	//process data here
	
	/*
  for(i=0; i<DMA_BUFFER_SIZE ; i++)
  {
		int magL = (int16_t)((sqrt(cbufL[i].real * cbufL[i].real + (cbufL[i].imag * cbufL[i].imag )))/MAGNITUDE_SCALING_FACTOR);
		int magR = (int16_t)((sqrt(cbufR[i].real * cbufR[i].real + (cbufR[i].imag * cbufR[i].imag )))/MAGNITUDE_SCALING_FACTOR);
		
		float phaseL = (float)atan2(cbufL[i].imag, cbufL[i].real) + 3.14159;
		float phaseR = (float)atan2(cbufR[i].imag, cbufR[i].real) + 3.14159;
		
		cbufLcross[i].real = magL * cos(phaseL);
		cbufLcross[i].imag = magL * sin(phaseL);
		cbufRcross[i].real = magR * cos(phaseR);
		cbufRcross[i].imag = magR * sin(phaseR);
		
		cbufL[i].real = cbufL[i].real - cbufRcross[i].real;
		cbufL[i].imag = cbufL[i].imag - cbufRcross[i].imag;
		
		cbufR[i].real = cbufR[i].real - cbufLcross[i].real;
		cbufR[i].imag = cbufR[i].imag - cbufLcross[i].imag;		
	}
	*/
	
	//fft(cbufL, DMA_BUFFER_SIZE, twiddle);
	//fft(cbufR, DMA_BUFFER_SIZE, twiddle);
	
	for(i=0; i<DMA_BUFFER_SIZE; i++)
	{
				
		//audio_chL = (int16_t)((sqrt(cbufL[i].real * cbufL[i].real + (cbufL[i].imag * cbufL[i].imag )))/MAGNITUDE_SCALING_FACTOR);
		//audio_chR = (int16_t)((sqrt(cbufR[i].real * cbufR[i].real + (cbufR[i].imag * cbufR[i].imag )))/MAGNITUDE_SCALING_FACTOR);
	
		audio_chL = (int16_t)(cbufL[i].real);
		audio_chR = (int16_t)(cbufR[i].real);
		
    //if (i==0) {audio_chL = TRIGGER;}
		
		txbuf[i]= (-(audio_chR<<16 & 0xFFFF0000) + (-(audio_chL) & 0x0000FFFF));	
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

	gpio_set_mode(P2_10,Output);
  audio_init ( hz48000, line_in, dma, DMA_HANDLER);

while(1){
	while (!(rx_buffer_full && tx_buffer_empty)){};
	  gpio_set(P2_10, HIGH);
		proces_buffer();
	  gpio_set(P2_10, LOW);
	}
}


