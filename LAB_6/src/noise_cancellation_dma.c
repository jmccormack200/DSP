// noise_cancellation_dma.c 

#include "audio.h"
#include "bilinear.h"

#define BLOCK_SIZE 1
#define NUM_TAPS 128

float32_t beta = 2e-11;

float32_t firStateF32[BLOCK_SIZE + NUM_TAPS -1];

float32_t firCoeffs32[NUM_TAPS] = {0.0f};

arm_lms_instance_f32 S;

float32_t input, signoise, wn, yn, yout, error;

float w[NUM_SECTIONS][2] = {0.0f, 0.0f}; // IIR coefficients
void DMA_HANDLER (void)  /****** DMA Interruption Handler*****/
{

    if (dma_state(0)){ 

				dma_clean(0);         
	
					if(tx_proc_buffer == (PONG))
						{
						dma_src_memory (0,(uint32_t)&(dma_tx_buffer_pong));                   
						tx_proc_buffer = PING; 
						}
					else
						{
						dma_src_memory (0,(uint32_t)&(dma_tx_buffer_ping));                
						tx_proc_buffer = PONG; 
						}
				tx_buffer_empty = 1;                				
       

				dma_transfersize(0,DMA_BUFFER_SIZE);
				dma_enable(0);			
    }
    if (dma_state(1)){ 
        	dma_clean(1);
					if(rx_proc_buffer == PONG)
					  {
						dma_dest_memory (1,(uint32_t)&(dma_rx_buffer_pong));   
						rx_proc_buffer = PING;
						}
					else
						{
						dma_dest_memory (1,(uint32_t)&(dma_rx_buffer_ping));   
						rx_proc_buffer = PONG;
						}
					rx_buffer_full = 1;   					
				dma_transfersize(1,DMA_BUFFER_SIZE);
				dma_enable(1);		
    }
}

void proces_buffer(void) 
{
 int ii;
  uint32_t *txbuf, *rxbuf;
  float32_t refnoise, signal;
  int16_t audio_chR, audio_chL, audio_out_chR, audio_out_chL;
	int section;
	
  if(tx_proc_buffer == PING) txbuf = dma_tx_buffer_ping; 
  else txbuf = dma_tx_buffer_pong; 
  if(rx_proc_buffer == PING) rxbuf = dma_rx_buffer_ping; 
  else rxbuf = dma_rx_buffer_pong; 
	
  for(ii=0; ii<DMA_BUFFER_SIZE ; ii++)
  {		
	audio_IN = (*rxbuf++);	
	audio_chL = (audio_IN & 0x0000FFFF);  
  audio_chR = ((audio_IN >>16)& 0x0000FFFF);		

		refnoise = (float32_t)(audio_chR);
    signal = (float32_t)(audio_chL);
		
    input = refnoise;
    for (section=0 ; section<NUM_SECTIONS ; section++)
    {
      wn = input - a[section][1]*w[section][0]
           - a[section][2]*w[section][1];
      yn = b[section][0]*wn + b[section][1]*w[section][0]
           + b[section][2]*w[section][1];
      w[section][1] = w[section][0];
      w[section][0] = wn;
      input = yn;
    }
    signoise = yn + signal;
    arm_lms_f32(&S, &refnoise, &signoise, &yout, &error, 1);
		audio_out_chL = (int16_t)(signoise);
    audio_out_chR = (int16_t)(error);
		*txbuf++ = ((audio_out_chR<<16 & 0xFFFF0000)) + (audio_out_chL & 0x0000FFFF);
 	} 
  tx_buffer_empty = 0;
  rx_buffer_full = 0;
	}

int main (void) {    //Main function

	gpio_set_mode(P2_10,Output);
	arm_lms_init_f32(&S, NUM_TAPS, (float32_t *)&firCoeffs32[0], &firStateF32[0], beta, 1);
  audio_init ( hz8000, line_in, dma, DMA_HANDLER);

while(1){
	while (!(rx_buffer_full && tx_buffer_empty)){};
	  gpio_set(P2_10, HIGH);
		proces_buffer();
	  gpio_set(P2_10, LOW);
	}
}

