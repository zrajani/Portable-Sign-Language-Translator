/****************************************************************************
 Title :    ATmega CC2500 2.4 Ghz wireless transceiver library
 Author:    Shawn D'Souza <shawnelec@gmail.com>
 Software:  AVR-GCC 3.3 
 Target:    any AVR ATmega series device with SPI port

 DESCRIPTION
           this library for AVR Studio 4 is a simple interface for the CC2500 
           2.4 Ghz wireless transceiver from Texas Instruments Ltd. 
	   The source code is a modification of the sample code given by Texas 
           Instruments in addition to my own code. 
           The source code is done under the GPL and is free to modify and use. 
	   
	   please use 16Mhz crystal in the circuit and define F_CPU form the 
           main routine. 
	   
	   configure the SPI Port as per the microcontroller.
           for additional register settings, use the SmartRF Studio software 
           by Texas Instruments 

 
 USAGE
           add this header file as #include <cc2500lib.h>
*****************************************************************************/

#define BYTE   unsigned char
#define UINT8  unsigned char



#define pkt_len 5

//function definitions

void write_rf_setting();     //preset config
void set_channel(UINT8 ch);
void set_address(UINT8 add);
void write_rf_setting();     //preset config
void write_burst_reg(BYTE addr, BYTE *buffer, BYTE count);
void read_burst_reg(BYTE addr, BYTE *buffer, BYTE count);
void halRfWriteReg(BYTE addr, BYTE value); //for dynamic config
void strobe(BYTE strobe);
BYTE read_reg(BYTE addr);
BYTE read_status(BYTE addr);
void send_pkt(BYTE *txBuffer, UINT8 size);
BYTE receive_pkt(BYTE *rxBuffer, UINT8 length);
void POWER_UP_RESET_CC2500();
void SPI_write(BYTE value);
BYTE SPI_read();
void initTimer0();
void SPI_MasterInit();
void SPI_MasterTransmit(char);
void SPI_SlaveInit();
char SPI_SlaveReceive();
void cc2500_init();
void wake();

#define WRITE_BURST     0x40
#define READ_SINGLE     0x80
#define READ_BURST      0xC0

//-------------------- port definitions (change as per microcontroller)-------------------------
//defined for ATmega16,32,64

#define SPI_DDR		DDRB
#define SPI_PORT	PORTB 
#define	DD_CS	3	//chip select
#define DD_SS	4	//ss in master mode !!
#define DD_MOSI 5	//mosi
#define	DD_MISO	6	//miso
#define	DD_SCK	7	//sck

#define P_CSn	3
#define P_SI	5
#define P_SO	6
#define P_SCLK	7

//---------------------- CC 2500 register definitions--------------------------------------------

#define BYTE unsigned char
#define UINT8 unsigned char
//------------------------------------------------------------------------------------------------------
// CC2500 STROBE, CONTROL AND STATUS REGSITER
#define IOCFG2       0x00        // GDO2 output pin configuration
#define IOCFG1       0x01        // GDO1 output pin configuration
#define IOCFG0       0x02        // GDO0 output pin configuration
#define IOCFG0D      0x02        
#define FIFOTHR      0x03        // RX FIFO and TX FIFO thresholds
#define SYNC1        0x04        // Sync word, high byte
#define SYNC0        0x05        // Sync word, low byte
#define PKTLEN       0x06        // Packet length
#define PKTCTRL1     0x07        // Packet automation control
#define PKTCTRL0     0x08        // Packet automation control
#define ADDR         0x09        // Device address
#define CHANNR       0x0A        // Channel number
#define FSCTRL1      0x0B        // Frequency synthesizer control
#define FSCTRL0      0x0C        // Frequency synthesizer control
#define FREQ2        0x0D        // Frequency control word, high byte
#define FREQ1        0x0E        // Frequency control word, middle byte
#define FREQ0        0x0F        // Frequency control word, low byte
#define MDMCFG4      0x10        // Modem configuration
#define MDMCFG3      0x11        // Modem configuration
#define MDMCFG2      0x12        // Modem configuration
#define MDMCFG1      0x13        // Modem configuration
#define MDMCFG0      0x14        // Modem configuration
#define DEVIATN      0x15        // Modem deviation setting
#define MCSM2        0x16        // Main Radio Control State Machine configuration
#define MCSM1        0x17        // Main Radio Control State Machine configuration
#define MCSM0        0x18        // Main Radio Control State Machine configuration
#define FOCCFG       0x19        // Frequency Offset Compensation configuration
#define BSCFG        0x1A        // Bit Synchronization configuration
#define AGCCTRL2     0x1B        // AGC control
#define AGCCTRL1     0x1C        // AGC control
#define AGCCTRL0     0x1D        // AGC control
#define WOREVT1      0x1E        // High byte Event 0 timeout
#define WOREVT0      0x1F        // Low byte Event 0 timeout
#define WORCTRL      0x20        // Wake On Radio control
#define FREND1       0x21        // Front end RX configuration
#define FREND0       0x22        // Front end TX configuration
#define FSCAL3       0x23        // Frequency synthesizer calibration
#define FSCAL2       0x24        // Frequency synthesizer calibration
#define FSCAL1       0x25        // Frequency synthesizer calibration
#define FSCAL0       0x26        // Frequency synthesizer calibration
#define RCCTRL1      0x27        // RC oscillator configuration
#define RCCTRL0      0x28        // RC oscillator configuration
#define FSTEST       0x29        // Frequency synthesizer calibration control
#define PTEST        0x2A        // Production test
#define AGCTEST      0x2B        // AGC test
#define TEST2        0x2C        // Various test settings
#define TEST1        0x2D        // Various test settings
#define TEST0        0x2E        // Various test settings

