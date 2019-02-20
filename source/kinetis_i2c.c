/* kinetis_i2c.c */
#include "kinetis_i2c.h"
#include "fsl_i2c.h"

static i2c_master_transfer_t transfer;

int32_t execute_I2C_transfer(void)
{
	int32_t status;
	// wordAddressSize used as counter in I2C_MasterTransferBlocking
	// so needs reset on each call
	set_wordAddressSize(1);	// I think this handles word addresses >8-bit

	do{
		status = I2C_MasterTransferBlocking(I2C0, &transfer);
	}while(status == kStatus_I2C_Addr_Nak);

	return status;
}

void initialize_I2C_transfer(void)
{
	memset(&transfer, 0, sizeof(transfer));
	set_TransferFlags(0); 	// kI2C_TransferDefaultFlag = 0x0U
}

void set_TransferFlags(uint32_t flags)
{
	transfer.flags = flags;
}

void set_slaveAddress(uint8_t address)
{
	transfer.slaveAddress = address;
}

void set_directionAsWrite(void)
{
	transfer.direction = kI2C_Write;
}
void set_directionAsRead(void)
{
	transfer.direction = kI2C_Read;
}

void set_wordAddress(uint32_t wordAddress)
{
	transfer.subaddress = wordAddress;
}

void set_wordAddressSize(uint8_t size)
{
	transfer.subaddressSize = size;
}

void set_bufferPointer(uint8_t *volatile bufferPtr)
{
	transfer.data = bufferPtr;
}

void set_bufferSize(uint32_t size)
{
	transfer.dataSize = size;
}
