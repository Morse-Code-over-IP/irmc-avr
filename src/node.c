#include <string.h>
#include <stdio.h>

#include "config.h"
#include "irmc.h"

extern unsigned char _adc_reg;

// FIXME
void
mtc(struct node *c)
{    
#ifdef SERVER_MRX
	c->ipaddr[0] = 108; // FIXME: this is magic
	c->ipaddr[1] = 22; // FIXME: this is magic
	c->ipaddr[2] = 91; // FIXME: this is magic
	c->ipaddr[3] = 71; // FIXME: this is magic
#endif
    
#ifdef SERVER_KOB
    c->ipaddr[0] = 192; // FIXME: this is magic
    c->ipaddr[1] = 94; // FIXME: this is magic
    c->ipaddr[2] = 73; // FIXME: this is magic
    c->ipaddr[3] = 9; // FIXME: this is magic
//mtc-kob.dyndns.org
#endif
}


// FIXME
void
setnode(struct node *c)
{
	c->ch = CHANNEL;
	c->port = 7890; // FIXME: This is magic
    mtc(c);
}
