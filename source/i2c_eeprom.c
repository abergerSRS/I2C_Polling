#include "i2c_eeprom.h"
#include <stdint.h>
#include "kinetis_i2c.h"

#ifndef TEST
#define STATIC static
#else
#define STATIC
#endif

STATIC uint8_t TxBuffer[PWB_SIZE];
STATIC uint16_t TxFillIndex = 0;
STATIC uint16_t dataIndex = 0;

/* Static helper functions */
static void enqueueAsBytes(uint32_t data, uint16_t dataSize_bytes)
{
	for(uint16_t i=0; i<dataSize_bytes; i++) {
		putByteIntoTxBuffer(TxFillIndex + i, (data >> i*8) & 0xff);
	}
}

static void fillTxBuffer(void *srcData, uint16_t arraySize, uint16_t elemSize_bytes)
{
	uint32_t data;

	for(TxFillIndex=0; TxFillIndex<PWB_SIZE; TxFillIndex += elemSize_bytes) {
		if(dataIndex < arraySize) {
			data = ((uint16_t *)srcData)[dataIndex];
			enqueueAsBytes(data,elemSize_bytes);
			dataIndex++;
		} else {
			data = 0;
			enqueueAsBytes(data,elemSize_bytes);
		}
	}
}

int writeDataToEEPROM(void *srcData, uint16_t arraySize, uint16_t elemSize_bytes, uint8_t destAddress)
{
	initialize_I2C_transfer();
	set_slaveAddress(destAddress);
	set_directionAsWrite();
	set_bufferPointer(TxBuffer);

	dataIndex = 0;
	uint8_t wordAddress = 0x00;
	do{
		fillTxBuffer(srcData, arraySize, elemSize_bytes);
		set_wordAddress(wordAddress);
		set_bufferSize(PWB_SIZE);
		/* send out current TxBuffer as a page of data */
		execute_I2C_transfer();
		wordAddress += PWB_SIZE;
	} while(dataIndex < arraySize);

	return 0;
}

uint8_t getByteFromTxBuffer(uint16_t index)
{
	return TxBuffer[index];
}

void putByteIntoTxBuffer(uint16_t index, uint8_t byte)
{
	TxBuffer[index] = byte;
}
