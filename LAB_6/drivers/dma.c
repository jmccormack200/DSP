
#include <LPC407x_8x_177x_8x.h>
#include "platform.h"
#include "dma.h"
#include "gpdma.h"



/**
 * @brief Lookup Table of Connection Type matched with
 * Peripheral Data (FIFO) register base address */

 
volatile const void *DMA_LUTPerAddr[] = {
        0,                              // Revered
        (&LPC_MCI->FIFO),               // SD Card
        (&LPC_SSP0->DR),                // SSP0 Tx
        (&LPC_SSP0->DR),                // SSP0 Rx
        (&LPC_SSP1->DR),                // SSP1 Tx
        (&LPC_SSP1->DR),                // SSP1 Rx
        (&LPC_SSP2->DR),                // SSP2 Tx
        (&LPC_SSP2->DR),                // SSP2 Rx
        (&LPC_ADC->GDR),                // ADC
        (&LPC_DAC->CR),                 // DAC
        (&LPC_UART0->THR),              // UART0 Tx
        (&LPC_UART0->RBR),              // UART0 Rx
        (&LPC_UART1->THR),              // UART1 Tx
        (&LPC_UART1->RBR),              // UART1 Rx
        (&LPC_UART2->THR),              // UART2 Tx
        (&LPC_UART2->RBR),              // UART2 Rx
        (&LPC_TIM0->MR0),               // MAT0.0
        (&LPC_TIM0->MR1),               // MAT0.1
        (&LPC_TIM1->MR0),               // MAT1.0
        (&LPC_TIM1->MR1),               // MAT1.1
        (&LPC_TIM2->MR0),               // MAT2.0
        (&LPC_TIM2->MR1),               // MAT2.1
        (&LPC_I2S->TXFIFO),             // I2S Tx
        (&LPC_I2S->RXFIFO),             // I2S Rx
        0,                              // Reverse
        0,                              // Reverse
        (&LPC_UART3->THR),              // UART3 Tx
        (&LPC_UART3->RBR),              // UART3 Rx
        (&LPC_UART4->THR),              // UART4 Tx
        (&LPC_UART4->RBR),              // UART4 Rx
        (&LPC_TIM3->MR0),               // MAT3.0
        (&LPC_TIM3->MR1),               // MAT3.1
};

/*! \brief Reads the interruption status*/					 
uint32_t dma_state(unsigned char ChannelNum){
return (LPC_GPDMA->IntStat & (1UL << (ChannelNum)));
};								 
/*! \brief Clean the interrupt requests. */						 
void dma_clean(unsigned char ChannelNum){
		LPC_GPDMA->IntTCClear |= (1 << ChannelNum);
};		
/*! \brief Rewrite the memory source address */						 
void dma_src_memory(unsigned char ChannelNum, unsigned int address){
    LPC_GPDMACH_TypeDef *pDMAch;
    pDMAch = ((LPC_GPDMACH_TypeDef *) (LPC_GPDMACH0_BASE + 0x20 * (ChannelNum)));
		pDMAch->CSrcAddr = address;	
};		

/*! \brief Rewrite the memory destination address */							 
void dma_dest_memory(unsigned char ChannelNum, unsigned int address){
    LPC_GPDMACH_TypeDef *pDMAch;
    pDMAch = ((LPC_GPDMACH_TypeDef *) (LPC_GPDMACH0_BASE + 0x20 * (ChannelNum)));
		pDMAch->CDestAddr =  address;	
};			

/*! \brief Write the data transfer size. */							 
void dma_transfersize(unsigned char ChannelNum, unsigned int size){
    LPC_GPDMACH_TypeDef *pDMAch;
    pDMAch = ((LPC_GPDMACH_TypeDef *) (LPC_GPDMACH0_BASE + 0x20 * (ChannelNum)));
		pDMAch->CControl |= (((((uint32_t) size )&0xFFF)<<0));	
	};		

/* Name: dma_init   
* Description: Initialization DMA   
*/    

