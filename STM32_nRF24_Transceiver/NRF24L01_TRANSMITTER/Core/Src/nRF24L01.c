/*
 * nRF24L01.c
 *
 *  Created on: Oct 2, 2024
 *      Author: ANAS ALSAYAR
 */


#include "NRF24L01.h"
#include "stm32f4xx_hal.h"
#include "stdio.h"

extern SPI_HandleTypeDef hspi1;
#define NRF24_SPI &hspi1

#define Chip_Enable_PORT   GPIOA
#define Chip_Enable_PIN    GPIO_PIN_3

#define Chip_Select_PORT   GPIOA
#define Chip_Select_PIN    GPIO_PIN_4


void CS_Select (void)
{
	HAL_GPIO_WritePin(Chip_Select_PORT, Chip_Select_PIN, GPIO_PIN_RESET);
}

void CS_UnSelect (void)
{
	HAL_GPIO_WritePin(Chip_Select_PORT, Chip_Select_PIN, GPIO_PIN_SET);
}


void CE_Enable (void)
{
	HAL_GPIO_WritePin(Chip_Enable_PORT, Chip_Enable_PIN, GPIO_PIN_SET);
}

void CE_Disable (void)
{
	HAL_GPIO_WritePin(Chip_Enable_PORT, Chip_Enable_PIN, GPIO_PIN_RESET);
}



// write a single byte to the particular register
void nrf24_WriteReg (uint8_t Reg, uint8_t Data)
{
	uint8_t buf[2];
	buf[0] = Reg|1<<5;
	buf[1] = Data;

	// Pull the CS Pin LOW to select the device
	CS_Select();

	HAL_SPI_Transmit(NRF24_SPI, buf, 2, 1000);

	// Pull the CS HIGH to release the device
	CS_UnSelect();
}

//write multiple bytes starting from a particular register
void nrf24_WriteRegMulti (uint8_t Reg, uint8_t *data, int size)
{
	uint8_t buf[2];
	buf[0] = Reg|1<<5;
//	buf[1] = Data;

	// Pull the CS Pin LOW to select the device
	CS_Select();

	HAL_SPI_Transmit(NRF24_SPI, buf, 1, 100);
	HAL_SPI_Transmit(NRF24_SPI, data, size, 1000);

	// Pull the CS HIGH to release the device
	CS_UnSelect();
}


uint8_t nrf24_ReadReg (uint8_t Reg)
{
	uint8_t data=0;

	// Pull the CS Pin LOW to select the device
	CS_Select();

	HAL_SPI_Transmit(NRF24_SPI, &Reg, 1, 100);
	HAL_SPI_Receive(NRF24_SPI, &data, 1, 100);

	// Pull the CS HIGH to release the device
	CS_UnSelect();

	return data;
}


/* Read multiple bytes from the register */
void nrf24_ReadReg_Multi (uint8_t Reg, uint8_t *data, int size)
{
	// Pull the CS Pin LOW to select the device
	CS_Select();

	HAL_SPI_Transmit(NRF24_SPI, &Reg, 1, 100);
	HAL_SPI_Receive(NRF24_SPI, data, size, 1000);

	// Pull the CS HIGH to release the device
	CS_UnSelect();
}


// send the command to the NRF
void nrfsendCmd (uint8_t cmd)
{
	// Pull the CS Pin LOW to select the device
	CS_Select();

	HAL_SPI_Transmit(NRF24_SPI, &cmd, 1, 100);

	// Pull the CS HIGH to release the device
	CS_UnSelect();
}
//
//void nrf24_reset(uint8_t REG)
//{
//	if (REG == STATUS)
//	{
//		nrf24_WriteReg(STATUS, 0x00);
//	}
//
//	else if (REG == FIFO_STATUS)
//	{
//		nrf24_WriteReg(FIFO_STATUS, 0x11);
//	}
//
//	else {
//	nrf24_WriteReg(CONFIG, 0x08);
//	nrf24_WriteReg(EN_AA, 0x3F);
//	nrf24_WriteReg(EN_RXADDR, 0x03);
//	nrf24_WriteReg(SETUP_AW, 0x03);
//	nrf24_WriteReg(SETUP_RETR, 0x03);
//	nrf24_WriteReg(RF_CH, 0x02);
//	nrf24_WriteReg(RF_SETUP, 0x0E);
//	nrf24_WriteReg(STATUS, 0x00);
//	nrf24_WriteReg(OBSERVE_TX, 0x00);
//	nrf24_WriteReg(CD, 0x00);
//	uint8_t rx_addr_p0_def[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
//	nrf24_WriteRegMulti(RX_ADDR_P0, rx_addr_p0_def, 5);
//	uint8_t rx_addr_p1_def[5] = {0xC2, 0xC2, 0xC2, 0xC2, 0xC2};
//	nrf24_WriteRegMulti(RX_ADDR_P1, rx_addr_p1_def, 5);
//	nrf24_WriteReg(RX_ADDR_P2, 0xC3);
//	nrf24_WriteReg(RX_ADDR_P3, 0xC4);
//	nrf24_WriteReg(RX_ADDR_P4, 0xC5);
//	nrf24_WriteReg(RX_ADDR_P5, 0xC6);
//	uint8_t tx_addr_def[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
//	nrf24_WriteRegMulti(TX_ADDR, tx_addr_def, 5);
//	nrf24_WriteReg(RX_PW_P0, 0);
//	nrf24_WriteReg(RX_PW_P1, 0);
//	nrf24_WriteReg(RX_PW_P2, 0);
//	nrf24_WriteReg(RX_PW_P3, 0);
//	nrf24_WriteReg(RX_PW_P4, 0);
//	nrf24_WriteReg(RX_PW_P5, 0);
//	nrf24_WriteReg(FIFO_STATUS, 0x11);
//	nrf24_WriteReg(DYNPD, 0);
//	nrf24_WriteReg(FEATURE, 0);
//	}
//}
//


