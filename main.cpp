/*
 * Copyright 2018 Fausto Annicchiarico Petruzzelli <petruzzelli.fausto@gmail.com>
 * Copyright 2018 Luigi Zevola <luigi.zevola@gmail.com>
 *
 * This file is part of DreamcatcherChat.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include "mbed.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "rffc5071.h"
#include "rffc5071_spi.h"
#include "radio.h"
#include "sx1280-hal.h"
#include <pthread.h>
#include <poll.h>

#define MODE_LORA

/*!
 * \brief Defines the nominal frequency
 */
#define RF_FREQUENCY                                2400000000UL // Hz

/*!
 * \brief Defines the output power in dBm
 *
 * \remark The range of the output power is [-18..+13] dBm
 */
#define TX_OUTPUT_POWER                             13

/*!
 * \brief Defines the states of the application
 */
typedef enum
{
    APP_LOWPOWER,
    APP_RX,
    APP_RX_TIMEOUT,
    APP_RX_ERROR,
    APP_TX,
    APP_TX_TIMEOUT,
}AppStates_t;

/*!
 * \brief Defines the buffer size, i.e. the payload size
 */
#define BUFFER_SIZE                                 255
#define TOKENSIZE 									1
#define HEADER_SIZE									TOKENSIZE+2

/*!
 * \brief Define the possible message type for this application
 */
const uint8_t MasterToken[] = "M";
const uint8_t SlaveToken[]  = "S";


/*!
 * \brief The size of the buffer
 */
uint8_t BufferSize = BUFFER_SIZE;

/*!
 * \brief The buffer
 */
uint8_t Buffer[BUFFER_SIZE];

/*!
 * \brief The State of the application
 */
AppStates_t AppState = APP_LOWPOWER;

int8_t RssiValue = 0;
int8_t SnrValue = 0;

/*!
 * \brief Function to be executed on Radio Tx Done event
 */
void OnTxDone( void );

/*!
 * \brief Function to be executed on Radio Rx Done event
 */
void OnRxDone( void );

/*!
 * \brief Function executed on Radio Tx Timeout event
 */
void OnTxTimeout( void );

/*!
 * \brief Function executed on Radio Rx Timeout event
 */
void OnRxTimeout( void );

/*!
 * \brief Function executed on Radio Rx Error event
 */
void OnRxError( IrqErrorCode_t );

/*!
 * \brief All the callbacks are stored in a structure
 */
RadioCallbacks_t callbacks =
{
    &OnTxDone,        // txDone
    &OnRxDone,        // rxDone
    NULL,             // syncWordDone
    NULL,             // headerDone
    &OnTxTimeout,     // txTimeout
    &OnRxTimeout,     // rxTimeout
    &OnRxError,       // rxError
    NULL,             // rangingDone
    NULL,             // cadDone
};

// mosi, miso, sclk, nss, busy, dio1, dio2, dio3, rst, callbacks...
SX1280Hal Radio( 999, 999, 999, 999, 36, 35, 999, 999,115, &callbacks );

DigitalOut ANT_SW( 999 );
DigitalOut TxLed( 101 );
DigitalOut RxLed( 999 );

/*!
 * \brief Define IO for Unused Pin
 */
DigitalOut F_CS( 999 );      // MBED description of pin
DigitalOut SD_CS( 999 );     // MBED description of pin

/*!
 * \brief Number of tick size steps for tx timeout
 */
#define TX_TIMEOUT_VALUE                            100 // ms

/*!
 * \brief Number of tick size steps for rx timeout
 */
#define RX_TIMEOUT_VALUE                            100 // ms

/*!
 * \brief Size of ticks (used for Tx and Rx timeout)
 */
#define RX_TIMEOUT_TICK_SIZE                        RADIO_TICK_SIZE_1000_US

/*!
 * \brief Mask of IRQs to listen to in rx mode
 */
uint16_t RxIrqMask = IRQ_RX_DONE | IRQ_RX_TX_TIMEOUT;

/*!
 * \brief Mask of IRQs to listen to in tx mode
 */
uint16_t TxIrqMask = IRQ_TX_DONE | IRQ_RX_TX_TIMEOUT;

/*!
 * \brief Locals parameters and status for radio API
 * NEED TO BE OPTIMIZED, COPY OF STUCTURE ALREADY EXISTING
 */
PacketParams_t PacketParams;
PacketStatus_t PacketStatus;

gpio_t_rffc gpio_rffc5072_select;
gpio_t_rffc gpio_rffc5072_clock;
gpio_t_rffc gpio_rffc5072_data;
gpio_t_rffc gpio_rffc5072_reset;