// Strobe commands
#define SRES         0x30        // Reset chip.
#define SFSTXON      0x31        // Enable and calibrate frequency synthesizer (if MCSM0.FS_AUTOCAL=1).
                                        // If in RX/TX: Go to a wait state where only the synthesizer is
                                        // running (for quick RX / TX turnaround).
#define SXOFF        0x32        // Turn off crystal oscillator.
#define SCAL         0x33        // Calibrate frequency synthesizer and turn it off
                                        // (enables quick start).
#define SRX          0x34        // Enable RX. Perform calibration first if coming from IDLE and
                                        // MCSM0.FS_AUTOCAL=1.
#define STX          0x35        // In IDLE state: Enable TX. Perform calibration first if
                                        // MCSM0.FS_AUTOCAL=1. If in RX state and CCA is enabled:
                                        // Only go to TX if channel is clear.
#define SIDLE        0x36        // Exit RX / TX, turn off frequency synthesizer and exit
                                        // Wake-On-Radio mode if applicable.
#define SAFC         0x37        // Perform AFC adjustment of the frequency synthesizer
#define SWOR         0x38        // Start automatic RX polling sequence (Wake-on-Radio)
#define SPWD         0x39        // Enter power down mode when CSn goes high.
#define SFRX         0x3A        // Flush the RX FIFO buffer.
#define SFTX         0x3B        // Flush the TX FIFO buffer.
#define SWORRST      0x3C        // Reset real time clock.
#define SNOP         0x3D        // No operation. May be used to pad strobe commands to two
                                        // bytes for simpler software.

#define PARTNUM      0x30
#define VERSION      0x31
#define FREQEST      0x32
#define LQI          0x33
#define RSSI         0x34
#define MARCSTATE    0x35
#define WORTIME1     0x36
#define WORTIME0     0x37
#define PKTSTATUS    0x38
#define VCO_VC_DAC   0x39
#define TXBYTES      0x3A
#define RXBYTES      0x3B

#define PATABLE      0x3E
#define TXFIFO       0x3F
#define RXFIFO       0x3F


#define CRC_OK	0x80

//BYTE paTable[] = {0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
BYTE txBuffer[(pkt_len+1)];
BYTE rxBuffer[pkt_len];
BYTE rfstate;

//=======================================================================
//
//=======================================================================

