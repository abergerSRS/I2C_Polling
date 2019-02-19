/* kinetis_i2c.h */

#ifndef KINETIS_I2C_H
#define KINETIS_I2C_H

#include <stdint.h>

void execute_I2C_transfer(void);

void initialize_I2C_transfer(void);
void set_TransferFlags(uint32_t flags);
void set_slaveAddress(uint8_t address);
void set_directionAsWrite(void);
void set_directionAsRead(void);
void set_wordAddress(uint32_t wordAddress);
void set_wordAddressSize(uint8_t size);
void set_bufferPointer(uint8_t *volatile bufferPtr);
void set_bufferSize(uint32_t size);

#endif /* KINETIS_I2C_H */
