#include "unity.h"
#include "i2c_eeprom.h"
#include "mock_kinetis_i2c.h"
#include <stdint.h>

#define I2C_MEM_ADDR 0x50U

extern uint8_t TxBuffer[PWB_SIZE];

int16_t test_data[33] = {
    721,
	814,
	762,
	698,
	650,
	624,
	576,
	474,
	334,
	197,
	92,
	20,
	-36,
	-103,
	-224,
	-254,
	-344,
	-447,
	-525,
	-567,
	-584,
	-601,
	-632,
	-673,
	-719,
	-769,
	-827,
	-878,
	-887,
	-836,
	-734,
	-586,
    -418
};

void setUp(void)
{
}

void tearDown(void)
{
}

void test_write_16bit_testData_toEEPROM(void)
{
    int errorCode = 1;
    int elemSize = sizeof(test_data[0]);
    int arraySize = sizeof(test_data)/elemSize;

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

    errorCode = writeDataToEEPROM(&test_data,arraySize,elemSize,I2C_MEM_ADDR);

    TEST_ASSERT_EQUAL(0,errorCode);

    /* compare the data enqueued on the final TxBuffer */
    int firstElemOfTestData = (numberOfPages-1)*PWB_SIZE/elemSize;
    for(int i=0; i<PWB_SIZE/elemSize; i++) {
        TEST_ASSERT_EQUAL_HEX16(test_data[firstElemOfTestData + i],
            (getByteFromTxBuffer(elemSize*i+1) << 8) | getByteFromTxBuffer(elemSize*i));
    }
}

void test_put_and_get_fromTxBuffer(void)
{
    uint8_t test_byte = 0x1a;
    uint16_t index = 7;

    putByteIntoTxBuffer(index, test_byte);

    TEST_ASSERT_EQUAL_HEX8(test_byte,getByteFromTxBuffer(index));
}