void dma_init(void) {   
    /* Enable GPDMA clock */
LPC_SC->PCONP |= (1 << 29);  //Enable PCGPDMA function power/clock control bit

    // Reset all channel configuration register
    LPC_GPDMACH0->CConfig = 0;
    LPC_GPDMACH1->CConfig = 0;
    LPC_GPDMACH2->CConfig = 0;
    LPC_GPDMACH3->CConfig = 0;
    LPC_GPDMACH4->CConfig = 0;
    LPC_GPDMACH5->CConfig = 0;
    LPC_GPDMACH6->CConfig = 0;
    LPC_GPDMACH7->CConfig = 0;

    /* Clear all DMA interrupt and error flag */
    LPC_GPDMA->IntTCClear = 0xFF;
    LPC_GPDMA->IntErrClr = 0xFF;
	
		LPC_GPDMA->Config = 0x01;
}

void dma_setup(char ChannelNum, unsigned int SrcMemAddr,unsigned int DstMemAddr, unsigned int SrcPeriph, unsigned int DstPeriph, unsigned int TransferSize, unsigned int BurstSize, unsigned int TransferWidth, unsigned int TransferType,  unsigned int Dmalli  ){
	
    LPC_GPDMACH_TypeDef *pDMAch;

    // Get Channel pointer
    pDMAch = ((LPC_GPDMACH_TypeDef *) (LPC_GPDMACH0_BASE + 0x20 * (ChannelNum)));
     
    // Reset the Interrupt status
    LPC_GPDMA->IntTCClear = GPDMA_DMACIntTCClear_Ch(ChannelNum);
    LPC_GPDMA->IntErrClr = GPDMA_DMACIntErrClr_Ch(ChannelNum);

    // Clear DMA configure
    pDMAch->CControl = 0x00;
    pDMAch->CConfig = 0x00;

    /* Assign Linker List Item value */
    pDMAch->CLLI = Dmalli;	
	
    switch (TransferType)
    {
    // Memory to memory
    case GPDMA_TRANSFERTYPE_M2M:
        // Assign physical source and destination address
        pDMAch->CSrcAddr = SrcMemAddr;
        pDMAch->CDestAddr = DstMemAddr;
        pDMAch->CControl  = 
		                      GPDMA_DMACCxControl_TransferSize(TransferSize) \
                        | GPDMA_DMACCxControl_SBSize(BurstSize) \
                        | GPDMA_DMACCxControl_DBSize(BurstSize) \
                        | GPDMA_DMACCxControl_SWidth(TransferWidth) \
                        | GPDMA_DMACCxControl_DWidth(TransferWidth) \
                        | GPDMA_DMACCxControl_SI \
                        | GPDMA_DMACCxControl_DI \
                        | GPDMA_DMACCxControl_I;
        break;
    // Memory to peripheral
    case GPDMA_TRANSFERTYPE_M2P:
    case GPDMA_TRANSFERTYPE_M2P_DEST_CTRL:
        // Assign physical source
        pDMAch->CSrcAddr = SrcMemAddr;
        // Assign peripheral destination address
        pDMAch->CDestAddr = (uint32_t)DMA_LUTPerAddr[DstPeriph]; 
        pDMAch->CControl = 
													GPDMA_DMACCxControl_TransferSize((uint32_t)TransferSize) \
                        | GPDMA_DMACCxControl_SBSize((uint32_t)BurstSize) \
                        | GPDMA_DMACCxControl_DBSize((uint32_t)BurstSize) \
                        | GPDMA_DMACCxControl_SWidth((uint32_t)TransferWidth) \
                        | GPDMA_DMACCxControl_DWidth((uint32_t)TransferWidth) \
                        | GPDMA_DMACCxControl_SI \
                        | GPDMA_DMACCxControl_I;
        break;
    // Peripheral to memory
    case GPDMA_TRANSFERTYPE_P2M:
    case GPDMA_TRANSFERTYPE_P2M_SRC_CTRL:
        // Assign peripheral source address
        pDMAch->CSrcAddr = (uint32_t)DMA_LUTPerAddr[SrcPeriph];
        // Assign memory destination address
        pDMAch->CDestAddr = DstMemAddr;
        pDMAch->CControl  = 
													GPDMA_DMACCxControl_TransferSize((uint32_t)TransferSize) \
                        | GPDMA_DMACCxControl_SBSize((uint32_t)BurstSize) \
                        | GPDMA_DMACCxControl_DBSize((uint32_t)BurstSize) \
                        | GPDMA_DMACCxControl_SWidth((uint32_t)TransferWidth) \
                        | GPDMA_DMACCxControl_DWidth((uint32_t)TransferWidth) \
                        | GPDMA_DMACCxControl_DI \
                        | GPDMA_DMACCxControl_I;
        break;
    // Peripheral to peripheral
    case GPDMA_TRANSFERTYPE_P2P:
        // Assign peripheral source address
       pDMAch->CSrcAddr = (uint32_t)DMA_LUTPerAddr[SrcPeriph];
        // Assign peripheral destination address
       pDMAch->CDestAddr = (uint32_t)DMA_LUTPerAddr[DstPeriph];

       pDMAch->CControl = GPDMA_DMACCxControl_TransferSize((uint32_t)TransferSize) \
                        | GPDMA_DMACCxControl_SBSize((uint32_t)BurstSize) \
                        | GPDMA_DMACCxControl_DBSize((uint32_t)BurstSize) \
                        | GPDMA_DMACCxControl_SWidth((uint32_t)TransferWidth) \
                        | GPDMA_DMACCxControl_DWidth((uint32_t)TransferWidth) \
                        | GPDMA_DMACCxControl_I;
        break;
     }	
//Configure DAM Request Select register
    if((SrcPeriph != 8)&&(SrcPeriph != 9))
    {
        if (SrcPeriph > 15)
        {
            LPC_SC->DMAREQSEL |= (1<< (SrcPeriph - 16));
        } else {
            LPC_SC->DMAREQSEL &= ~(1<<(SrcPeriph));
        }
    }
    if((DstPeriph != 8)&&(DstPeriph != 9))
    {
        if (DstPeriph > 15)
        {
            LPC_SC->DMAREQSEL |= (1<< (DstPeriph - 16));
        } else {
            LPC_SC->DMAREQSEL &= ~(1<<(DstPeriph));
        }
    }

    /* Enable DMA channels, little endian */
    LPC_GPDMA->Config = (0x01);
    while (!(LPC_GPDMA->Config & 0x01));
		
    // Calculate DMA connection
if (SrcPeriph > 15) {SrcPeriph = SrcPeriph -16;}
if (DstPeriph > 15) {DstPeriph = DstPeriph -16;}
		
//Configure GPDMA Config register
   pDMAch->CConfig = GPDMA_DMACCxConfig_ITC 
									 | GPDMA_DMACCxConfig_TransferType((uint32_t)TransferType) \
									 | GPDMA_DMACCxConfig_SrcPeripheral(SrcPeriph) \
									 | GPDMA_DMACCxConfig_DestPeripheral(DstPeriph);		
	}

