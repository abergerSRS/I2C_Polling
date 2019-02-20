#ifndef _I2C_EEPROM_H
#define _I2C_EEPROM_H
#include <stdint.h>

#define PWB_SIZE 		16	// page size of EEPROM
#define CAL_TABLE_SIZE	100	// cable table size (in array elements)

int writeDataToEEPROM(const void *srcData, uint16_t arraySize, uint16_t elemSize_bytes, uint8_t destAddress);
int read16bitDataFromEEPROM(void *receivedData, uint8_t srcAddress, uint32_t wordAddress);
int read32bitDataFromEEPROM(void *receivedData, uint8_t srcAddress, uint32_t wordAddress);

/* Helper functions */
// TODO: should these be public?
uint8_t getByteFromTxBuffer(uint16_t index);
void putByteIntoTxBuffer(uint16_t index, uint8_t byte);
int16_t convert_2bytes_to_int16(uint8_t LSByte, uint8_t MSByte);
int32_t convert_4bytes_to_int32(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4);


#endif // _I2C_EEPROM_H
