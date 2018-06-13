#ifndef MBED_SPI_H
#define MBED_SPI_H

#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gpio.h"

//#define _SPI_DEBUG
//#define  RH_LINUX_SPI_DEBUG

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif

#include <linux/types.h>
#include <linux/spi/spidev.h>

#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
}
#endif

class SPI {

public:

    /** Create a SPI master connected to the specified pins
     *
     *  mosi or miso can be specfied as NC if not used
     *
     *  @param mosi SPI Master Out, Slave In pin
     *  @param miso SPI Master In, Slave Out pin
     *  @param sclk SPI Clock pin
     *  @param ssel SPI chip select pin
     */
    SPI(PinName mosi, PinName miso, PinName sclk, PinName ssel);
	~SPI();
    /** Configure the data transmission format
     *
     *  @param bits Number of bits per SPI frame (4 - 16)
     *  @param mode Clock polarity and phase mode (0 - 3)
     *
     * @code
     * mode | POL PHA
     * -----+--------
     *   0  |  0   0
     *   1  |  0   1
     *   2  |  1   0
     *   3  |  1   1
     * @endcode
     */
    void format(int bits, int mode = 0);

    /** Set the spi bus clock frequency
     *
     *  @param hz SCLK frequency in hz (default = 1MHz)
     */
    void frequency(int hz = 1000000);

    /** Write to the SPI Slave and return the response
     *
     *  @param value Data to be sent to the SPI slave
     *
     *  @returns
     *    Response from the SPI slave
    */
    virtual int write(int value);
	virtual int writeLen(unsigned char * ptr, unsigned int len);
	virtual int writeCommand(unsigned char cmd,unsigned char * ptr, unsigned int len);
	virtual void readCommand(unsigned char * cmd, unsigned int len_cmd,uint8_t * buffer, unsigned int len_buffer);
	virtual void writeBuffer(unsigned char * cmd, unsigned int len_cmd,uint8_t * buffer, unsigned int len_buffer);

    /** Acquire exclusive access to this SPI bus
     */
    virtual void lock(void);

    /** Release exclusive access to this SPI bus
     */
    virtual void unlock(void);

public:
    

protected:
    uint8_t _rx_buffer[255+1];
	uint8_t _tx_buffer[255+1];
	int _fd;
	int exchange(uint32_t length);
   	uint32_t _speed;
	uint8_t _dataMode;
	uint8_t _bits;
};


#endif