/* Name: dma_enable  
* Description: Enable the chosen dma channel
*/  
	void dma_enable(unsigned char ChannelNum){
		
    LPC_GPDMACH_TypeDef *pDMAch;
		//Get channel pointer
		pDMAch = ((LPC_GPDMACH_TypeDef *) (LPC_GPDMACH0_BASE + 0x20 * (ChannelNum)));
		pDMAch->CConfig |= GPDMA_DMACCxConfig_E;
	}		
	
/* Name: dma_disable  
* Description: Disable the chosen dma channel
*/  
	void dma_disable(unsigned char ChannelNum){
		
    LPC_GPDMACH_TypeDef *pDMAch;
		//Get channel pointer
		pDMAch = ((LPC_GPDMACH_TypeDef *) (LPC_GPDMACH0_BASE + 0x20 * (ChannelNum)));
    pDMAch->CConfig &= ~GPDMA_DMACCxConfig_E;
	}		
	
	static void (*dma_callback)(void) = 0;

void dma_set_callback(void (*callback)(void)) {
	dma_callback = callback;

   NVIC_ClearPendingIRQ(DMA_IRQn);
   NVIC_EnableIRQ(DMA_IRQn);
   NVIC_SetPriority(DMA_IRQn, 3);  
}

//Interruption call the callback function 

 void DMA_IRQHandler(void) {
	if (dma_callback) {
		dma_callback();
	}
}
	
// *******************************ARM University Program Copyright © ARM Ltd 2014*************************************   
