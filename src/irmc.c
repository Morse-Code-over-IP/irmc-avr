#include "config.h"

#include <avr/io.h>
#include <util/delay.h>

#include <string.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include "irmc.h"

#ifdef ETHERNET
    #include "w5100.h"
#endif

unsigned long tx_sequence = 0, rx_sequence;
unsigned long _timer_reg;
unsigned char _adc_reg = 0;

void
timer_init()
{
	TIMSK1 |= (1 << TOIE1);
	TCCR1B |= (1 << CS10);
}

ISR(TIMER1_OVF_vect)
{
	_timer_reg++;
}

unsigned long
fastclock()
{
	unsigned long ms;

	ms = (double) (_timer_reg * (65536 * 1000 / F_CPU)) + (double) (TCNT1 * (1000 / F_CPU));

	return(ms);
}

void
identifyclient(struct dp *c, struct cp *n, struct node *s)
{
	c->command = DAT;
	c->length = 492;
	n->channel = s->ch;
	snprintf(c->id, 128, "%s", s->myid);
	snprintf(c->status, 128, "irmc avr 0.01");
	c->n = 0;
        c->a21 = 1;     /* These magic numbers was provided by Les Kerr */
        c->a22 = 755;
        c->a23 = 65535;
	tx_sequence++;
	c->sequence = tx_sequence;
#ifdef ETHERNET
	w5100_sendto((unsigned char *)n, 4, s->ipaddr, s->port); // FIXME
	w5100_sendto((unsigned char *)c, 496, s->ipaddr, s->port);
#endif
}

void
wait(unsigned int ms)
{
	unsigned int i;

	for(i = 0; i < ms; i++)_delay_ms(1);
}

void
sounder(struct dp *c)
{
	unsigned short i;
	unsigned int cycles;

	for(i = 0; i < c->n; i++){
        // Beep
		if(c->code[i] > 0){
#ifdef BLINK
            PORTD |= (1 << PIN_LED_DATA); // blink
#endif
			cycles = c->code[i] * 1000 / CYCLE;
			while(cycles > 0){
				PORTD |= (1 << PIN_SPEAKER);
				_delay_us(HALFCYCLE);
				PORTD &= ~(1 << PIN_SPEAKER);
				_delay_us(HALFCYCLE);
				cycles--;
			}
		}
        // No beep
		if(c->code[i] < 0){
			if((c->code[i] * -1) > 1000) c->code[i] = -1000;
#ifdef BLINK
            PORTD &= ~(1 << PIN_LED_DATA); // blink
#endif
			PORTD &= ~(1 << PIN_SPEAKER);
			wait(-1 * c->code[i]);
		}
	}
}

void
txloop(struct dp *c, struct node *s)
{
	int i;
	unsigned long kp, kr;
	unsigned long timeout = 0;

	c->command = DAT;
	c->length = 492;
        c->a21 = 1;     /* These magic numbers were provided by Les Kerr */
        c->a22 = 755;
        c->a23 = 16777215;
	snprintf(c->status, 128, "?"); // FIXME
	snprintf(c->id, 128, "%s", s->myid); // FIXME
	kp = fastclock();
	kr = 0;
	c->n++;
	c->code[c->n - 1] = -2000;
	for(;;){
		while(PINC & (1 << PIN_KEY)) kr = fastclock();
		c->n++;
		c->code[c->n - 1] = kr - kp;
		for(;;){
			if(PINC & (1 << PIN_KEY)){
				kp = fastclock();
				c->n++;
				c->code[c->n - 1] = -1 * (kp - kr);
				break;
			}
			timeout = fastclock() - kr;
			if((timeout > KEYUPTIMEOUT && c->n > 1) || (c->n > 50)){
				PORTD |= (1 << PIN_LED_CONNECT);
				tx_sequence++;
				c->sequence = tx_sequence;
				for(i = 0; i < 2; i++)
                {
#ifdef ETHERNET
					w5100_sendto((unsigned char *)c, 496, s->ipaddr, s->port); // FIXME
#endif
                }
				c->n = 0;
			}
			if(timeout > TXTIME) return;
		}
	}
}

ISR(ADC_vect)
{
	_adc_reg = ADCH;
	ADCSRA |= (1 << ADSC); /* restart adc */
}

int
main()
{
	unsigned long keepalive_t = 0;
	unsigned char buf[MAXDATASIZE];
	unsigned int n, stat = 0, adccurr = 0;
	unsigned long dummyip ;
	unsigned int dummyport = 0;

	struct cp connectmsg = {CON, 0};
	struct cp disconnectmsg = {DIS, 0};
	struct dp data;
	struct node node;
	for(n = 0; n < 51; n++) data.code[n] = 0;

	DDRD |= (1 << PIN_LED_CONNECT);
#ifdef BLINK
    DDRD |= (1 << PIN_LED_DATA);
#endif
	timer_init();
	usart_init();
	spi_init();
	DDRD |= (1 << PIN_SPEAKER); 
	DDRC &= ~(1 << PIN_KEY); 
	
	/* configure adc input for channel presets */
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	ADMUX |= (1 << REFS0); 
	ADMUX |= (1 << ADLAR);
	ADCSRA |= (1 << ADEN); /* enable adc */
	ADCSRA |= (1 << ADIE); /* enable adc interrupt */

#ifdef ETHERNET
	w5100_init(); // FIXME
	w5100_socket_open(Sn_MR_UDP, LOCAL_PORT, 0);
#endif
	sei();
	ADCSRA |= (1 << ADSC); /* start adc */
	adccurr = _adc_reg;
	setnode(&node);
	for(;;){
		if(keepalive_t <= 0){
			if(adccurr != _adc_reg){
#ifdef ETHERNET
				w5100_sendto((unsigned char *)&disconnectmsg, 4, node.ipaddr, node.port); // FIXME
#endif
				adccurr = _adc_reg;
				setnode(&node);
			}
			identifyclient(&data, &connectmsg, &node);
			keepalive_t = KEEPALIVE_CYCLE;
		}
		keepalive_t--;

		if(PINC & (1 << PIN_KEY)){ // FIXME: remove PINC
			txloop(&data, &node);
		}

#ifdef ETHERNET
		n = w5100_recvfrom(buf, MAXDATASIZE-1, (unsigned char *)&dummyip, &dummyport); // FIXME
#endif
		if(n == 2) PORTD |= (1 << PIN_LED_CONNECT);
		if(n == 496){
			PORTD |= (1 << PIN_LED_CONNECT);
			memcpy(&data, buf, 496);
			if(data.n > 0 && rx_sequence != data.sequence){
				rx_sequence = data.sequence;
				sounder(&data);
			}
		}

		stat++;
		if(stat == STATUSTIMER){
			stat = 0;
			PORTD &= ~(1 << PIN_LED_CONNECT);
		}
	}
	return 0;
}

