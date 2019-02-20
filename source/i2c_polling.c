/*
 * The Clear BSD License
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 * that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*  Standard C Included Files */
#include <string.h>
/*  SDK Included Files */
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_i2c.h"

#include "pin_mux.h"

/* Custom Included Files */
#include "i2c_eeprom.h"
#include "currentQcomp.h"
#include "angleComp.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* I2C source clock */
#define I2C_MASTER_CLK_SRC I2C0_CLK_SRC
#define I2C_MASTER_CLK_FREQ CLOCK_GetFreq(I2C0_CLK_SRC)
#define EXAMPLE_I2C_MASTER_BASEADDR I2C0
#define I2C_BAUDRATE 100000U

#define I2C_MEM_ADDR	0x50U



/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void delay(unsigned int cycles);

/*******************************************************************************
 * Variables
 ******************************************************************************/

int16_t rcvd_16bitData[CAL_TABLE_SIZE];
int32_t rcvd_32bitData[CAL_TABLE_SIZE];

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Main function
 */
int main(void)
{
    i2c_master_config_t masterConfig;
    uint32_t sourceClock;
    status_t status = kStatus_Success;

    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    /*
	* masterConfig->baudRate_Bps = 100000U;
	* masterConfig->enableStopHold = false;
	* masterConfig->glitchFilterWidth = 0U;
	* masterConfig->enableMaster = true;
	*/
    I2C_MasterGetDefaultConfig(&masterConfig);
    masterConfig.baudRate_Bps = I2C_BAUDRATE;
    sourceClock = I2C_MASTER_CLK_FREQ;
    I2C_MasterInit(EXAMPLE_I2C_MASTER_BASEADDR, &masterConfig, sourceClock);


    PRINTF("\r\nI2C board2board polling example -- Master transfer.\r\n");

    /* Test read/write 16-bit data ------------------------------------------ */

    uint8_t firstWordAddress = 0x00;
	read16bitDataFromEEPROM(rcvd_16bitData, I2C_MEM_ADDR, firstWordAddress);
	
	uint8_t elemSize = sizeof(currentQcomp[0]);
    uint8_t numElem = sizeof(currentQcomp)/elemSize;

    uint16_t test_16bitdata[numElem];
    uint32_t i;
    for(i = 0; i<numElem; i++){
    	test_16bitdata[i] = i;
    }

    status = writeDataToEEPROM(&currentQcomp, numElem, elemSize, I2C_MEM_ADDR);
    //status = writeDataToEEPROM(&test_16bitdata, numElem, elemSize, I2C_MEM_ADDR);

    PRINTF("Receive sent data from slave :");
    read16bitDataFromEEPROM(rcvd_16bitData, I2C_MEM_ADDR, firstWordAddress);

    for (i=0; i<CAL_TABLE_SIZE; i++)
    {
        if (i % 10 == 0)
        {
            PRINTF("\r\n");
        }

        PRINTF("%d\r\n", rcvd_16bitData[i]);
    }
    PRINTF("\r\n\r\n");

    /* Test read/write 32-bit data ------------------------------------------ */
    elemSize = sizeof(angleComp[0]);
	numElem = sizeof(angleComp)/elemSize;
	uint8_t pageNum = 0x01;

	uint32_t test_32bitdata[numElem];
	for(i = 0; i<numElem; i++){
		test_32bitdata[i] = i;
	}

	status = writeDataToEEPROM(&angleComp, numElem, elemSize, (I2C_MEM_ADDR | pageNum));
	//status = writeDataToEEPROM(&test_32bitdata, numElem, elemSize, (I2C_MEM_ADDR | pageNum));


	PRINTF("Receive sent data from slave :");
	read32bitDataFromEEPROM(rcvd_32bitData, (I2C_MEM_ADDR | pageNum), firstWordAddress);

	for (i=0; i<CAL_TABLE_SIZE; i++)
	{
		if (i % 10 == 0)
		{
			PRINTF("\r\n");
		}

		PRINTF("%d\r\n", rcvd_32bitData[i]);
	}
	PRINTF("\r\n\r\n");

	/* Transfer completed. Check the data.*/
	/*
	for (uint32_t i = 0U; i < I2C_DATA_LENGTH; i++)
	{
		if (g_master_rxBuff[i] != g_master_txBuff[i])
		{
			PRINTF("\r\nError occured in the transfer ! \r\n");
			break;
		}
	}
	*/

    PRINTF("\r\nEnd of I2C example .\r\n");
    uint8_t k = 0;
    while (1)
    {
    	I2C_MasterStart(EXAMPLE_I2C_MASTER_BASEADDR, k, kI2C_Write);
    	delay(120000);
    	I2C_MasterStop(EXAMPLE_I2C_MASTER_BASEADDR);
    	k++;
    }
}

void delay(unsigned int cycles){
	unsigned int idx;
	for(idx = 0; idx < cycles; idx++){
		__asm("NOP"); /* delay */
	}
}