rffc5071_spi_config_t rffc5071_spi_config;/* = {
 	.gpio_select = gpio_rffc5072_select,
 	.gpio_clock = gpio_rffc5072_clock,
 	.gpio_data = gpio_rffc5072_data,
 };*/

spi_bus_t spi_bus_rffc5071
{
	.obj=NULL,
 	.config = &rffc5071_spi_config,
 	.start = rffc5071_spi_start,
 	.stop = rffc5071_spi_stop,
 	.transfer = rffc5071_spi_transfer,
 	.transfer_gather = rffc5071_spi_transfer_gather,
};

rffc5071_driver_t mixer= {
 	.bus = &spi_bus_rffc5071,
 	.gpio_reset = gpio_rffc5072_reset,
 };

uint8_t broadcast_msg[BUFFER_SIZE-HEADER_SIZE];
uint8_t bcast_msg_len = 0;
uint16_t bcast_frame_no = 0;

uint8_t my_msg[BUFFER_SIZE-HEADER_SIZE];
uint8_t my_msg_len = 0;

struct threadargs
{
  int fd;
};

int pipefd_term[2];
int pipefd_radio[2];

void broadcast_message()
{
	#ifdef DEBUG
        printf( "Sending message to slaves (frame %hu)\r\n", bcast_frame_no);
	#endif

	
	int ret = read(pipefd_radio[0], broadcast_msg, BUFFER_SIZE-HEADER_SIZE);

	if (ret != -1)
	{
		bcast_frame_no = bcast_frame_no + 1;
		bcast_msg_len = ret;
		broadcast_msg[bcast_msg_len-1] = '\0';
		#ifdef DEBUG_COM
			printf("%d,%d:%s\r\n",ret,strlen((char*)broadcast_msg),broadcast_msg);
		#endif
	}

	TxLed = 1;

	memcpy( Buffer, 			MasterToken, 		TOKENSIZE );
	memcpy( Buffer+TOKENSIZE,	&bcast_frame_no,	2);
	memcpy( Buffer+HEADER_SIZE,	broadcast_msg,		bcast_msg_len);

    Radio.SetDioIrqParams( TxIrqMask, TxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE );
    Radio.SendPayload( Buffer, HEADER_SIZE+bcast_msg_len, ( TickTime_t ){ RX_TIMEOUT_TICK_SIZE, TX_TIMEOUT_VALUE } );
}


void send_message()
{
	// A master already exists then become a slave
	#ifdef DEBUG
    	printf( "Sending message to master\r\n" );
	#endif

	int ret = read(pipefd_radio[0], my_msg, BUFFER_SIZE-HEADER_SIZE);
	
	if (ret != -1)
	{
		bcast_frame_no = bcast_frame_no + 1;
		my_msg_len = ret;
		my_msg[my_msg_len-1] = '\0';
		#ifdef DEBUG_COM
			printf("%d,%d:%s\r\n",ret,strlen((char*)my_msg),my_msg);
		#endif
	}
	
	TxLed = 1;
	memcpy( Buffer, 			SlaveToken, 		TOKENSIZE );
	memcpy( Buffer+TOKENSIZE ,  &bcast_frame_no, 	2);
	memcpy( Buffer+HEADER_SIZE, my_msg, 			my_msg_len);

    Radio.SetDioIrqParams( TxIrqMask, TxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE );
    Radio.SendPayload( Buffer, HEADER_SIZE+my_msg_len, ( TickTime_t ){ RX_TIMEOUT_TICK_SIZE, TX_TIMEOUT_VALUE } );
}

void receive_message()
{
	 TxLed = 0;
	 Radio.SetDioIrqParams( RxIrqMask, RxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE );
     Radio.SetRx( ( TickTime_t ) { RX_TIMEOUT_TICK_SIZE, RX_TIMEOUT_VALUE } );
}

void * consumer(void *p)
{
  int fd = reinterpret_cast<struct threadargs *>(p)->fd;
  fcntl(fd, F_SETFL, O_NONBLOCK);
  uint8_t buffer[BUFFER_SIZE-HEADER_SIZE];

  for (;;) {
    if (read(fd, buffer, BUFFER_SIZE-HEADER_SIZE)!=-1)
	{	
		printf(">%s\r\n",buffer);
	}
  }
}

