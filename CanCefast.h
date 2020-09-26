/*
  CanCefast.h
  Autor: Everton A. Gomes
  Data: 23/05/2020
*/
/* DISCLAIMER:
 * All functions used in this code are documented in http://asf.atmel.com/docs/latest/sam3x/html/sam_can_quickstart.html.
 * They can be found in hidden folders in your computer. For example, the can.h file, available in
 * C:\Users\ealve\AppData\Local\Arduino15\packages\arduino\hardware\sam\1.6.12\system\libsam\include
 * in the machine where this code was built, has declarations of the functions used in this code.
 * Please check the location of this file in your computer. Don't forget to enable hidden folders visualization.
 */
 
#ifndef _CAN_CEFAST_H
#define _CAN_CEFAST_H

//********************************************************************************
//                             Registradores
//********************************************************************************

//CAN Registers: address and pointers
#define CAN_IER		0x400B4004		//CAN Interrupt Enable Register
//#define CAN_SR		0x400B4010		//CAN Status Register
#define CAN_MSR_0	0x400B4210		//CAN Message Status Register - Mailbox 0
#define CAN_MSR_1	0x400B4230		//CAN Message Status Register - Mailbox 1
#define CAN_MSR_2	0x400B4250		//CAN Message Status Register - Mailbox 2
#define CAN_MSR_3	0x400B4270		//CAN Message Status Register - Mailbox 3
#define CAN_MSR_4	0x400B4290		//CAN Message Status Register - Mailbox 4
#define CAN_MSR_5	0x400B42B0		//CAN Message Status Register - Mailbox 5
#define CAN_MSR_6	0x400B42D0		//CAN Message Status Register - Mailbox 6
#define CAN_MSR_7	0x400B42F0		//CAN Message Status Register - Mailbox 7


uint32_t *pCAN_IER = (uint32_t*)(CAN_IER);
uint32_t *pCAN_SR = (uint32_t*)(0x400B4010);
uint32_t *pCAN_MSR_0 = (uint32_t*)(CAN_MSR_0);
uint32_t *pCAN_MSR_1 = (uint32_t*)(CAN_MSR_1);
uint32_t *pCAN_MSR_2 = (uint32_t*)(CAN_MSR_2);
uint32_t *pCAN_MSR_3 = (uint32_t*)(CAN_MSR_3);
uint32_t *pCAN_MSR_4 = (uint32_t*)(CAN_MSR_4);
uint32_t *pCAN_MSR_5 = (uint32_t*)(CAN_MSR_5);
uint32_t *pCAN_MSR_6 = (uint32_t*)(CAN_MSR_6);
uint32_t *pCAN_MSR_7 = (uint32_t*)(CAN_MSR_7);


//********************************************************************************
//                             #defines
//********************************************************************************

#define CONTRL_CAN_0  //comment if CAN0 is not used
//#define CONTRL_CAN_1 //comment if CAN1 is not used

#define TIMEOUT_CAN		20000	//initialization timeout for CAN module
#define CAN0_MID_MB5	0x400B42A8


//IDs das mensagens:
#define ID_MSG_1	0x2702	//pressD | tempD | velD
#define ID_MSG_2	0x3066	//pressT | tempT | rpm | velT
#define ID_MSG_3	0xEA27	//comb


uint32_t *mb5_mid = (uint32_t*)(CAN0_MID_MB5); //MID register from mailbox 5 of CAN0 controller	


//********************************************************************************
//                             Variaveis
//********************************************************************************

//Estrutura da mailbox
/*
typedef struct {
	uint32_t ul_mb_idx;
	uint8_t uc_obj_type;  //! Mailbox object type, one of the six different objects.
	uint8_t uc_id_ver;    //! 0 stands for standard frame, 1 stands for extended frame.
	uint8_t uc_length;    //! Received data length or transmitted data length.
	uint8_t uc_tx_prio;   //! Mailbox priority, no effect in receive mode.
	uint32_t ul_status;   //! Mailbox status register value.
	uint32_t ul_id_msk;   //! No effect in transmit mode.
	uint32_t ul_id;       //! Received frame ID or the frame ID to be transmitted.
	uint32_t ul_fid;      //! Family ID.
	uint32_t ul_datal;
	uint32_t ul_datah;
} can_mb_conf_t;*/

//********************************************************************************
//                             Funcoes
//********************************************************************************


void configureCanInterrupt(){
	
	NVIC_EnableIRQ(CAN0_IRQn);  //Enables CAN interrupt line
	NVIC_SetPriority(CAN0_IRQn,12); //CAN interrupt priority
	
}


//Habilita interrupções do CAN
void can_enable_interrupts(uint32_t mask){
	*pCAN_IER = mask;
	
}


