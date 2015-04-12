#define TXTIME 1000
#define KEYUPTIMEOUT 200
#define STATUSTIMER 3000

#define KEEPALIVE_CYCLE 80000
#define MAXDATASIZE 498

/* Definitions for tone generator */
#define PITCH 650		// frequency of the tone
#define CYCLE (1000000 / PITCH)
#define HALFCYCLE CYCLE / 2

#define DIS 0x0002
#define DAT 0x0003
#define CON 0x0004
#define ACK 0x0005

struct cp{
        unsigned short command;
        unsigned short channel;
};

struct dp{
        unsigned short command;
        unsigned short length;
        char id[128];
        char a1[4];
        unsigned long sequence;
        unsigned long a21;
        unsigned long a22;
        unsigned long a23;
        signed long code[51];
        unsigned long n;
        char status[128];  /* This is called version in MorseKob */
        char a4[8];

};

struct node{
	unsigned char ipaddr[4];
	unsigned int port;
	unsigned int ch;
	char myid[128];
};

void setnode(struct node *);
