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
COMPLEX cbuf[N];
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
  uint32_t *txbuf, *rxbuf;

  if(tx_proc_buffer == PING) txbuf = dma_tx_buffer_ping; 
  else txbuf = dma_tx_buffer_pong; 
  if(rx_proc_buffer == PING) rxbuf = dma_rx_buffer_ping; 
  else rxbuf = dma_rx_buffer_pong; 
	
  for(i=0; i<DMA_BUFFER_SIZE ; i++)
  {
	audio_IN =   rxbuf[i];	
	audio_chL = (uint16_t)(audio_IN & 0x0000FFFF); 
    cbuf[i].real = (float)audio_chL; //*hamming[i];
    cbuf[i].imag = 0.0f;
    sinebuf[i] = audio_chL;
  }
	
   fft(cbuf,DMA_BUFFER_SIZE,twiddle);

  for(i=0; i<DMA_BUFFER_SIZE ; i++)
  {
		
		audio_chL = (int16_t)((sqrt(cbuf[i].real * cbuf[i].real + (cbuf[i].imag * cbuf[i].imag )))/MAGNITUDE_SCALING_FACTOR);
	
    if (i==0) {audio_chL = TRIGGER;}
   		
  	txbuf[i]= ((sinebuf[i])<<16 & 0xFFFF0000) + (-(audio_chL) & 0x0000FFFF);	
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


