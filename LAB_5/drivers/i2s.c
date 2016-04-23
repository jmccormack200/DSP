
#include <LPC407x_8x_177x_8x.h>
#include "platform.h"
#include "i2s.h"



/* Name: i2s_init   
* Description: Initialization I2S bus clock and Pins.   
* Pins ready to interact with the QSB BASE BOARD
* I2S RX SCK P0_4
* I2S RX WS  P0_5
* I2S RX SDA P0_6
* I2S TX SCK P0_7
* I2S TX WS  P0_8
* I2S TX SDA P0_9
* 
* Configuration: Slave, 16 bits data, Stereo
*/    

void i2s_init() {   
// Pin configuration
    //D IOCON
    LPC_IOCON -> P0_4 |= (1 << 0);  //I2S RX SCK 
    LPC_IOCON -> P0_5 |= (1 << 0);  //I2S RX WS
    LPC_IOCON -> P0_6 |= (1 << 0);  //I2S RX SDA
    //W IOCON
    LPC_IOCON -> P0_7 |= (1 << 0);  //I2S TX SCK
    LPC_IOCON -> P0_7 |= (1 << 4);  //Enable pull-up resistor
    LPC_IOCON -> P0_8 |= (1 << 0);  //I2S TX WS
    LPC_IOCON -> P0_8 |= (1 << 4);
    LPC_IOCON -> P0_9 |= (1 << 0);  //I2S TX SDA
    LPC_IOCON -> P0_9 |= (1 << 4);
    

    LPC_SC->PCONP |= (1 << 27);  //Power control pheriperals
    
    LPC_I2S->DAI = 0x03F9;  //16 bits data, Stereo, Stop On, Resset On, Slave mode, Wordwidth 16 bits 
		LPC_I2S->DAO = 0x03F9;  //16 bits data, Stereo, Stop On, Resset On, Slave mode, Wordwidth 16 bits and  no MUTE

    LPC_I2S->TXMODE = 0x00 ;  // Clock source, disable MCLK
    LPC_I2S->RXMODE = 0x00 ;
		
    LPC_SC->PCLKSEL |= 0x00; //Turned off clock to APB1 pheripherals. No clock needed in slave mode

		LPC_I2S->TXBITRATE = 0x00;
		LPC_I2S->RXBITRATE = 0x00;


}

void i2s_rx_irq_enable(){
   LPC_I2S->IRQ |=(1UL << 0);  //Enable I2S Receive Interrupt
}

void i2s_tx_irq_enable(){
   LPC_I2S->IRQ |=(1UL << 1);  //Enable I2S Transmit Interrupt
}

void i2s_rx_depth_irq(int d){
   LPC_I2S->IRQ |=((d &(0x0F)) << 8);  //FIFO level on wich irq request is created
}

void i2s_tx_depth_irq(int d){
   LPC_I2S->IRQ |=((d &(0x0F)) << 16);  //FIFO level on wich irq request is created
}

void i2s_rx_dma1_enable(){
	 LPC_I2S->DMA1 |=(1UL << 0);  //Enable I2S receive DMA request
}

void i2s_tx_dma1_enable(){
   LPC_I2S->DMA1 |=(1UL << 1);  //Enable I2S transmit DMA request
}

void i2s_rx_depth_dma1(int d){
   LPC_I2S->DMA1 |=((d &(0x0F)) << 8);  //FIFO level on wich irq request is created
}

void i2s_tx_depth_dma1(int d){
   LPC_I2S->DMA1 |=((d &(0x0F)) << 16);  //FIFO level on wich irq request is created
}

void i2s_rx_dma2_enable(){
	 LPC_I2S->DMA2 |=(1UL << 0);  //Enable I2S receive DMA request
}

void i2s_tx_dma2_enable(){
   LPC_I2S->DMA2 |=(1UL << 1);  //Enable I2S transmit DMA request
}

void i2s_rx_depth_dma2(int d){
   LPC_I2S->DMA2 |=((d &(0x0F)) << 8);  //FIFO level on wich irq request is created
}

void i2s_tx_depth_dma2(int d){
   LPC_I2S->DMA2 |=((d &(0x0F)) << 16);  //FIFO level on wich irq request is created
}

void i2s_start(void){
    //Clear STOP,RESET and MUTE bit
    LPC_I2S->DAO &= ~((uint32_t)(1<<4));
    LPC_I2S->DAI &= ~((uint32_t)(1<<4));
    LPC_I2S->DAO &= ~((uint32_t)(1<<3));
    LPC_I2S->DAI &= ~((uint32_t)(1<<3));
    LPC_I2S->DAO &= ~((uint32_t)(1<<15));
}

void i2s_stop(void){
    //Clear STOP,RESET and MUTE bit
    LPC_I2S->DAO |= ((uint32_t)(1<<4));
    LPC_I2S->DAI |= ((uint32_t)(1<<4));
    LPC_I2S->DAO |= ((uint32_t)(1<<3));
    LPC_I2S->DAI |= ((uint32_t)(1<<3));
    LPC_I2S->DAO |= ((uint32_t)(1<<15));
}

uint32_t i2s_rx(void){
return 	LPC_I2S->RXFIFO;	//Reads data from the data register  
}

void i2s_tx(uint32_t c){
	LPC_I2S->TXFIFO = c;  
}


static void (*i2s_callback)(void) = 0;

void i2s_set_callback(void (*callback)(void)) {
	i2s_callback = callback;

   NVIC_ClearPendingIRQ(I2S_IRQn);
   NVIC_EnableIRQ(I2S_IRQn);
   NVIC_SetPriority(I2S_IRQn, 3);  
}

 void I2S_IRQHandler(void) {
	if (i2s_callback) {
		i2s_callback();
	}
}

// *******************************ARM University Program Copyright © ARM Ltd 2014*************************************   