//Initializes CAN module with 250 kbps baudrate 
void startCan(){
  
  uint32_t ul_sysclk = SystemCoreClock; //84 MHz
  
  #ifdef CONTRL_CAN_0
  
  pmc_enable_periph_clk(ID_CAN0); //Enabling clock for CAN peripherals
  can_init(CAN0, ul_sysclk, CAN_BPS_250K); //Initialize CAN0 with 250 kbit/s baudrate:
  can_reset_all_mailbox(CAN0); //Resetting all mailboxes
  #endif
  #ifdef CONTRL_CAN_1
  pmc_enable_periph_clk(ID_CAN1);
  can_init(CAN1, ul_sysclk, CAN_BPS_250K);
  can_reset_all_mailbox(CAN1);
  #endif
  
  configureCanInterrupt(); //Enables CAN interrupt line

  
}

//Configures a mailbox as tx
void canConfigMailboxTx(can_mb_conf_t* mb, uint8_t index){
  mb->ul_mb_idx = index; 
  mb->uc_obj_type = CAN_MB_TX_MODE;
  mb->uc_tx_prio = 15;
  mb->uc_id_ver = 0;
  mb->ul_id_msk = 0;
  can_mailbox_init(CAN0, mb);
}


//Configures a mailbox as rx
void canConfigMailboxRx(can_mb_conf_t* mb, uint8_t index, uint32_t id){
  mb->ul_mb_idx = index; 
  mb->uc_obj_type = CAN_MB_RX_MODE;
  //#define CAN_MAM_MIDvA_Msk (0x7ffu << CAN_MAM_MIDvA_Pos)
  mb->ul_id_msk = CAN_MAM_MIDvA_Msk | CAN_MAM_MIDvB_Msk;
  //CAN_MAM_MIDvA_Msk = (0x7ffu << 18) -> all bits are 1
  //CAN_MID_MIDvB_Msk (0x3ffffu) -> all bits are 1
  mb->ul_id = CAN_MID_MIDvA(id);
  can_mailbox_init(CAN0, mb);
}

void canConfigMailboxRx(can_mb_conf_t* mb, uint8_t index, uint32_t id, uint32_t mask_vA){
  mb->ul_mb_idx = index; 
  mb->ul_id_msk = (mask_vA << CAN_MAM_MIDvA_Pos) | CAN_MAM_MIDvB_Msk;
  mb->ul_id = CAN_MID_MIDvA(id);
  mb->uc_obj_type = CAN_MB_RX_OVER_WR_MODE;
  can_mailbox_init(CAN0, mb);
}	

/*
//Configura uma mailbox mb para operar com o id "id" no modo "mode"
void canConfigMailbox(can_mb_conf_t* mb, uint8_t index, uint32_t id, uint8_t mode){
  mb->ul_mb_idx = index; 
  mb->uc_obj_type = mode;
  mb->uc_id_ver = 0; 
  mb->ul_id_msk = CAN_MAM_MIDvA_Msk | CAN_MAM_MIDvB_Msk; //mask de aceitação
  mb->ul_id = CAN_MID_MIDvA(id); 
  can_mb_conf_t_init(CAN0, mb);
}*/

//Configura o nível de prioridade da mailbox (relevante apenas para tx)
//Sets the mailbox priotity level (relevant only for tx mailboxes)
void canConfigPriority(can_mb_conf_t* mb, uint8_t priority){
  mb->uc_tx_prio = priority;
}

/*uint8_t canSend(can_mb_conf_t* mb, uint32_t id){
  uint8_t retval;
  //Preparing transmit ID, data and data length code in CAN0 mailbox 0:
  mb->ul_id = CAN_MID_MIDvA(id); //ID is the same CAN1 mailbox 0's one
  mb->ul_datal = 0x024AA512; //low-part of data to be transmitted
  mb->ul_datah = 0x00ABCDEF; //high-part of data to be transmitted
  mb->uc_length = 8; //data length
  retval = can_mb_conf_t_write(CAN0, mb); //send mailbox to CAN bus
  return retval;
}*/

uint8_t canSend(can_mb_conf_t* mb, uint32_t id, uint32_t data){
  uint8_t retval;
  mb->ul_id = CAN_MID_MIDvA(id); //ID is the same CAN1 mailbox 0's one
  mb->ul_datal = data;
  mb->ul_datah = 0;
  mb->uc_length = sizeof(data);
  retval = can_mailbox_write(CAN0, mb); //send mailbox to CAN bus
  return retval;
}

uint8_t canSend(can_mb_conf_t* mb, uint32_t id, uint32_t data_high, uint32_t data_low){
  uint8_t retval;
  mb->ul_id = CAN_MID_MIDvA(id); //ID is the same CAN1 mailbox 0's one
  mb->ul_datal = data_low;
  mb->ul_datah = data_high;
  //mb->uc_length = sizeof(data_low)+sizeof(data_high);
  mb->uc_length = sizeof(data_low) + sizeof(data_high);
  retval = can_mailbox_write(CAN0, mb); //send mailbox to CAN bus
  return retval;
}


#endif