void NRF24_ReadKeyRegisters(void)
{
    uint8_t status = nrf24_ReadReg(STATUS);
    uint8_t fifo_status = nrf24_ReadReg(FIFO_STATUS);
    uint8_t config = nrf24_ReadReg(CONFIG);
    uint8_t observe_tx = nrf24_ReadReg(OBSERVE_TX);
    uint8_t rf_channel = nrf24_ReadReg(RF_CH);

    printf("STATUS Register: 0x%02X\n", status);
    printf("FIFO_STATUS Register: 0x%02X\n", fifo_status);
    printf("CONFIG Register: 0x%02X\n", config);
    printf("OBSERVE_TX Register: 0x%02X\n", observe_tx);
    printf("RF_CH Register: 0x%02X\n", rf_channel);

    // Optionally, print the TX_ADDR
    uint8_t tx_address[5];
    nrf24_ReadReg_Multi(TX_ADDR, tx_address, 5);
    printf("TX_ADDR: 0x%02X%02X%02X%02X%02X\n", tx_address[0], tx_address[1], tx_address[2], tx_address[3], tx_address[4]);
}


// Function to read and print NRF24L01+ receiver registers
void NRF24_PrintReceiverRegisters(void)
{
    uint8_t status = nrf24_ReadReg(STATUS);          // Read STATUS register
    uint8_t fifo_status = nrf24_ReadReg(FIFO_STATUS);  // Read FIFO_STATUS register
    uint8_t config = nrf24_ReadReg(CONFIG);            // Read CONFIG register
    uint8_t observe_tx = nrf24_ReadReg(OBSERVE_TX);    // Read OBSERVE_TX register (for TX-related info)
    uint8_t rf_ch = nrf24_ReadReg(RF_CH);              // Read RF channel
    uint8_t rx_addr_p0[5];                             // Buffer for RX_ADDR_P0
    nrf24_ReadReg_Multi(RX_ADDR_P0, rx_addr_p0, 5);     // Read RX_ADDR_P0 (Receiver Address)

    // Print the values of important registers
    printf("STATUS Register: 0x%02X\n", status);
    printf("FIFO_STATUS Register: 0x%02X\n", fifo_status);
    printf("CONFIG Register: 0x%02X\n", config);
    printf("OBSERVE_TX Register: 0x%02X\n", observe_tx);  // Not as important in RX, but check it
    printf("RF_CH Register: 0x%02X\n", rf_ch);

    // Print the RX_ADDR_P0 register (5 bytes)
    printf("RX_ADDR_P0: 0x%02X%02X%02X%02X%02X\n",
           rx_addr_p0[0], rx_addr_p0[1], rx_addr_p0[2], rx_addr_p0[3], rx_addr_p0[4]);
}


void NRF24_Init (void)
{
	// disable the chip before configuring the device
	CE_Disable();


	// reset everything
//	nrf24_reset (0);

	nrf24_WriteReg(CONFIG, 0);  // will be configured later

	nrf24_WriteReg(EN_AA, 0);  // No Auto ACK

	nrf24_WriteReg (EN_RXADDR, 0);  // Not Enabling any data pipe right now

	nrf24_WriteReg (SETUP_AW, 0x03);  // 5 Bytes for the TX/RX address

	nrf24_WriteReg (SETUP_RETR, 0);   // No retransmission

	nrf24_WriteReg (RF_CH, 0);  // will be setup during Tx or RX

	nrf24_WriteReg (RF_SETUP, 0x0E);   // Power= 0db, data rate = 2Mbps

	// Enable the chip after configuring the device
	CE_Enable();

}


// set up the Tx mode