void * producer(void *p)
{
	int fd = reinterpret_cast<struct threadargs *>(p)->fd;
	char buffer[BUFFER_SIZE-HEADER_SIZE];

	for (;;) {
		fgets(buffer, BUFFER_SIZE-HEADER_SIZE, stdin);
		write(fd, (void *)buffer, strlen(buffer));
	}
}

void process_message(uint16_t frame_no)
{
	int recv_str_len = strlen((char *)(Buffer+HEADER_SIZE));
   	#ifdef DEBUG_COM
		printf("String length is %d, Recv buffer size is %d",recv_str_len,BufferSize);
	#endif
	write(pipefd_term[1], Buffer+HEADER_SIZE, recv_str_len+1);
	
	bcast_frame_no = frame_no;
}

int main()
{
	bool isMaster = true;
    ModulationParams_t modulationParams; 
	uint16_t mixer_freq = 0;
	pthread_t p,c;
	struct threadargs p_args,c_args;

	printf( "\n\n\r     Dreamcatcher Chat (v.%s)\n\n\r", "0.0 alpha" );

	gpio_rffc5072_select=(gpio_t_rffc)malloc(sizeof(struct gpio_rffc));	
	if (gpio_rffc5072_select==NULL)
		printf("Pointer is NULL\n");
	gpio_rffc5072_select->pin=135;

	gpio_rffc5072_clock=(gpio_t_rffc)malloc(sizeof(struct gpio_rffc));	
	if (gpio_rffc5072_clock==NULL)
		printf("Pointer is NULL\n");
	gpio_rffc5072_clock->pin=134;

	gpio_rffc5072_data=(gpio_t_rffc)malloc(sizeof(struct gpio_rffc));	
	if (gpio_rffc5072_data==NULL)
		printf("Pointer is NULL\n");
	gpio_rffc5072_data->pin=133;

	gpio_rffc5072_reset=(gpio_t_rffc)malloc(sizeof(struct gpio_rffc));	
	if (gpio_rffc5072_reset==NULL)
		printf("Pointer is NULL\n");
	gpio_rffc5072_reset->pin=136;

	rffc5071_spi_config.gpio_select=gpio_rffc5072_select;
	rffc5071_spi_config.gpio_clock= gpio_rffc5072_clock;
	rffc5071_spi_config.gpio_data=  gpio_rffc5072_data;
	mixer.gpio_reset=gpio_rffc5072_reset;
 
	spi_bus_start(&spi_bus_rffc5071, &rffc5071_spi_config);

	rffc5071_setup(&mixer);

    // wait for on board DC/DC start-up time
    usleep( 500000 ); 

    Radio.Init( );
    Radio.SetRegulatorMode( USE_DCDC );

    memset( &Buffer, 0x00, BufferSize );

    modulationParams.PacketType                  = PACKET_TYPE_LORA;
    modulationParams.Params.LoRa.SpreadingFactor = LORA_SF7;
    modulationParams.Params.LoRa.Bandwidth       = LORA_BW_0400;
    modulationParams.Params.LoRa.CodingRate      = LORA_CR_4_5;

    PacketParams.PacketType                 = PACKET_TYPE_LORA;
    PacketParams.Params.LoRa.PreambleLength = 0x08;
    PacketParams.Params.LoRa.HeaderType     = LORA_PACKET_VARIABLE_LENGTH;
    PacketParams.Params.LoRa.PayloadLength  = 50;
    PacketParams.Params.LoRa.Crc            = LORA_CRC_ON;
    PacketParams.Params.LoRa.InvertIQ       = LORA_IQ_INVERTED;

	while (!mixer_freq)
	{
		printf("Select operating frequency (MHz):");
		if ((scanf("%hu",&mixer_freq)==1)&&(mixer_freq>=40)&&(mixer_freq<=6000))
		{
			printf("Selected mixer frequency: %hu MHz\r\n",mixer_freq);
		    rffc5071_set_frequency(&mixer, mixer_freq);
		}else{
			printf("Invalid choice. Range is 40 MHz to 6 GHz\r\n");
			mixer_freq=0;
		}
	}

    if (pipe(pipefd_radio) != 0) {
      perror("pipe");
      exit(1);
    }

    if (pipe(pipefd_term) != 0) {
      perror("pipe");
      exit(1);
    }
	
	fcntl(pipefd_radio[0], F_SETFL, O_NONBLOCK);

	c_args.fd = pipefd_term[0];
	p_args.fd = pipefd_radio[1];

	pthread_create(&p,NULL,&producer,(void *) &p_args);
	pthread_create(&c,NULL,&consumer,(void *) &c_args);

    Radio.SetStandby( STDBY_RC );
    Radio.SetPacketType( modulationParams.PacketType );
    Radio.SetModulationParams( &modulationParams );
    Radio.SetPacketParams( &PacketParams );

	Radio.SetRfFrequency( RF_FREQUENCY );
    Radio.SetBufferBaseAddresses( 0x00, 0x00 );
    Radio.SetTxParams( TX_OUTPUT_POWER, RADIO_RAMP_20_US );

    receive_message();
    AppState = APP_LOWPOWER;

	while(1)
	{
		switch( AppState )
        {
            case APP_RX:
                AppState = APP_LOWPOWER;
                Radio.GetPayload( Buffer, &BufferSize, BUFFER_SIZE );

                if ((isMaster)&&(BufferSize>0)){
					
                    if( strncmp( (const char*)Buffer, (const char*)SlaveToken, TOKENSIZE ) == 0 )
                    {
						uint16_t frame_no;
						memcpy(&frame_no,Buffer+TOKENSIZE,2);
						if (frame_no > bcast_frame_no)
						{
							// A new message has been received from slave.
							process_message(frame_no);
						}
						
						broadcast_message();
                    } else if( strncmp( ( const char* )Buffer, ( const char* )MasterToken, TOKENSIZE ) == 0 ) {
						// Found another master, becoming slave
						isMaster = false;

						uint16_t frame_no;
						memcpy(&frame_no,Buffer+TOKENSIZE,2);

						// A new message has been received. Synchronize frame number
						process_message(frame_no);
						
                        send_message();
                    } else {    // Set device as master ans start again
						#ifdef DEBUG_COM
							for (int i =0;i<BufferSize;i++)
							{
								printf("%02x ",Buffer[i]);
							}
							printf( "...no valid token received (%d)\r\n",BufferSize );
						#endif
						isMaster = true; 
                        receive_message();
                    }
                }else if ((!isMaster)&&(BufferSize>0)){
					
                    if( strncmp( ( const char* )Buffer, ( const char* )MasterToken, TOKENSIZE ) == 0 )
                    {
						#ifdef DEBUG
                        	printf( "Received message from Master\r\n" );
						#endif

						uint16_t frame_no;
						memcpy(&frame_no,Buffer+TOKENSIZE,2);

						if (frame_no > bcast_frame_no)
						{
							// A new message has been received from master.
							process_message(frame_no);
						}
						
                        send_message();
                    }
                    else // valid reception but not a PING as expected
                    {
						#ifdef DEBUG
							printf( "waiting for new message\r\n" );                            
						#endif
						isMaster = true;
                        receive_message();
                    }
                }
                break;
				
				case APP_TX:
                AppState = APP_LOWPOWER;
                receive_message();
                break;

            case APP_RX_TIMEOUT:
                AppState = APP_LOWPOWER;
                if( isMaster == true )
                {
                    // Send the message again
                    broadcast_message();
					
                }else{
					receive_message();
                }
                break;

            case APP_RX_ERROR:
                AppState = APP_LOWPOWER;
                // We have received a Packet with a CRC error, send reply as if packet was correct
                if(isMaster)
                    broadcast_message();
                else
                    send_message();
                break;

            case APP_TX_TIMEOUT:
                AppState = APP_LOWPOWER;
                receive_message();
                break;

            case APP_LOWPOWER:
                break;

            default:
                // Set low power
				AppState = APP_LOWPOWER;
                break;
        }

	}
}


void OnTxDone( void )
{
    AppState = APP_TX;
}

void OnRxDone( void )
{
    AppState = APP_RX;
	#ifdef PRINT_RSSI_ON_RX
    	Radio.GetPacketStatus(&packetStatus);
    	RssiValue = packetStatus.LoRa.RssiPkt;
    	SnrValue = packetStatus.LoRa.SnrPkt;
    	printf("rssi: %d; snr: %d\n\r", RssiValue, SnrValue );
	#endif
}

void OnTxTimeout( void )
{
    AppState = APP_TX_TIMEOUT;
    printf( "Transmission Timeout\r\n" );
}

void OnRxTimeout( void )
{
    AppState = APP_RX_TIMEOUT;
	//printf( "No Response\r\n" );
}

void OnRxError( IrqErrorCode_t errorCode )
{
    AppState = APP_RX_ERROR;
    printf( "RX packet with CRC error\r\n" );
}

void OnRangingDone( IrqRangingCode_t val )
{
}

void OnCadDone( bool channelActivityDetected )
{
}