void write_rf_setting()     //preset config for optimized sensitivity
{
     //SENSITIVITY OF THE UNIT IS OPTIMIZED...
	 
	 halRfWriteReg(FSCTRL1    ,0x06);   //CHANGE THIS TO INCREASE/REDUCE SENSITIVITY
	 halRfWriteReg(FSCTRL0    ,0x00);
	 halRfWriteReg(FREQ2      ,0x5D);
	 halRfWriteReg(FREQ1      ,0x93);
	 halRfWriteReg(FREQ0      ,0xB1);
	 halRfWriteReg(MDMCFG4    ,0x2D); //configuration of baud rate
	 halRfWriteReg(MDMCFG3    ,0x3B); //set MDMCFG0-4 for this
	 halRfWriteReg(MDMCFG2    ,0x73); //current baud rate is 250 Kbps
	 halRfWriteReg(MDMCFG1    ,0x22); //
	 halRfWriteReg(MDMCFG0    ,0xF8); //
	 halRfWriteReg(CHANNR     ,0x03);
	 halRfWriteReg(DEVIATN    ,0x00);
	 halRfWriteReg(FREND1     ,0xB6);
	 halRfWriteReg(FREND0     ,0x10);
	 halRfWriteReg(MCSM0      ,0x18);
	 halRfWriteReg(FOCCFG     ,0x1D);
	 halRfWriteReg(BSCFG      ,0x1C);
	 halRfWriteReg(AGCCTRL2   ,0xC7);
	 halRfWriteReg(AGCCTRL1   ,0x00);
	 halRfWriteReg(AGCCTRL0   ,0xB2);
	 halRfWriteReg(FSCAL3     ,0xEA);
	 halRfWriteReg(FSCAL2     ,0x0A);
	 halRfWriteReg(FSCAL1     ,0x00);
	 halRfWriteReg(FSCAL0     ,0x11);
	 halRfWriteReg(FSTEST     ,0x59);
	 halRfWriteReg(TEST2      ,0x88);
	 halRfWriteReg(TEST1      ,0x31);
	 halRfWriteReg(TEST0      ,0x0B);
	 halRfWriteReg(FIFOTHR    ,0x07);
	 halRfWriteReg(IOCFG2     ,0x06);
	 halRfWriteReg(IOCFG0D    ,0x01);
	 halRfWriteReg(PKTCTRL1   ,0x04);
	 halRfWriteReg(PKTCTRL0   ,0x05);
	 halRfWriteReg(ADDR       ,0x00);
	 halRfWriteReg(PKTLEN     ,0xFF);


}


void set_channel(uint8_t ch)   //transmitter/receiver channel number
{
halRfWriteReg(CHANNR     , ch);
}


void set_address(uint8_t add)   //transmitter/receiver address
{
halRfWriteReg(ADDR     , add);
}




void wake()
{
        rfstate=read_status(MARCSTATE);	//check sleep state and other
		if((rfstate& 0x1f)==0x00)
		{
	
			POWER_UP_RESET_CC2500();
			write_rf_setting();
			
		}
}


void cc2500_init()   // initialize the CC2500 chip and SPI
{
        initTimer0();
        SPI_MasterInit();	
	POWER_UP_RESET_CC2500();
	write_rf_setting();
	//write_burst_reg(PATABLE, paTable, sizeof(paTable));
	halRfWriteReg(PATABLE,0xFF);// 1 dbm output power
}



void data_tx()
{   
        halRfWriteReg(PATABLE,0xFF);
	halRfWriteReg(FREND0, 0x10);
	_delay_ms(25);	

    txBuffer[0]= pkt_len; //packet length

   	send_pkt(txBuffer, sizeof(txBuffer));
//	_delay_ms(25);	
}

unsigned char data_rx(void)
{	
	//unsigned char	length;
	//length = sizeof(rxBuffer);
    if (receive_pkt(rxBuffer, pkt_len))	
		return 1;
	else
		return 0;
		
}

void initTimer0(){
	/* clkI/O/64 (From prescaler) */
	TCCR0 = (0<<CS02)|(1<<CS01)|(1<<CS00);
	TIMSK = 0;
}


void SPI_MasterInit(void)
{
	/* Set MOSI and SCK output, all others input */
	sbi(SPI_DDR, DD_CS);	//cs
	sbi(SPI_DDR, DD_SS);	//ss
	sbi(SPI_DDR, DD_MOSI);	//mosi
	cbi(SPI_DDR, DD_MISO);	//miso
	sbi(SPI_DDR, DD_SCK);	//sck
	
	/*	Enable SPI, Master, set clock rate 
		10 is fck/64	*/
	SPCR = (1<<SPE)|(1<<MSTR)|(0<<SPR1)|(0<<SPR0);	
	SPSR |= (0<<SPI2X);

}
void SPI_MasterTransmit(char cData)
{
	cbi(SPI_PORT, DD_CS);

	/* Start transmission */
	SPDR = cData;
	
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));

	sbi(SPI_PORT, DD_CS);
}

void SPI_SlaveInit(void)
{
	/* Set MISO output, all others input */
	cbi(SPI_DDR, DD_SS);	//ss
	cbi(SPI_DDR, DD_MOSI);	//mosi
	sbi(SPI_DDR, DD_MISO);	//miso
	cbi(SPI_DDR, DD_SCK);	//sck
	
	/* Enable SPI */
	SPCR = (1<<SPE);
}
char SPI_SlaveReceive(void)
{
	/* Wait for reception complete */
	while(!(SPSR & (1<<SPIF)))
	;
	/* Return data register */
	return SPDR;
}



