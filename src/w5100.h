

#define USART_BAUDRATE 19200
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

#define SPI_OUT   PORTB
#define SPI_IN    PINB
#define SPI_DDR   DDRB
#define SPI_MOSI  PB3
#define SPI_MISO  PB4
#define SPI_SCK   PB5
#define SPI_SS  PB2
#define LOCAL_PORT 49888

#define WIZNET_WRITE_OPCODE 0xF0
#define WIZNET_READ_OPCODE 0x0F

#define MR  	0x0000   /* mode register */
#define GAR0 	0x0001   /* gateway address: 0x0001 to 0x0004 */
#define SUBR0	0x0005   /* subnet mask address: 0x0005 to 0x0008 */
#define SHAR0 	0x0009   /* source hardware address (MAC): 0x0009 to 0x000E */
#define SIPR0	0x000F   /* source IP address: 0x000F to 0x0012 */
#define RMSR	0x001A   /* RX memory size register */
#define TMSR	0x001B   /* TX memory size register */

#define MR_RST	0x80 /* wiznet reset */

#define S0_MR	0x0400 /* socket 0 mode register */
#define S0_CR	0x0401 /* socket 0 channel register */
#define S0_IR	0x0402 /* socket 0 channel interrupt register */
#define S0_SR	0x0403 /* socket 0 channel status register */
#define S0_PORT0	0x0404 /* socket 0 source port register */
#define S0_DIPR0	0x040c
#define S0_DPORT0	0x0410
#define S0_TX_FSR0 0x0420
#define S0_TX_RD0 0x0422
#define S0_TX_WR0 0x0424
#define S0_RX_RSR0 0x0426
#define S0_RX_RD0 0x0428

/* Sn_MR values */
#define Sn_MR_CLOSE 	0x00  /* unused socket */
#define Sn_MR_TCP 	0x01  /* TCP */
#define Sn_MR_UDP 	0x02  /* UDP */
#define Sn_MR_IPRAW	0x03  /* IP LAYER RAW SOCK */
#define Sn_MR_MACRAW	0x04  /* MAC LAYER RAW SOCK */
#define Sn_MR_PPPOE 	0x05  /* PPPoE */
#define Sn_MR_ND 	0x20  /* No Delayed Ack(TCP) flag */
#define Sn_MR_MULTI 	0x80  /* support multicating */


/* Sn_CR values */
#define Sn_CR_OPEN 	0x01  /* initialize or open socket */
#define Sn_CR_LISTEN 	0x02  /* wait connection request in tcp mode(Server mode) */
#define Sn_CR_CONNECT	0x04  /* send connection request in tcp mode(Client mode) */
#define Sn_CR_DISCON 	0x08  /* send closing reqeuset in tcp mode */
#define Sn_CR_CLOSE 	0x10  /* close socket */
#define Sn_CR_SEND 	0x20  /* updata txbuf pointer, send data */
#define Sn_CR_SEND_MAC	0x21  /* send data with MAC address, so without ARP process */
#define Sn_CR_SEND_KEEP	0x22  /*  send keep alive message */
#define Sn_CR_RECV 	0x40  /* update rxbuf pointer, recv data */

/* Sn_SR values */
#define SOCK_CLOSED      	0x00  /* closed */
#define SOCK_INIT       	0x13  /* init state */
#define SOCK_LISTEN      	0x14  /* listen state */
#define SOCK_SYNSENT        	0x15  /* connection state */
#define SOCK_SYNRECV        	0x16  /* connection state */
#define SOCK_ESTABLISHED    	0x17  /* success to connect */
#define SOCK_FIN_WAIT     	0x18  /* closing state */
#define SOCK_CLOSING        	0x1A  /* closing state */
#define SOCK_TIME_WAIT     	0x1B  /* closing state */
#define SOCK_CLOSE_WAIT     	0x1C  /* closing state */
#define SOCK_LAST_ACK     	0x1D  /* closing state */
#define SOCK_UDP         	0x22  /* udp socket */
#define SOCK_IPRAW        	0x32  /* ip raw mode socket */
#define SOCK_MACRAW        	0x42  /* mac raw mode socket */
#define SOCK_PPPOE      	0x5F  /* pppoe socket */

/* These are applies to only when rx/tx memory is set to 2K */
/* RMSR = 0x55 and TMSR = 0x55 */
#define RX_BASE 0x6000
#define RX_MASK 0x07ff
#define TX_BASE 0x4000
#define TX_MASK 0x07ff


void write_data(unsigned char *, unsigned char *, unsigned int);
void read_data(unsigned char *, unsigned char *, unsigned int);
unsigned int wiz_write_buf(unsigned int, unsigned char *, unsigned int);
unsigned int wiz_read_buf(unsigned int, unsigned char *, unsigned int);
void usart_init(void);
int usart_send(unsigned char);
int usart_recv(void);
void send_data_processing(unsigned char *, unsigned int);
void w5100_init(void);
void spi_init(void);
void spi_send(unsigned char);
void w5100_write(unsigned int, unsigned char);
unsigned char w5100_read(unsigned int);
unsigned int w5100_read16(unsigned int);
unsigned int w5100_rxcheck(void);
void clear_subnet(void);
void set_subnet(unsigned char *);
void set_gatewat(unsigned char *);
void set_ip(unsigned char *);
void set_mac(unsigned char *);

void get_subnet(unsigned char *);
void get_gatewat(unsigned char *);
void get_ip(unsigned char *);
void get_mac(unsigned char *);

unsigned char w5100_socket_open(unsigned char, unsigned int, unsigned char);
void w5100_socket_close(void);
int w5100_sendto(unsigned char *, unsigned int, unsigned char *, unsigned int);
unsigned int w5100_recvfrom(unsigned char *, unsigned int, unsigned char *, unsigned int *);

