/*
 * nRF24L01.c
 *
 *  Created on: Oct 2, 2024
 *      Author: ANAS ALSAYAR
 */


#include "NRF24L01.h"
#include "stm32f1xx_hal.h"

extern SPI_HandleTypeDef hspi1;
#define NRF24_SPI &hspi1

#define NRF24_CE_PORT   GPIOB
#define NRF24_CE_PIN	GPIO_PIN_1


#define NRF24_CS_PORT   GPIOA
#define NRF24_CS_PIN	GPIO_PIN_4



void CS_Select ()
{
	HAL_GPIO_WritePin(NRF24_CS_PORT, NRF24_CS_PIN, GPIO_PIN_RESET);
}

void CS_Unselect ()
{
	HAL_GPIO_WritePin(NRF24_CS_PORT, NRF24_CS_PIN, GPIO_PIN_SET);
}

void CE_Enable ()
{
	HAL_GPIO_WritePin(NRF24_CE_PORT, NRF24_CE_PIN, GPIO_PIN_SET);
}

void CE_Disable ()
{
	HAL_GPIO_WritePin(NRF24_CE_PORT, NRF24_CE_PIN, GPIO_PIN_RESET);
}


/*To write to a register, you send a command that includes the register address with a specific write bit (bit 5) set.
To read from a register, you send a command that includes the register address with the write bit cleared (bit 5 not set).*/


// write a single byte to the particular register
void nrf24_WriteReg (uint8_t Reg , uint8_t Data)
{

	uint8_t buf[2];
	buf[0] = Reg | 1<<5;
	buf[1] = Data;

	//Pull the CS pin Low
	CS_Select();

	HAL_SPI_Transmit(NRF24_SPI , buf, 2 , 1000);

	//Pull the CS pin High
	CS_Unselect();
}


//write multiple bytes starting from a particular register
void nrf24_WriteRegMulti (uint8_t Reg, uint8_t *Data, int size)
{

	uint8_t buf[2];
	buf[0] = Reg | 1<<5;
//	buf[1] = Data;

	//Pull the CS pin Low
	CS_Select();

	HAL_SPI_Transmit(NRF24_SPI , buf, 1 , 1000);
	HAL_SPI_Transmit(NRF24_SPI , Data, size , 1000);

	//Pull the CS pin High
	CS_Unselect();

}



//Read only one byte
uint8_t nrf24_ReadReg (uint8_t Reg)
{

	uint8_t data = 0;
	CS_Select();

	HAL_SPI_Transmit(NRF24_SPI, &Reg, 1, 100);
	HAL_SPI_Receive(NRF24_SPI, &data, 1, 100);
	//Pull the CS pin High
	CS_Unselect();

	return data;
}

// Multiple Readings
void nrf24_ReadReg_Multi (uint8_t Reg, uint8_t *data, int size)
{

	CS_Select();

	HAL_SPI_Transmit(NRF24_SPI, &Reg, 1, 100);
	HAL_SPI_Receive(NRF24_SPI, data, size, 1000);
	//Pull the CS pin High
	CS_Unselect();


}


void nrfsendCmd (uint8_t cmd)
{

		CS_Select();
		HAL_SPI_Transmit(NRF24_SPI, &cmd, 1, 100);
		CS_Unselect();
}


void NRF24_Init ()
{
	//Disable the chip before configuring the device
	CE_Disable();
	
		// reset everything

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

void NRF24_TxMode(uint8_t *Address , uint8_t channel)
{
	//Disable the chip before configuring
		CE_Disable();

		nrf24_WriteReg (RF_CH, channel);  //  select the channel // this can take up to 6 bits of data so unsigned 8 will be enough for this
		nrf24_WriteRegMulti(TX_ADDR, Address, 5);  // Write the TX address


			// power up the device
				uint8_t config = nrf24_ReadReg(CONFIG);
				config = config | (1<<1);   // write 1 in the PWR_UP bit
			//	config = config & (0xF2);    // write 0 in the PRIM_RX, and 1 in the PWR_UP, and all other bits are masked
				nrf24_WriteReg (CONFIG, config);

				CE_Enable();

}

// transmit the data

	uint8_t NRF24_Transmit(uint8_t *data)
	{

		uint8_t cmdtosend = 0;

		// select the device
			CS_Select();

		// payload command
			cmdtosend = W_TX_PAYLOAD;
			HAL_SPI_Transmit(NRF24_SPI, &cmdtosend, 1, 100);


		// send the payload
			HAL_SPI_Transmit(NRF24_SPI, data, 32, 1000);

		// Unselect the device
			CS_Unselect();

			HAL_Delay(1);

			uint8_t fifostatus = nrf24_ReadReg(FIFO_STATUS);

			// check the fourth bit of FIFO_STATUS to know if the TX fifo is empty
				if ((fifostatus&(1<<4)) && (!(fifostatus&(1<<3))))
				{
					cmdtosend = FLUSH_TX;
					nrfsendCmd(cmdtosend);

					return 1;
				}

				return 0;

	}


	void NRF24_RxMode (uint8_t *Address, uint8_t channel)
	{
		// disable the chip before configuring the device
		CE_Disable();

		nrf24_WriteReg (RF_CH, channel);  // select the channel


		// Select Data Pipe 1
		uint8_t en_rxaddr = nrf24_ReadReg(EN_RXADDR);
		en_rxaddr = en_rxaddr | (1<<1);
		nrf24_WriteReg (EN_RXADDR, en_rxaddr);
//		nrf24_WriteReg (EN_RXADDR, 0X02);    // In binary, 0x02 is 00000010, which means disables all pipes except for Pipe 1.


		nrf24_WriteRegMulti(RX_ADDR_P1, Address, 5);  // Write the Pipe1 address

		nrf24_WriteReg (RX_PW_P1, 32);   // 32 byte payload size for pipe 1


		// Power up the device
		uint8_t config = nrf24_ReadReg(CONFIG);
		config = config | (1<<1) | (1<<0);
		nrf24_WriteReg (CONFIG, config);

		// Enable the chip after configuring the device
		CE_Enable();
	}



	uint8_t isDataAvailable (int pipenum)
	{
		uint8_t status = nrf24_ReadReg(STATUS);

		if ((status&(1<<6))&&(status&(pipenum<<1)))
		{

			nrf24_WriteReg(STATUS, (1<<6));

			return 1;
		}

		return 0;
	}


	/*// EXTI interrupt handler (called when the IRQ pin goes low)
void EXTI_IRQHandler(void) {
    // Check if the interrupt is triggered by the correct EXTI line
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_X) != RESET) {
        // Clear the interrupt flag
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_X);

        // Read the STATUS register from nRF24L01
        uint8_t status = nrf24_ReadReg(STATUS);

        // Check if data is available (RX_DR flag is set)
        if (status & (1 << 6)) {
            // Handle data received (read from RX FIFO)
            uint8_t data[32];
            NRF24_Receive(data);

            // Clear RX_DR flag in STATUS register
            nrf24_WriteReg(STATUS, (1 << 6));
        }

        // Check for other interrupts (TX_DS, MAX_RT) if necessary
        // ...
    }
}
	 * */













