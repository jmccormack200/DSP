// loop_dma.c

#include "audio.h"

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
 int ii;
  uint32_t *txbuf, *rxbuf;

  if(tx_proc_buffer == PING) txbuf = dma_tx_buffer_ping; 
  else txbuf = dma_tx_buffer_pong; 
  if(rx_proc_buffer == PING) rxbuf = dma_rx_buffer_ping; 
  else rxbuf = dma_rx_buffer_pong; 
	
  for(ii=0; ii<DMA_BUFFER_SIZE ; ii++)
  {
		//Process
		
	*txbuf++ = *rxbuf++;
  }
  tx_buffer_empty = 0;
  rx_buffer_full = 0;
	}

int main (void) {    //Main function

	gpio_set_mode(P2_10,Output);
  audio_init ( hz48000, mic_in, dma, DMA_HANDLER);

while(1){
	while (!(rx_buffer_full && tx_buffer_empty)){};
	  gpio_set(P2_10, HIGH);
		proces_buffer();
	  gpio_set(P2_10, LOW);
	}
}


