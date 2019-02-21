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

STATIC uint8_t RxBuffer_16[2*CAL_TABLE_SIZE];
STATIC uint8_t RxBuffer_32[4*CAL_TABLE_SIZE];


uint8_t getByteFromTxBuffer(uint16_t index)
{
	return TxBuffer[index];
}

void putByteIntoTxBuffer(uint16_t index, uint8_t byte)
{
	TxBuffer[index] = byte;
}

int32_t convert_Nbytes_to_int(uint8_t srcBytes[], uint16_t elemSize_bytes, uint16_t startLoc)
{
	int32_t value = 0;
	for(int i=0; i<elemSize_bytes; i++) {
		value |= (int32_t)srcBytes[startLoc+i] << (i*8);
	}

	return value;
}


/* Static helper functions */
static void enqueueAsBytes(uint32_t data, uint16_t dataSize_bytes)
{
	for(uint16_t i=0; i<dataSize_bytes; i++) {
		putByteIntoTxBuffer(TxFillIndex + i, (data >> i*8) & 0xff);
	}
}

static void convertBufferedBytesTo16Bit(int16_t *destArray, uint16_t destSize)
{
	for(int i=0; i<destSize; i++) {
		destArray[i] = convert_Nbytes_to_int(RxBuffer_16, 2, 2*i);
	}
}

static void convertBufferedBytesTo32Bit(int32_t *destArray, uint16_t destSize)
{
	for(int i=0; i<destSize; i++) {
		destArray[i] = convert_Nbytes_to_int(RxBuffer_32, 4, 4*i);
	}
}

static void fillTxBuffer(const void *srcData, uint16_t arraySize, uint16_t elemSize_bytes)
{
	uint32_t data;

	for(TxFillIndex=0; TxFillIndex<PWB_SIZE; TxFillIndex += elemSize_bytes) {
		if(dataIndex < arraySize) {
			if(elemSize_bytes == 2) {
				data = ((uint16_t *)srcData)[dataIndex];
			} else if(elemSize_bytes == 4) {
				data = ((uint32_t *)srcData)[dataIndex];
			}
			enqueueAsBytes(data,elemSize_bytes);
			dataIndex++;
		} else {
			data = 0;
			enqueueAsBytes(data,elemSize_bytes);
		}
	}
}

int writeDataToEEPROM(const void *srcData, uint16_t arraySize, uint16_t elemSize_bytes, uint8_t destAddress)
{
	initialize_I2C_transfer();
	set_directionAsWrite();
	set_bufferPointer(TxBuffer);

	dataIndex = 0;
	uint32_t wordAddress = 0x00;
	int32_t status = 0;
	do{
		fillTxBuffer(srcData, arraySize, elemSize_bytes);
		set_slaveAddress(destAddress);
		set_wordAddress(wordAddress);
		set_bufferSize(PWB_SIZE);

		status = execute_I2C_transfer();

		wordAddress += PWB_SIZE;
		if(wordAddress > 0xff) {
			wordAddress = 0;
			destAddress++;
		}
	} while(dataIndex < arraySize);

	return status;
}

int read16bitDataFromEEPROM(void *receivedData, uint8_t srcAddress, uint32_t wordAddress)
{
	int32_t status = 0;

	initialize_I2C_transfer();
	set_slaveAddress(srcAddress);
	set_directionAsRead();
	set_bufferPointer(RxBuffer_16);
	set_wordAddress(wordAddress);
	set_bufferSize(2*CAL_TABLE_SIZE);	// need 2 bytes per 16-bit element

	status = execute_I2C_transfer();

	convertBufferedBytesTo16Bit(receivedData, CAL_TABLE_SIZE);

	return status;
}

int read32bitDataFromEEPROM(void *receivedData, uint8_t srcAddress, uint32_t wordAddress)
{
	int32_t status = 0;

	initialize_I2C_transfer();
	set_slaveAddress(srcAddress);
	set_directionAsRead();
	set_bufferPointer(RxBuffer_32);
	set_wordAddress(wordAddress);
	set_bufferSize(4*CAL_TABLE_SIZE);	// need 4 bytes per 32-bit element

	status = execute_I2C_transfer();

	convertBufferedBytesTo32Bit(receivedData, CAL_TABLE_SIZE);

	return status;
}
