/** 
 * This file is a library to use the audio board with de 
 * Freescale FMDK-K20D50
 * Configures I2C protocol, the CODEC registers, the I2S module,
 * and the interruptions vector.
 */


#ifndef audio_h
#define audio_h

#include <LPC407x_8x_177x_8x.h>
#include <gpio.h>
#include <delay.h>
#include "i2c.h"
#include "i2s.h"
#include "dma.h"
#include "gpdma.h"
#include "audio.h"

	
void init_LED() {  // init leds
gpio_set_mode(LED1, PullUp);
gpio_set_mode(LED2, PullUp);
gpio_set_mode(LED3, PullDown);
gpio_set_mode(LED4, PullDown);
}

typedef struct BITVAL    // used in function prbs()
{
 unsigned short b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1;
 unsigned short b7:1, b8:1, b9:1, b10:1, b11:1, b12:1,b13:1;
 unsigned short dweebie:2; //Fills the 2 bit hole - bits 14-15
} bitval;

typedef union SHIFT_REG
{
 unsigned short regval;
 bitval bt;
} shift_reg;
int fb = 1;                        // feedback variable
shift_reg sreg = {0xFFFF};         // shift register

short prbs(void) 			       //gen pseudo-random sequence {-1,1}
{
  int prnseq;
  if(sreg.bt.b0)
	prnseq = -NOISELEVEL;		           //scaled negative noise level
  else
	prnseq = NOISELEVEL;		           //scaled positive noise level
  fb =(sreg.bt.b0)^(sreg.bt.b1);   //XOR bits 0,1
  fb^=(sreg.bt.b11)^(sreg.bt.b13); //with bits 11,13 -> fb
  sreg.regval<<=1;
  sreg.bt.b0=fb;			       //close feedback path
  return prnseq;			       //return noise sequence value
}


/* Name: Codec_WriteRegister  
* Requires: Device Register address, Data for register  
* Description: Writes the data to the device's register  
*/    
void Codec_WriteRegister ( uint8_t RegisterAddr, uint8_t RegisterValue) {    

uint8_t Byte[2];	
	
Byte[0] = (RegisterAddr<<1) & 0xFF;  //Register address are the 7 MSB of the second byte send. 
Byte[1] = (RegisterValue ) & 0xFF; 
  delay_ms(1);  
       
i2c_write(CODEC_DEVICE_ADDRESS, Byte, 2);

}   
  
/*--------initialization function--------------------------------
* Name: wm8731_init  
* 
* Configure the freescale and the CODEC
*
* Requires: 
*
*   -Sampling rate; hz8000, hz32000, hz48000, hz96000
*   -Input; line_in, mic_in;
*   -Mode; inter, dma 
* 
* Description: 
* Configure the I2C protocol
* Write with I2C protocol into the CODEC registers  to configure it. 
* Configure the I2S protocol
* Configure the interruptions for the I2S RX FIFO and TX FIFO
*/ 

void audio_init ( char sampling_rate, char audio_input, char mode, void (*handler)(void)) {    
 
	
i2c_init(); //Configure the I2C module in the Freescale

/*Configure wm8731 as master I2S, 16 bits, 256fs.
TIP; WM8731 registers are 9 bits. MSB have to be write on the LSB of the register address but, because in our configurations this bit is always 0, this program don’t write it*/

Codec_WriteRegister ( WM8731_RESET, 0x00);   					//Reset
Codec_WriteRegister ( WM8731_LLINEIN, 0x17); 					//Left Line In. Select gain 0dB.
Codec_WriteRegister ( WM8731_RLINEIN, 0x17); 					//Right Line In. Select gain 0dB.
Codec_WriteRegister ( WM8731_LHEADOUT, 0x6F);					//Left Headphone Out. Select volume; 7F=+6dB, 30=-73dB; 79 steps, 1dB each.  
Codec_WriteRegister ( WM8731_RHEADOUT, 0x6F);					//Right Headphone Out (6F = -9dB).
Codec_WriteRegister ( WM8731_ANALOG, audio_input); 		//Select input; LINE in or microphone in. 
Codec_WriteRegister ( WM8731_DIGITAL, 0x00); 					//Audio Path Control, no filters, no soft mute.
Codec_WriteRegister ( WM8731_POWERDOWN, 0x00); 				//Power control ->Disable power down; all on.
Codec_WriteRegister ( WM8731_INTERFACE, 0x52); 				//Format; 16 bits; I2S data format. Right channel when tx-sync is high.
Codec_WriteRegister ( WM8731_SAMPLING, sampling_rate); //Sample Control; Mclk, 256fs, normal mode, and sampling frequency. 
Codec_WriteRegister ( WM8731_CONTROL, 0x01); 					//Digital Activation.
		
if (mode == dma){
    i2s_init();   //Configure I2S 
	
	  dma_init();
	
    NVIC_DisableIRQ (DMA_IRQn);     // Disable interrupt for DMA
 //   NVIC_SetPriority(DMA_IRQn, ((0x01<<3)|0x01));	 // Interrupt priority
	
//Configure DMA Chanel 0 for I2S Transmit
    dma_setup(0, (uint32_t)&dma_tx_buffer_ping,0, 0, (uint32_t)DMA_I2S_Channel_0, (uint32_t)DMA_BUFFER_SIZE, (uint32_t)GPDMA_BSIZE_32, (uint32_t)GPDMA_WIDTH_WORD, (uint32_t)GPDMA_TRANSFERTYPE_M2P,  0 );
//Configure DMA Chanel 1 for I2S Recive	
    dma_setup(1, 0,(uint32_t)&dma_rx_buffer_ping, (uint32_t)DMA_I2S_Channel_1, 0, (uint32_t)DMA_BUFFER_SIZE, (uint32_t)GPDMA_BSIZE_32, (uint32_t)GPDMA_WIDTH_WORD, (uint32_t)GPDMA_TRANSFERTYPE_P2M,  0 );
	
	  dma_enable(0);
	  dma_enable(1);
	
   // NVIC_EnableIRQ (DMA_IRQn); // Enable interrupt for DMA
		dma_set_callback(handler);
	
	  i2s_rx_depth_dma2(4);
	  i2s_tx_depth_dma1(4);
	
		i2s_rx_dma2_enable();
		i2s_tx_dma1_enable();
		
		i2s_start();		
  }
	
else{ //Mode interruptions
   i2s_init();   //Configure I2S

   i2s_tx_depth_irq(1); //FIFO level on wich a TX irq request is created
   i2s_rx_depth_irq(1); //FIFO level on wich a RX irq request is created
   i2s_tx_irq_enable();  //Enable I2S Transmit Interrupt
   i2s_rx_irq_enable();  //Enable I2S Receive Interrupt
		
   i2s_start();
	 i2s_set_callback(handler);
	
  // NVIC_ClearPendingIRQ(I2S_IRQn);
  // NVIC_EnableIRQ(I2S_IRQn);
  //NVIC_SetPriority(I2S_IRQn, 0);   
	
	
}
}



#endif //audio
