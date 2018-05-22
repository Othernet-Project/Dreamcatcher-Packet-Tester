#include "SPI.h"

SPI::SPI(PinName mosi, PinName miso, PinName sclk, PinName ssel)
{
	_bits = 8;
    if( (_fd = ::open("/dev/spidev2.0", O_RDWR)) < 0) {
		fprintf(stderr,"Unable to open SPI device\r\n");
	}
	usleep(100000);

	#ifdef _SPI_DEBUG
	printf("SPI::SPI\n");
	#endif
}

SPI::~SPI()
{
}

void SPI::format(int bits, int mode)
{
	_bits = bits;
	/* Bits size */
	if ( ioctl(_fd, SPI_IOC_WR_BITS_PER_WORD, &bits) < 0 )
		fprintf(stderr,"SPI::format: Unable to set bits");
	if ( ioctl(_fd, SPI_IOC_RD_BITS_PER_WORD, &bits) < 0 )
		fprintf(stderr,"SPI::format: Unable to get bits");

	_dataMode = mode;
	if ( ioctl(_fd, SPI_IOC_WR_MODE, &mode) < 0 )
		fprintf(stderr,"SPI::format: Unable to set SPI mode");
	if ( ioctl(_fd, SPI_IOC_RD_MODE, &mode) < 0 )
		fprintf(stderr,"SPI::format: Unable to get SPI mode");
}

void SPI::frequency(int hz)
{
	/* Frequency configuration */
	_speed=hz;
	if ( ioctl(_fd, SPI_IOC_WR_MAX_SPEED_HZ, &hz) < 0 )
		fprintf(stderr,"SPI::frequency: Unable to set SPI speed");
	if ( ioctl(_fd, SPI_IOC_RD_MAX_SPEED_HZ, &hz) < 0)
		fprintf(stderr,"SPI::frequency: Unable to get SPI speed");
}

int SPI::exchange(uint32_t length)
{
	int ret;
   struct spi_ioc_transfer tr = {0};

   tr.tx_buf = (unsigned long)_tx_buffer;	//tx and rx MUST be the same length!
   tr.rx_buf = (unsigned long)_rx_buffer;
   tr.len = length;
   tr.delay_usecs = 0;
   tr.speed_hz = _speed;
   tr.bits_per_word = _bits;
   tr.pad=0;

	if( (ret = ioctl(_fd, SPI_IOC_MESSAGE(1), &tr)) < 0)
		perror("RHLinuxSPI::exchange() : failed");
#ifdef RH_LINUX_SPI_DEBUG
	printf("RHLinuxSPI::exchange() : length=%d ret=%d\n", length,  ret);
#endif

	return ret;
}

int SPI::write(int value)
{
	_tx_buffer[0] = value;
	exchange(1);

#ifdef _SPI_DEBUG
	fprintf(stderr,"SPI::format: 0x%2.2X 0x%2.2X\n", value, _rx_buffer[0]);
#endif

    return _rx_buffer[0];
}

int SPI::writeLen(unsigned char * ptr, unsigned int len)
{
	memcpy(_tx_buffer,ptr,len);
	exchange(len);

#ifdef _SPI_DEBUG
	fprintf(stderr,"SPI::format: 0x%2.2X 0x%2.2X\n", _rx_buffer[0]);
#endif

    return _rx_buffer[0];
}

int SPI::writeCommand(unsigned char cmd,unsigned char * ptr, unsigned int len)
{
	memcpy(_tx_buffer,&cmd,1);
	memcpy(_tx_buffer+1,ptr,len);
	exchange(len+1);

#ifdef _SPI_DEBUG
	fprintf(stderr,"SPI::format: 0x%2.2X 0x%2.2X\n", _rx_buffer[0]);
#endif

    return _rx_buffer[0];
}

void SPI::readCommand(unsigned char * cmd, unsigned int len_cmd,uint8_t * buffer, unsigned int len_buffer)
{
	memcpy(_tx_buffer,cmd,len_cmd);
	memset(&_tx_buffer[len_cmd], 0, len_buffer);
	exchange(len_cmd+len_buffer);
	memcpy(buffer,&_rx_buffer[len_cmd],len_buffer);

#ifdef _SPI_DEBUG
	//fprintf(stderr,"SPI::format: 0x%2.2X 0x%2.2X\n", _rx_buffer[0]);
#endif
	
}

void SPI::writeBuffer(unsigned char * cmd, unsigned int len_cmd,uint8_t * buffer, unsigned int len_buffer)
{
	memcpy(_tx_buffer,cmd,len_cmd);
	memcpy(&_tx_buffer[len_cmd],buffer,len_buffer);
	exchange(len_cmd+len_buffer);
	//memcpy(buffer,&_rx_buffer[len_cmd],len_buffer);

#ifdef _SPI_DEBUG
	//fprintf(stderr,"SPI::format: 0x%2.2X 0x%2.2X\n", _rx_buffer[0]);
#endif
	
}

void SPI::lock(void)
{

}

void SPI::unlock(void){
}