void NRF24_TxMode (uint8_t *Address, uint8_t channel)
{
	// disable the chip before configuring the device
	CE_Disable();

	nrf24_WriteReg (RF_CH, channel);  // select the channel

	nrf24_WriteRegMulti(TX_ADDR, Address, 5);  // Write the TX address


	// power up the device
	uint8_t config = nrf24_ReadReg(CONFIG);
	config &= ~(1 << PRIM_RX);  // Clear PRIM_RX (TX mode)
	config |= (1 << PWR_UP);    // Ensure PWR_UP is set (power up the device)
	nrf24_WriteReg(CONFIG, config);

	// Enable the chip after configuring the device
	CE_Enable();
}




uint8_t NRF24_Transmit (uint8_t *data)
{
    uint8_t cmdtosend = W_TX_PAYLOAD;

    printf("Before transmission:\n");
    NRF24_ReadKeyRegisters();  // Monitor registers before transmission

    // Select the device
    CS_Select();

    // Payload command
    HAL_SPI_Transmit(NRF24_SPI, &cmdtosend, 1, 100);

    // Send the payload (32 bytes)
    HAL_SPI_Transmit(NRF24_SPI, data, 32, 1000);

    // Unselect the device
    CS_UnSelect();

    HAL_Delay(1);

    // Check the STATUS and FIFO_STATUS registers
    printf("After transmission:\n");
    NRF24_ReadKeyRegisters();  // Monitor registers after transmission





    // Check the STATUS register for the TX_DS flag
    uint8_t fifostatus = nrf24_ReadReg(FIFO_STATUS);
        	  if ((fifostatus & (1 << 5)) == 0)
            {
        		cmdtosend = FLUSH_TX;
        		nrfsendCmd(cmdtosend);
        		         		 	    // reset FIFO_STATUS
//   			    nrf24_reset (FIFO_STATUS);

        		  	       return 1;  // Transmission was successful
    }

    // Flush the TX FIFO if the transmission fails
    cmdtosend = FLUSH_TX;
    nrfsendCmd(cmdtosend);

    printf("Transmission Failed.\n");
    return 0;  // Transmission failed
}






void NRF24_RxMode (uint8_t *Address, uint8_t channel)
{
	// disable the chip before configuring the device
	CE_Disable();

//	nrf24_reset (STATUS);

	nrf24_WriteReg (RF_CH, channel);  // select the channel

	// select data pipe 1 (instead of Pipe 2, if using Pipe 1 for data)
	uint8_t en_rxaddr = nrf24_ReadReg(EN_RXADDR);
	en_rxaddr = en_rxaddr | (1 << 1);  // enable Pipe 1 instead of Pipe 2
	nrf24_WriteReg (EN_RXADDR, en_rxaddr);

	/* Write the address for Data Pipe 1 */
	nrf24_WriteRegMulti(RX_ADDR_P1, Address, 5);  // Write the Pipe 1 address

	// Set the payload size for Pipe 1 (32 bytes)
	nrf24_WriteReg (RX_PW_P1, 32);  // Set the payload size for pipe 1

	// power up the device in Rx mode
	uint8_t config = nrf24_ReadReg(CONFIG);
	config = config | (1 << 1) | (1 << 0);  // Power up and set PRIM_RX
	nrf24_WriteReg (CONFIG, config);

	// Enable the chip after configuring the device
	CE_Enable();
}





uint8_t isDataAvailable (int pipenum)
{
	uint8_t status = nrf24_ReadReg(STATUS);

    if ((status & (1 << 6)) && (((status >> 1) & 0x07) == pipenum))
	{

		nrf24_WriteReg(STATUS, (1<<6));

		return 1;
	}

	return 0;
}



void NRF24_Receive (uint8_t *data)
{
	uint8_t cmdtosend = 0;

	// select the device
	CS_Select();

	// payload command
	cmdtosend = R_RX_PAYLOAD;
	HAL_SPI_Transmit(NRF24_SPI, &cmdtosend, 1, 100);

	// Receive the payload
	HAL_SPI_Receive(NRF24_SPI, data, 32, 1000);

	// Unselect the device
	CS_UnSelect();

	HAL_Delay(1);

	cmdtosend = FLUSH_RX;
	nrfsendCmd(cmdtosend);
}




void NRF24_ReadAll (uint8_t *data)
{
	for (int i=0; i<10; i++)
	{
		*(data+i) = nrf24_ReadReg(i);
	}

	nrf24_ReadReg_Multi(RX_ADDR_P0, (data+10), 5);

	nrf24_ReadReg_Multi(RX_ADDR_P1, (data+15), 5);

	*(data+20) = nrf24_ReadReg(RX_ADDR_P2);
	*(data+21) = nrf24_ReadReg(RX_ADDR_P3);
	*(data+22) = nrf24_ReadReg(RX_ADDR_P4);
	*(data+23) = nrf24_ReadReg(RX_ADDR_P5);

	nrf24_ReadReg_Multi(RX_ADDR_P0, (data+24), 5);

	for (int i=29; i<38; i++)
	{
		*(data+i) = nrf24_ReadReg(i-12);
	}

}