void SPI_write(BYTE cData)
{
	/* Start transmission */
	SPDR = cData;
	
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));
}

BYTE SPI_transfer_byte(BYTE addr)
{
	SPDR = addr;

	/* Wait for reception complete */
	while(!(SPSR & (1<<SPIF)));
	
	/* Return data register */
	return SPDR;
}
//----------------------------------------------------------------
//
//=======================================================================
BYTE SPI_read()
{
	//Wait for reception complete 
	while(!(SPSR & (1<<SPIF)));
	
	// Return data register 
	return SPDR;
}
//=======================================================================

//-------------------------------------------------------------------------------------------------------
//  BYTE read_reg(BYTE addr)
//
//  DESCRIPTION:
//      This function gets the value of a single specified CC2500 register.
//
//  ARGUMENTS:
//      BYTE addr
//          Address of the CC2500 register to be accessed.
//
//  RETURN VALUE:
//      BYTE
//          Value of the accessed CC2500 register.
//-------------------------------------------------------------------------------------------------------
BYTE read_reg(BYTE addr) 
{
	unsigned char value;
	cbi(SPI_PORT, P_CSn);
	while(bit_is_set(PINB, P_SO));
//	cbi(SPI_PORT, P_SCLK);
	addr|=READ_SINGLE;
	SPI_write(addr);
	
	value = SPI_transfer_byte(addr);
	sbi(SPI_PORT, P_CSn);
//	cbi(SPI_PORT, P_SCLK);
//	cbi(SPI_PORT, P_SI);
	return value;
}


//-------------------------------------------------------------------------------------------------------
//  BYTE read_status(BYTE addr)
//
//  DESCRIPTION:
//      This function reads a CC2500 status register.
//
//  ARGUMENTS:
//      BYTE addr
//          Address of the CC2500 status register to be accessed.
//
//  RETURN VALUE:
//      BYTE
//          Value of the accessed CC2500 status register.
//-------------------------------------------------------------------------------------------------------
BYTE read_status(BYTE addr) 
{
	unsigned char value;
	cbi(SPI_PORT, P_CSn);
	while(bit_is_set(PINB, P_SO));
//	cbi(SPI_PORT, P_SCLK);
	addr|=READ_BURST;
	SPI_write(addr);

	value = SPI_transfer_byte(addr);

	sbi(SPI_PORT, P_CSn);
//	cbi(SPI_PORT, P_SCLK);
//	cbi(SPI_PORT, P_SI);
	return value;
}// read_status


//-------------------------------------------------------------------------------------------------------
//  void halRfWriteReg(BYTE addr, BYTE value)
//
//  DESCRIPTION:
//      Function for writing to a single CC2500 register
//
//  ARGUMENTS:
//      BYTE addr
//          Address of a specific CC2500 register to accessed.
//      BYTE value
//          Value to be written to the specified CC2500 register.
//-------------------------------------------------------------------------------------------------------
void halRfWriteReg(BYTE addr, BYTE value) 
{
	cbi(SPI_PORT, P_CSn);
	while(bit_is_set(PINB, P_SO));
//	cbi(SPI_PORT, P_SCLK);
	addr&=0x7F;
	SPI_write(addr);
	SPI_write(value);
//	cbi(SPI_PORT, P_SI);
	sbi(SPI_PORT, P_CSn);

}// halRfWriteReg


//-------------------------------------------------------------------------------------------------------
//  void read_burst_reg(BYTE addr, BYTE *buffer, BYTE count)
//
//  DESCRIPTION:
//      This function reads multiple CC2500 register, using SPI burst access.
//
//  ARGUMENTS:
//      BYTE addr
//          Address of the first CC2500 register to be accessed.
//      BYTE *buffer
//          Pointer to a byte array which stores the values read from a
//          corresponding range of CC2500 registers.
//      BYTE count
//          Number of bytes to be written to the subsequent CC2500 registers.
//-------------------------------------------------------------------------------------------------------
void read_burst_reg(BYTE addr, BYTE *buffer, BYTE count) 
{
	unsigned char j,value;
	cbi(SPI_PORT, P_CSn);
	while(bit_is_set(PINB, P_SO));
 //	cbi(SPI_PORT, P_SCLK);
	addr|=READ_BURST;
	SPI_write(addr);
	for(j=0;j<count;j++) 
	{
	  value = SPI_transfer_byte(addr);
	  buffer[j]=value;
	}
	sbi(SPI_PORT, P_CSn);
}// read_burst_reg



