#include <string.h>
#include <stdio.h>
#include "irmc.h"

extern unsigned char _adc_reg;


void
mtc(struct node *c)
{
	char id[] = "FE (Fernan in Penang, MY) Intl. Morse Only";

	snprintf(c->myid, 128, "%s", id);
	c->ipaddr[0] = 50;
	c->ipaddr[1] = 135;
	c->ipaddr[2] = 251;
	c->ipaddr[3] = 176;
}

void 
morse(struct node *c)
{
		char id[] = "fernan";

		snprintf(c->myid, 128, "%s", id);
		c->ipaddr[0] = 208;
		c->ipaddr[1] = 111;
		c->ipaddr[2] = 40;
		c->ipaddr[3] = 60;
}

void
setnode(struct node *c)
{
	if(_adc_reg < 11){
		mtc(c);
		c->ch = 100;
		c->port = 7891;
	} else if((_adc_reg > 11) && (_adc_reg < 32)){
		morse(c);
		c->ch = 100;
		c->port = 7890;
	} else if((_adc_reg > 32) && (_adc_reg < 53)){
		morse(c);
		c->ch = 994;
		c->port = 7890;
	} else if((_adc_reg > 53) && (_adc_reg < 74)){
		morse(c);
		c->ch = 1000;
		c->port = 7890;
	} else if((_adc_reg > 74) && (_adc_reg < 96)){
		morse(c);
		c->ch = 1010;
		c->port = 7890;
	} else if((_adc_reg > 96) && (_adc_reg < 117)){
		morse(c);
		c->ch = 1020;
		c->port = 7890;
	} else if((_adc_reg > 117) && (_adc_reg < 138)){
		morse(c);
		c->ch = 1030;
		c->port = 7890;
	} else if((_adc_reg > 138) && (_adc_reg < 159)){
		morse(c);
		c->ch = 1040;
		c->port = 7890;
	} else if((_adc_reg > 159) && (_adc_reg < 181)){
		morse(c);
		c->ch = 2346;
		c->port = 7890;
	} else if((_adc_reg > 159) && (_adc_reg < 202)){
		mtc(c);
		c->ch = 11;
		c->port = 7890;
	} else if((_adc_reg > 202) && (_adc_reg < 223)){
		mtc(c);
		c->ch = 16;
		c->port = 7890;
	} else if(_adc_reg > 223){
		mtc(c);
		c->ch = 22;
		c->port = 7890;
	}
}
