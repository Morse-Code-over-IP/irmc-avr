#include <util/delay.h>
#include "w5100.h"


extern unsigned char w5100_subnet[];

unsigned char
w5100_socket_open(unsigned char protocol, unsigned int port, unsigned char flag)
{

	w5100_socket_close();
	w5100_write(S0_MR, protocol | flag);
	w5100_write(S0_PORT0, (unsigned char) ((port & 0xff00) >> 8));
	w5100_write((S0_PORT0 + 1), (unsigned char) (port & 0x00ff));
	w5100_write(S0_CR, Sn_CR_OPEN);
	while(w5100_read(S0_CR));
	
	return 1;
}

void
w5100_socket_close()
{
	w5100_write(S0_CR, Sn_CR_CLOSE);
	while(w5100_read(S0_CR));

}

int
w5100_sendto(unsigned char * buf, unsigned int len, unsigned char * addr, unsigned int port)
{
	unsigned int tx_wr0, tx_rd0, timeout = 0;


        w5100_write(S0_DIPR0 + 0, addr[0]);
        w5100_write(S0_DIPR0 + 1, addr[1]);
        w5100_write(S0_DIPR0 + 2, addr[2]);
        w5100_write(S0_DIPR0 + 3, addr[3]);

	w5100_write(S0_DPORT0, (unsigned char)((port & 0xff00) >> 8));
	w5100_write((S0_DPORT0 + 1), (unsigned char)(port & 0x00ff));

	send_data_processing((unsigned char *) buf, len);
	set_subnet(w5100_subnet);

	w5100_write(S0_CR, Sn_CR_SEND);
	
	while(w5100_read(S0_CR));

	do{
	        tx_wr0 = w5100_read(S0_TX_WR0);
	        tx_wr0 = ((tx_wr0 & 0x00ff) << 8) + w5100_read(S0_TX_WR0 + 1);
	        tx_rd0 = w5100_read(S0_TX_RD0);
	        tx_rd0 = ((tx_rd0 & 0x00ff) << 8) + w5100_read(S0_TX_RD0 + 1);
		_delay_ms(1);
		timeout++;
		if(timeout > 100) return 1;
		if(timeout > 2){
			w5100_init();
			w5100_socket_open(Sn_MR_UDP, LOCAL_PORT, 0);
		}
	}while(tx_wr0 != tx_rd0);

	clear_subnet();
        return 0;
}


unsigned int
w5100_recvfrom(unsigned char * buf, unsigned int len, unsigned char * addr, unsigned int * port)
{
	unsigned char head[8];
	unsigned int data_len=0;
	unsigned int ptr=0;
	
	if(w5100_rxcheck() == 0) return 0;

	if ( len > 0 )
	{
		ptr = w5100_read(S0_RX_RD0);
		ptr = ((ptr & 0x00ff) << 8) + w5100_read(S0_RX_RD0 + 1);
		
		switch(w5100_read(S0_MR) & 0x07)
		{
			case Sn_MR_UDP :
				read_data((unsigned char *)ptr, head, 0x08);
				ptr += 8;
				/* read peer's IP address, port number. */
				addr[0] = head[0];
				addr[1] = head[1];
				addr[2] = head[2];
				addr[3] = head[3];
				*port = head[4];
				*port = (*port << 8) + head[5];
				data_len = head[6];
				data_len = (data_len << 8) + head[7];
				
				read_data((unsigned char *)ptr, buf, data_len); /* data copy. */
				ptr += data_len;
				
				w5100_write(S0_RX_RD0,(unsigned char)((ptr & 0xff00) >> 8));
				w5100_write((S0_RX_RD0 + 1),(unsigned char)(ptr & 0x00ff));
				break;
			
			case Sn_MR_IPRAW :
				read_data((unsigned char *)ptr, head, 0x06);
				ptr += 6;
				
				addr[0] = head[0];
				addr[1] = head[1];
				addr[2] = head[2];
				addr[3] = head[3];
				data_len = head[4];
				data_len = (data_len << 8) + head[5];
				
				read_data((unsigned char *)ptr, buf, data_len); /* data copy. */
				ptr += data_len;
				
				w5100_write(S0_RX_RD0,(unsigned char)((ptr & 0xff00) >> 8));
				w5100_write((S0_RX_RD0 + 1),(unsigned char)(ptr & 0x00ff));
				break;
			case Sn_MR_MACRAW :
				read_data((unsigned char *)ptr,head,2);
				ptr+=2;
				data_len = head[0];
				data_len = (data_len<<8) + head[1] - 2;
				
				read_data((unsigned char *) ptr,buf,data_len);
				ptr += data_len;
				w5100_write(S0_RX_RD0,(unsigned char)((ptr & 0xff00) >> 8));
				w5100_write((S0_RX_RD0 + 1),(unsigned char)(ptr & 0x00ff));
				
				break;
			
			default :
				break;
		}
		w5100_write(S0_CR,Sn_CR_RECV);
		
		while( w5100_read(S0_CR) );
		   
	}
	
	return data_len;
}
