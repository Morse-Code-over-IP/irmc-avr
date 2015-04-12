#include <avr/io.h>
#include <util/delay.h>

#include "pins.h"
#include "w5100.h"

unsigned char w5100_subnet[4]; /* needed for the ARP errata */



void
spi_init()
{
	SPI_DDR = (1<<SPI_MOSI)|(1<<SPI_SCK)|(1<<SPI_SS);
	SPI_OUT |= (1<<SPI_SS); /* reset w5100 */
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0); /* Enable SPI, Master, clock fck/16 */
}

void
spi_send(unsigned char b)
{
	SPDR = b;
	while(!(SPSR & (1<<SPIF)));
	
}

void
w5100_write(unsigned int addr, unsigned char data)
{
	SPI_OUT &=~(1<<SPI_SS);
	spi_send(WIZNET_WRITE_OPCODE);
	spi_send((addr & 0xff00) >> 8);
	spi_send(addr & 0x00ff);
	spi_send(data);
	SPI_OUT |= (1<<SPI_SS);

}


unsigned char
w5100_read(unsigned int addr)
{
	unsigned char b;

	SPI_OUT &=~(1<<SPI_SS);
	spi_send(WIZNET_READ_OPCODE);
	spi_send((addr & 0xff00) >> 8);
	spi_send(addr & 0x00ff);
	spi_send(0);
	b = SPDR;

	SPI_OUT |= (1<<SPI_SS);

	return(b);
}

void
w5100_init()
{

	static unsigned char subnet_mask[] = {255,255,255,0}; // FIXME: this is magic number
	static unsigned char mac_address[] = {0xde, 0xad, 0xbe, 0xef, 0xed, 0x00}; // FIXME: this is magic number
	static unsigned char ip_address[] = {192,168,1,12}; // FIXME: this is magic number
	static unsigned char gateway_address[] = {192,168,1,1}; // FIXME: this is magic number

	w5100_write(MR, MR_RST); /* reset w5100 data */

	/* Assign 2K TX/RX memory socket */
	w5100_write(RMSR,0x55);
	w5100_write(TMSR,0x55);

	w5100_write(SHAR0 + 0, mac_address[0]);
	w5100_write(SHAR0 + 1, mac_address[1]);
	w5100_write(SHAR0 + 2, mac_address[2]);
	w5100_write(SHAR0 + 3, mac_address[3]);
	w5100_write(SHAR0 + 4, mac_address[4]);
	w5100_write(SHAR0 + 5, mac_address[5]);

	w5100_write(GAR0 + 0, gateway_address[0]);
	w5100_write(GAR0 + 1, gateway_address[1]);
	w5100_write(GAR0 + 2, gateway_address[2]);
	w5100_write(GAR0 + 3, gateway_address[3]);

	clear_subnet(); /* w5100 chip ARP errata */
	w5100_subnet[0] = subnet_mask[0];
	w5100_subnet[1] = subnet_mask[1];
	w5100_subnet[2] = subnet_mask[2];
	w5100_subnet[3] = subnet_mask[3];

//	w5100_write(SIPR0 + 0, ip_address[0]);
//	w5100_write(SIPR0 + 1, ip_address[1]);
//	w5100_write(SIPR0 + 2, ip_address[2]);
//	w5100_write(SIPR0 + 3, ip_address[3]);
	set_ip(ip_address);

}



void
usart_init()
{
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0);
	UCSR0C |= (1 << UMSEL01) | (1 << UCSZ00) | (1 << UCSZ01);
	UBRR0H = (BAUD_PRESCALE >> 8);
	UBRR0L = BAUD_PRESCALE;

}


int
usart_send(unsigned char b)
{
	while((UCSR0A & (1 << UDRE0)) == 0);
	UDR0 = b;

	return 0;
}

int 
usart_recv()
{
	if((UCSR0A & (1 << RXC0)) == 0) return 1;
	else return UDR0;
}

/* check the tx_write buffer for space and copy data in tx_write buffer */
void
send_data_processing(unsigned char * data, unsigned int len)
{
	unsigned int ptr;
	ptr = w5100_read(S0_TX_WR0);
	ptr = ((ptr & 0x00ff) << 8) + w5100_read(S0_TX_WR0 + 1);
	write_data(data, (unsigned char *)(ptr), len);
	ptr += len;
	w5100_write(S0_TX_WR0,(unsigned char)((ptr & 0xff00) >> 8));
	w5100_write((S0_TX_WR0 + 1),(unsigned char)(ptr & 0x00ff));
}


void
write_data(unsigned char * src, unsigned char * dst, unsigned int len)
{
	int size;
	unsigned int dst_mask;
	unsigned char * dst_ptr;

	dst_mask = (unsigned int)dst & TX_MASK;
	dst_ptr = (unsigned char *)(TX_BASE + dst_mask);
	
//	while(w5100_read16(S0_TX_FSR0) < len)_delay_ms(1);
	if((dst_mask + len) > 2048)
	{
		size = 2048 - dst_mask;
		wiz_write_buf((int)dst_ptr, (unsigned char*)src, size);
		src += size;
		size = len - size;
		dst_ptr = (unsigned char *)(TX_BASE);
		wiz_write_buf((int)dst_ptr, (unsigned char*)src, size);
	} 
	else
	{
		wiz_write_buf((unsigned int)dst_ptr, (unsigned char*)src, len);
	}
}

