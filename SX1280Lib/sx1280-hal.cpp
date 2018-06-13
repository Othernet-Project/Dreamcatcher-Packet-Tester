/*
  ______                              _
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2016 Semtech

Description: Handling of the node configuration protocol

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis, Gregory Cristian and Matthieu Verdy
*/
#include "sx1280-hal.h"

/*!
 * \brief Helper macro to create Interrupt objects only if the pin name is
 *        different from NC
 */
#define CreateDioPin( pinName, dio )                 \
            if( pinName == NC )                      \
            {                                        \
                dio = NULL;                          \
            }                                        \
            else                                     \
            {                                        \
                dio = new InterruptIn( pinName );    \
            }

int intDIO1(void *){
	printf("DIO1 Interrupt");
	//SX1280::OnDioIrq();
}
void intDIO2(void){
	printf("DIO2 Interrupt");
}
void intDIO3(void){
	printf("DIO3 Interrupt");
}

#define DioAssignCallback2( dio, pinMode, callback )                    \
if( dio != NULL )                                          \
{                                                          \
    dio->rise( callback );  \
}

#define DioAssignCallback( dio, pinMode, callback )                    \
            if( dio != NULL )                                          \
            {                                                          \
                dio->rise( this, static_cast <Trigger>( callback ) );  \
            }
/*!
* dio->rise( this, callback );
 * \brief Used to block execution waiting for low state on radio busy pin.
 *        Essentially used in SPI communications
 */
#define WaitOnBusy( )          while( BUSY == 1 ){ }


// This code handles cases where assert_param is undefined
#ifndef assert_param
#define assert_param( ... )
#endif

SX1280Hal::SX1280Hal( PinName mosi, PinName miso, PinName sclk, PinName nss,
                      PinName busy, PinName dio1, PinName dio2, PinName dio3, PinName rst,
                      RadioCallbacks_t *callbacks )
        :   SX1280( callbacks ),
            RadioNss( nss ),
            RadioReset( rst ),
            RadioCtsn( NC ),
            BUSY( busy )
{
    CreateDioPin( dio1, DIO1 );
    CreateDioPin( dio2, DIO2 );
    CreateDioPin( dio3, DIO3 );
    RadioSpi = new SPI( mosi, miso, sclk, 0);

    RadioNss = 1;
    RadioReset = 1;
}

SX1280Hal::SX1280Hal( PinName tx, PinName rx, PinName ctsn,
                      PinName busy, PinName dio1, PinName dio2, PinName dio3, PinName rst,
                      RadioCallbacks_t *callbacks )
        :   SX1280( callbacks ),
            RadioNss( NC ),
            RadioReset( rst ),
            RadioCtsn( ctsn ),
            BUSY( busy )
{
    CreateDioPin( dio1, DIO1 );
    CreateDioPin( dio2, DIO2 );
    CreateDioPin( dio3, DIO3 );
    RadioSpi = NULL;
    RadioCtsn = 0;
    RadioReset = 1;
}

SX1280Hal::~SX1280Hal( void )
{
    if( this->RadioSpi != NULL )
    {
        delete RadioSpi;
    }
    if( DIO1 != NULL )
    {
        delete DIO1;
    }
    if( DIO2 != NULL )
    {
        delete DIO2;
    }
    if( DIO3 != NULL )
    {
        delete DIO3;
    }
};

void SX1280Hal::SpiInit( void )
{
    RadioNss = 1;
    RadioSpi->format( 8, 0 );
#if defined( TARGET_KL25Z )
    RadioSpi->frequency( 4000000 );
#elif defined( TARGET_NUCLEO_L476RG )
    RadioSpi->frequency( 8000000 );
#else
    RadioSpi->frequency( 8000000 );
#endif

    wait_ms( 100 );
}


void SX1280Hal::IoIrqInit( DioIrqHandler irqHandler )
{
    assert_param( RadioSpi != NULL);
    if( RadioSpi != NULL )
    {
        SpiInit( );
    }

    BUSY.mode( PullNone );
	#ifdef DEBUG
		printf("Creating interrupt bindings for pin %u\r\n",DIO1->_pin);
	#endif
    DioAssignCallback( DIO1, PullNone, irqHandler );

	#ifdef DEBUG
		printf("Creating interrupt bindings for pin %u\r\n",DIO2->_pin);
	#endif
    DioAssignCallback( DIO2, PullNone, irqHandler );

	#ifdef DEBUG
		printf("Creating interrupt bindings for pin %u\r\n",DIO3->_pin);
	#endif
    DioAssignCallback( DIO3, PullNone, irqHandler );
}

void SX1280Hal::Reset( void )
{
    __disable_irq( );
    wait_ms( 20 );
    RadioReset.output( );
    RadioReset = 0;
    wait_ms( 50 );
    RadioReset = 1;
    RadioReset.input( ); // Using the internal pull-up
    wait_ms( 20 );
    __enable_irq( );
}

void SX1280Hal::Wakeup( void )
{
    __disable_irq( );

    //Don't wait for BUSY here

    if( RadioSpi != NULL )
    {
        RadioNss = 0;
        RadioSpi->write( RADIO_GET_STATUS );
        RadioSpi->write( 0 );
        RadioNss = 1;
    }

    // Wait for chip to be ready.
    WaitOnBusy( );

    __enable_irq( );
}

