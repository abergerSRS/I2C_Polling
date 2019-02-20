#include "unity.h"
#include "i2c_eeprom.h"
#include "mock_kinetis_i2c.h"
#include <stdint.h>
#include "currentQcomp.h"

#define I2C_MEM_ADDR 0x50U

extern uint8_t TxBuffer[PWB_SIZE];
extern uint8_t RxBuffer_16[2*CAL_TABLE_SIZE];

void setUp(void)
{
}

void tearDown(void)
{
}

void test_put_and_get_fromTxBuffer(void)
{
    uint8_t test_byte = 0x1a;
    uint16_t index = 7;

    putByteIntoTxBuffer(index, test_byte);

    TEST_ASSERT_EQUAL_HEX8(test_byte,getByteFromTxBuffer(index));
}

void test_convert_2bytes_to_int16(void)
{
    uint8_t LSB_8 = 0x1a;
    uint8_t MSB_8 = 0x37;
    int16_t exp_val = 0x371a; // MSB_8 << 8 | LSB_8

    int16_t ret_val = 0;
    ret_val = convert_2bytes_to_int16(LSB_8, MSB_8);

    TEST_ASSERT_EQUAL(exp_val, ret_val);
}

void test_write_16bit_testData_toEEPROM(void)
{
    int errorCode = 1;
    int elemSize = sizeof(currentQcomp[0]);
    int arraySize = sizeof(currentQcomp)/elemSize;

    // calculate ceil(totalBytes/PWB_SIZE)
    int numberOfPages = 1 + ((elemSize*arraySize)-1)/PWB_SIZE;

    initialize_I2C_transfer_Expect();
    set_slaveAddress_Expect(I2C_MEM_ADDR);
    set_directionAsWrite_Expect();
    set_bufferPointer_Expect(TxBuffer);

    for(int j=0; j<numberOfPages; j++) {
        set_wordAddress_Expect(j*PWB_SIZE);
        set_bufferSize_Expect(PWB_SIZE);
        execute_I2C_transfer_Expect();
    }

    errorCode = writeDataToEEPROM(&currentQcomp,arraySize,elemSize,I2C_MEM_ADDR);

    TEST_ASSERT_EQUAL(0,errorCode);

    /* compare the data enqueued on the final TxBuffer */
    int firstElemOfTestData = (numberOfPages-1)*PWB_SIZE/elemSize;
    for(int i=0; i<PWB_SIZE/elemSize; i++) {
        TEST_ASSERT_EQUAL_HEX16(currentQcomp[firstElemOfTestData + i],
            (getByteFromTxBuffer(elemSize*i+1) << 8) | getByteFromTxBuffer(elemSize*i));
    }
}

void test_read_16bit_testData_fromEEPROM(void)
{
	int errorCode = 1;

	int elemSize = sizeof(currentQcomp[0]);
	int arraySize = sizeof(currentQcomp)/elemSize;

	int16_t rcvd_data[CAL_TABLE_SIZE];
	uint32_t firstWord = 0x00;

	initialize_I2C_transfer_Expect();
	set_slaveAddress_Expect(I2C_MEM_ADDR);
	set_directionAsRead_Expect();
	set_bufferPointer_Expect(RxBuffer_16);
	set_wordAddress_Expect(firstWord);
	set_bufferSize_Expect(2*CAL_TABLE_SIZE); // 2 bytes per 16-bit element
	execute_I2C_transfer_Expect();

	// simulate EEPROM reading, and saving data to RxBuffer_16
	for(int i=0; i<CAL_TABLE_SIZE; i++) {
		RxBuffer_16[2*i] = currentQcomp[i] & 0xff;
		RxBuffer_16[2*i+1] = (currentQcomp[i] >> 8) & 0xff;
	}

	errorCode = read16bitDataFromEEPROM(&rcvd_data,I2C_MEM_ADDR,firstWord);

	TEST_ASSERT_EQUAL(0,errorCode);

	for(int i=0; i<CAL_TABLE_SIZE; i++) {
		TEST_ASSERT_EQUAL(currentQcomp[i],rcvd_data[i]);
	}

}


