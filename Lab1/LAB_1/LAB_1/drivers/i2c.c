
// IF YOU ARE USING THE QSB BASE BOARD BE SURE THAT JUMPERS JP8 & JP9 ARE AT 2-3 POSITION

#include <LPC407x_8x_177x_8x.h>
#include "platform.h"
#include "i2c.h"
#include "delay.h"

//I2C write o read
#define WRITE 0x00 // Master write     
#define READ 0x01 // Master read 

/* Name: i2c_enable  
* Description: Enable i2c module
*/  
void i2c_enable(){
LPC_I2C0->CONSET = 0x40; // Enable I2C
}

/* Name: i2c_Start  
* Description: Send the start signal (master)
*/  
void i2c_start(){

LPC_I2C0->CONSET =0x60;  //transmit start
}

/* Name: i2c_Stop  
* Description: Make the stop signal (master)
* The master disable any transmissions.
*/  
void i2c_stop(){

LPC_I2C0->CONSET = 0x10; // Desable transmission
LPC_I2C0->CONCLR = 0x08; //Clear SI

}

/* Name: i2c_tx     
* Description: Puts data in de data register for send.  
*/
void i2c_tx(unsigned char c){
	LPC_I2C0->DAT = c;  
}

/* Name: i2c_rx     
* Description: Reads data from the data register  
*/

unsigned char i2c_rx(void){
return 	LPC_I2C0->DAT ;
}


/* Name: i2c_ack     
* Description: Reads data from the data register  
*/

void i2c_ack(){
	LPC_I2C0->CONSET = 0x04;   
}

 //**********************************************************************************************
/* Name: i2c_init   
* Description: Initialization I2C bus clock and Pins.   
* Transmition frequency 100KHz. 
* I2C0 SDA P1_30
* I2C0_SCK P1_31
*/    

void i2c_init() {   

LPC_SC->PCONP |= (1 << 7);  //Power control pheriperals For I2C0
LPC_SC->PCLKSEL = 4; 
i2c_enable();
// Clock frequency = (CLK/PCLKSEL)/(I2CSCLH+I2CSCLL) = 30M/300 = 100khz standard frequency clock.
LPC_I2C0->SCLH = 0x96; // CSCLH = 150
LPC_I2C0->SCLL = 0x96; // CSCLH = 150

// Configure GPIO’s     
LPC_IOCON -> P1_30 = 0xA4;  //I2C0 SDA  
LPC_IOCON -> P1_31 = 0xA4;  //I2C0_SCK
	
}

/* Name: i2c_write    
* Description: Write buff_len number of bytes to the addressed slave device. 
*/
void i2c_write(uint8_t address, uint8_t *buffer, int buff_len) {
 int state = 0;	
 int i = 0;	
  address = (address << 1)| WRITE;  //7 bits address and one 0 to indicate we want to write on de CODEC 
   
  i2c_start();
	while(state != 0x08){    //Wait until the start signal have been recived

	state = LPC_I2C0->STAT ;
	}
	i2c_tx(address);
	LPC_I2C0->CONCLR = 0x28; //Clear SI and start flag
	
 	while(state != 0x18){    //Wait until the addres has been recived	
	state = LPC_I2C0->STAT;
	}	

	i2c_tx(buffer[0]);
	LPC_I2C0->CONCLR = 0x18; //Clear SI and start flag
	
	while(state != 0x28){    //Wait until the register address has been recived
	state = LPC_I2C0->STAT;	}	
	
 for (i = 1; i < buff_len; i++) {	
	i2c_tx(buffer[i]);
	i2c_ack();
	LPC_I2C0->CONCLR = 0x18;   //Clear SI and start flag
	state = 0;
	while(state != 0x28){      //Wait until the data has been transmited
	state = LPC_I2C0->STAT;	}
	}
  i2c_stop();     		  // send Stop signal    
  delay_ms(10);        //wait 
}


/* Name: i2c_read    
* Description:Read buff_len number of bytes from the addressed slave device. 
*/
void i2c_read(uint8_t address, uint8_t *buffer, int buff_len) {

}

// *******************************ARM University Program Copyright © ARM Ltd 2014*************************************   