void SX1280Hal::WriteCommand( RadioCommands_t command, uint8_t *buffer, uint16_t size )
{
    WaitOnBusy( );

    if( RadioSpi != NULL )
    {
        //RadioNss = 0;
		RadioSpi->writeCommand(command,buffer,size);
        /*RadioSpi->write( ( uint8_t )command );
        for( uint16_t i = 0; i < size; i++ )
        {
            RadioSpi->write( buffer[i] );
        }*/
        //RadioNss = 1;
    }

    if( command != RADIO_SET_SLEEP )
    {
        WaitOnBusy( );
    }
}

void SX1280Hal::ReadCommand( RadioCommands_t command, uint8_t *buffer, uint16_t size )
{
    WaitOnBusy( );

    if( RadioSpi != NULL )
    {
        //RadioNss = 0;
        if( command == RADIO_GET_STATUS )
        {
            //buffer[0] = RadioSpi->write( ( uint8_t )command );
			//RadioSpi->write( 0 );
            //RadioSpi->write( 0 );
			unsigned char buffer_transfer[]={0,0};
			buffer[0] =RadioSpi->writeCommand(command,buffer_transfer,2);
        }
        else
        {
            //RadioSpi->write( ( uint8_t )command );
            //RadioSpi->write( 0 );
            //for( uint16_t i = 0; i < size; i++ )
            //{
            //     buffer[i] = RadioSpi->write( 0 );
            //}
			unsigned char buffer_transfer[]={( uint8_t )command,0};
			RadioSpi->readCommand(buffer_transfer,2,buffer,size);
			
        }
        //RadioNss = 1;
    }

    WaitOnBusy( );
}

void SX1280Hal::WriteRegister( uint16_t address, uint8_t *buffer, uint16_t size )
{
    WaitOnBusy( );

    if( RadioSpi != NULL )
    {
        //RadioNss = 0;
        //RadioSpi->write( RADIO_WRITE_REGISTER );
        //RadioSpi->write( ( address & 0xFF00 ) >> 8 );
        //RadioSpi->write( address & 0x00FF );
        //for( uint16_t i = 0; i < size; i++ )
        //{
        //    RadioSpi->write( buffer[i] );
        //}
		unsigned char buffer_transfer[]={RADIO_WRITE_REGISTER,(unsigned char) ((address & 0xFF00) >> 8), (unsigned char) (address & 0x00FF) };
		RadioSpi->writeBuffer(buffer_transfer,3,buffer,size);

        //RadioNss = 1;
    }

    WaitOnBusy( );
}

void SX1280Hal::WriteRegister( uint16_t address, uint8_t value )
{
    WriteRegister( address, &value, 1 );
}

void SX1280Hal::ReadRegister( uint16_t address, uint8_t *buffer, uint16_t size )
{
    WaitOnBusy( );

    if( RadioSpi != NULL )
    {
        //RadioNss = 0;
        //RadioSpi->write( RADIO_READ_REGISTER );
        //RadioSpi->write( ( address & 0xFF00 ) >> 8 );
        //RadioSpi->write( address & 0x00FF );
        //RadioSpi->write( 0 );
        //for( uint16_t i = 0; i < size; i++ )
        //{
        //    buffer[i] = RadioSpi->write( 0 );
        //}
		unsigned char buffer_transfer[]={RADIO_READ_REGISTER,(unsigned char) (( address & 0xFF00 ) >> 8), (unsigned char) (address & 0x00FF),0};
		RadioSpi->readCommand(buffer_transfer,4,buffer,size);
        
		//RadioNss = 1;
    }

    WaitOnBusy( );
}

uint8_t SX1280Hal::ReadRegister( uint16_t address )
{
    uint8_t data;

    ReadRegister( address, &data, 1 );
    return data;
}

void SX1280Hal::WriteBuffer( uint8_t offset, uint8_t *buffer, uint8_t size )
{
    WaitOnBusy( );

    if( RadioSpi != NULL )
    {
        //RadioNss = 0;
        //RadioSpi->write( RADIO_WRITE_BUFFER );
        //RadioSpi->write( offset );
        //for( uint16_t i = 0; i < size; i++ )
        //{
        //    RadioSpi->write( buffer[i] );
        //}
		unsigned char buffer_transfer[]={RADIO_WRITE_BUFFER,offset};
		RadioSpi->writeBuffer(buffer_transfer,2,buffer,size);
		
        //RadioNss = 1;
    }
   
    WaitOnBusy( );
}

void SX1280Hal::ReadBuffer( uint8_t offset, uint8_t *buffer, uint8_t size )
{
    WaitOnBusy( );

    if( RadioSpi != NULL )
    {
        //RadioNss = 0;
        //RadioSpi->write( RADIO_READ_BUFFER );
        //RadioSpi->write( offset );
        //RadioSpi->write( 0 );
        //for( uint16_t i = 0; i < size; i++ )
        //{
        //    buffer[i] = RadioSpi->write( 0 );
        //}
		unsigned char buffer_transfer[]={RADIO_READ_BUFFER,offset,0};
		RadioSpi->readCommand(buffer_transfer,3,buffer,size);
		
        //RadioNss = 1;
    }
    
    WaitOnBusy( );
}

uint8_t SX1280Hal::GetDioStatus( void )
{
    return ( *DIO3 << 3 ) | ( *DIO2 << 2 ) | ( *DIO1 << 1 ) | ( BUSY << 0 );
}