unsigned int
wiz_write_buf(unsigned int addr, unsigned char * buf, unsigned int len)
{
	unsigned int i;

	for(i = 0; i < len; i++){
		SPI_OUT &=~(1<<SPI_SS);
		spi_send(WIZNET_WRITE_OPCODE);
		spi_send(((addr + i) & 0xff00) >> 8);
		spi_send((addr + i) & 0x00ff);
		spi_send(buf[i]);
		SPI_OUT |= (1<<SPI_SS);
	}

	return len;
}

unsigned int
w5100_read16(unsigned int addr)
{
		unsigned int val = 0, buf = 0;

		do{
			val = w5100_read(S0_TX_FSR0);
			val = (val << 8) + w5100_read(S0_TX_FSR0 + 1);
			if(val != 0){
				buf = w5100_read(S0_TX_FSR0);
				buf = (buf << 8) + w5100_read(S0_TX_FSR0 + 1);
			}
		} while(val != buf);
		
		return buf;
}


unsigned int
w5100_rxcheck()
{
		unsigned int val = 0, buf = 0;

		do{
			val = w5100_read(S0_RX_RSR0);
			val = (val << 8) + w5100_read(S0_RX_RSR0 + 1);
			if(val != 0){
				buf = w5100_read(S0_RX_RSR0);
				buf = (buf << 8) + w5100_read(S0_RX_RSR0 + 1);
			}
		} while(val != buf);
		
		return val;
}

void
read_data(unsigned char * src, unsigned char * dst, unsigned int len)
{
	unsigned int size;
	unsigned int src_mask;
	unsigned char * src_ptr;

	src_mask = (unsigned int)src & RX_MASK;
	src_ptr = (unsigned char *)(RX_BASE + src_mask);
	

	if( (src_mask + len) > 2048) 
	{
		size = 2048 - src_mask;
		wiz_read_buf((unsigned int)src_ptr, (unsigned char*)dst,size);
		dst += size;
		size = len - size;
		src_ptr = (unsigned char *)(RX_BASE);
		wiz_read_buf((unsigned int)src_ptr, (unsigned char*) dst,size);
	} 
	else
	{
		wiz_read_buf((unsigned int)src_ptr, (unsigned char*) dst,len);
	}
}


/**
@brief	This function reads into W5100 memory(Buffer)
*/ 
unsigned int
wiz_read_buf(unsigned int addr, unsigned char * buf, unsigned int len)
{
	unsigned int i;
 
	for (i = 0; i < len; i++)
      	{
		SPI_OUT &=~(1<<SPI_SS);
		spi_send(0x0F);
		spi_send(((addr+i) & 0xff00) >> 8);
		spi_send((addr+i) & 0x00ff);
		spi_send(0);
		buf[i] = SPDR;
		SPI_OUT |= (1<<SPI_SS);
	}

	return len;
}


void
clear_subnet()
{
	w5100_write(SUBR0 + 0, 0);
	w5100_write(SUBR0 + 1, 0);
	w5100_write(SUBR0 + 2, 0);
	w5100_write(SUBR0 + 3, 0);
}


void
set_subnet(unsigned char * addr)
{
	w5100_write(SUBR0 + 0, addr[0]);
	w5100_write(SUBR0 + 1, addr[1]);
	w5100_write(SUBR0 + 2, addr[2]);
	w5100_write(SUBR0 + 3, addr[3]);
}

void
set_gateway(unsigned char * addr)
{
	w5100_write(GAR0 + 0, addr[0]);
	w5100_write(GAR0 + 1, addr[1]);
	w5100_write(GAR0 + 2, addr[2]);
	w5100_write(GAR0 + 3, addr[3]);
}

void
set_ip(unsigned char * addr)
{
	w5100_write(SIPR0 + 0, addr[0]);
	w5100_write(SIPR0 + 1, addr[1]);
	w5100_write(SIPR0 + 2, addr[2]);
	w5100_write(SIPR0 + 3, addr[3]);
}

void
set_mac(unsigned char * addr)
{
	w5100_write(SHAR0 + 0, addr[0]);
	w5100_write(SHAR0 + 1, addr[1]);
	w5100_write(SHAR0 + 2, addr[2]);
	w5100_write(SHAR0 + 3, addr[3]);
	w5100_write(SHAR0 + 4, addr[4]);
	w5100_write(SHAR0 + 5, addr[5]);
}

void
get_ip(unsigned char * addr)
{
	addr[0] = w5100_read(SIPR0 + 0);
	addr[1] = w5100_read(SIPR0 + 1);
	addr[2] = w5100_read(SIPR0 + 2);
	addr[3] = w5100_read(SIPR0 + 3);
}

void
get_gateway(unsigned char * addr)
{
	addr[0] = w5100_read(GAR0 + 0);
	addr[1] = w5100_read(GAR0 + 1);
	addr[2] = w5100_read(GAR0 + 2);
	addr[3] = w5100_read(GAR0 + 3);
}

void
get_subnet(unsigned char * addr)
{
	addr[0] = w5100_read(SUBR0 + 0);
	addr[1] = w5100_read(SUBR0 + 1);
	addr[2] = w5100_read(SUBR0 + 2);
	addr[3] = w5100_read(SUBR0 + 3);
}

void
get_mac(unsigned char * addr)
{
	addr[0] = w5100_read(SHAR0 + 0);
	addr[1] = w5100_read(SHAR0 + 1);
	addr[2] = w5100_read(SHAR0 + 2);
	addr[3] = w5100_read(SHAR0 + 3);
	addr[4] = w5100_read(SHAR0 + 4);
	addr[5] = w5100_read(SHAR0 + 5);
}

