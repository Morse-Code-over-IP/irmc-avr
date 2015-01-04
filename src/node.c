#include <string.h>
#include <stdio.h>
#include "irmc.h"

extern unsigned char _adc_reg;


void
mtc(struct node *c)
{
	char id[] = "DG6FL/AVR"; // FIXME: this is magic

	snprintf(c->myid, 128, "%s", id);
	c->ipaddr[0] = 192; // FIXME: this is magic
	c->ipaddr[1] = 94; // FIXME: this is magic
	c->ipaddr[2] = 73; // FIXME: this is magic
	c->ipaddr[3] = 9; // FIXME: this is magic
//mtc-kob.dyndns.org
// 192.94.73.9
}

void
setnode(struct node *c)
{
	c->ch = 33;
	c->port = 7890;
        mtc(c);
}