//-------------------------------------------------------------------------------------------------------
//  void write_burst_reg(BYTE addr, BYTE *buffer, BYTE count)
//
//  DESCRIPTION:
//      This function writes to multiple CC2500 register, using SPI burst access.
//
//  ARGUMENTS:
//      BYTE addr
//          Address of the first CC2500 register to be accessed.
//      BYTE *buffer
//          Array of bytes to be written into a corresponding range of
//          CCxx00 registers, starting by the address specified in _addr_.
//      BYTE count
//          Number of bytes to be written to the subsequent CC2500 registers.   
//-------------------------------------------------------------------------------------------------------
void write_burst_reg(BYTE addr, BYTE *buffer, BYTE count)
{
	unsigned char i,j,value;
	cbi(SPI_PORT, P_CSn);
	while(bit_is_set(PINB, P_SO));
	addr|=WRITE_BURST;
	SPI_write(addr);
	for(j=0;j<count;j++) {
	  value=buffer[j];
	  SPI_write(value);
	  for(i=0;i<10;i++);
	}
	sbi(SPI_PORT, P_CSn);
}// write_burst_reg




//-------------------------------------------------------------------------------------------------------
//  void strobe(BYTE strobe)
//
//  DESCRIPTION:
//      Function for writing a strobe command to the CC2500
//
//  ARGUMENTS:
//      BYTE strobe
//          Strobe command
//-------------------------------------------------------------------------------------------------------
void strobe(BYTE strobe) 
{
	cbi(SPI_PORT, P_CSn);
	
	while(bit_is_set(PINB, P_SO));
	SPI_write(strobe);
	sbi(SPI_PORT, P_CSn);

}// strobe



//-------------------------------------------------------------------------------------------------------
//  void send_pkt(BYTE *txBuffer, UINT8 size)
//

//      
//  ARGUMENTS:
//      BYTE *txBuffer
//          Pointer to a buffer containg the data that are going to be transmitted
//
//      UINT8 size
//          The size of the txBuffer
//-------------------------------------------------------------------------------------------------------

void send_pkt(BYTE *txBuffer, UINT8 size) 
{
	strobe(SFTX);
	_delay_ms(10);
	write_burst_reg(TXFIFO, txBuffer, size);
	_delay_ms(10);
	strobe(STX);
	_delay_ms(10);

	TCNT0=0;
	while((read_status(TXBYTES)&0x7F)!=0x00)	
	{
		
		if(TCNT0 == 255){ return; }
			
	}

	return ;
}// send_pkt*/


//-------------------------------------------------------------------------------------------------------
//  BOOL receive_pkt(BYTE *rxBuffer, UINT8 *length)
//

//  
//  ARGUMENTS:
//      BYTE *rxBuffer
//          Pointer to the buffer where the incoming data should be stored
//      UINT8 *length
//          Pointer to a variable containing the size of the buffer where the incoming data should be
//          stored. After this function returns, that variable holds the packet length.
//          
//  RETURN VALUE:
//      BOOL
//          TRUE:   CRC OK
//          FALSE:  CRC NOT OK
//-------------------------------------------------------------------------------------------------------
BYTE receive_pkt(BYTE *rxBuffer, UINT8 length) 
{
    BYTE status=0;
    UINT8 packetLength;
	status = read_status(RXBYTES);
	
	if((status & 0x7F)==0x00) 
	{
		
		if(read_status(MARCSTATE)!=0x0D) strobe(SRX);
		return 0;
	}
	
	TCNT0=0;
	while((read_status(MARCSTATE)&0x1f)!=0x01) 
	{
		
		if(TCNT0 == 255)
		{ 
			strobe(SFRX);
			return 0;
		}

	}
   packetLength = read_reg(RXFIFO);
    
    if (packetLength == length) {
		read_burst_reg(RXFIFO, rxBuffer, packetLength); 
        
        strobe(SFRX);
        
        return 1;//(status & CRC_OK);
    } else {
		strobe(SFRX);
       return 0;
    }  
}// receive_pkt



//------------------------------------------------------------
//
//------------------------------------------------------------
void POWER_UP_RESET_CC2500() {
	
	strobe(SIDLE);
	sbi(SPI_PORT, P_CSn);
	_delay_us(5);
	cbi(SPI_PORT, P_CSn);
	_delay_us(10);  	 
	sbi(SPI_PORT, P_CSn);
	_delay_us(40);
	cbi(SPI_PORT, P_CSn);
	while(bit_is_set(PINB, P_SO));
	strobe(SRES);
	sbi(SPI_PORT, P_CSn);
//	cbi(SPI_PORT, P_SCLK);
//	cbi(SPI_PORT, P_SI);
}


//==============================================================

