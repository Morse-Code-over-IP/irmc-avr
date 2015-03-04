/* Definitions for the IP connection */

struct node{
    unsigned char ipaddr[4];
    unsigned int port;
    unsigned int ch;
    char myid[128];
};

void setnode(struct node *);
