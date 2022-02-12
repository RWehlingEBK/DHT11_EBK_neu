/*
 * DHT11_ITAS2022.c
 * Version 2
 * Author : RalfW
 */ 

#include <avr/io.h>
#define F_CPU 11059280
#include <util/delay.h>

#define DHT11DDR	DDRA
#define DHT11PORT	PORTA
#define DHT11PIN	PINB
#define DHT11PWR	PINB5
#define DHT11DATA	PINB6


#define DHT11TX	1
#define DHT11RX	0

#define DHT11HIGH 1
#define DHT11LOW  0

//###############################################################
//Zum Debuggen
#define DEBUGPORT	PORTD
#define DEBUGDDR	DDRD
#define DEBUGPIN	PIND

#define DEBUG_RESPONSE	PINB0
#define DEBUG_0			PINB1
#define DEBUG_1			PINB2
//###############################################################


void initDHT11()
{
//Datenrichtung setzen
DHT11DDR =(1<<DHT11PWR);
//Pin auf 5V schalten mit dem Port Register
DHT11PORT=(1<<DHT11PWR); 	
}

//Damit wir erkennen welche Daten empfangen wurden
void initDHT11Debug()
{
	//Datenrichtung setzen
	DHT11DDR =(1<<DEBUG_RESPONSE) | (1<<DEBUG_0) | (1<<DEBUG_1);
}

void DHT11DebugToggle (uint8_t val)
{
DHT11PIN =val?(1<<DEBUG_1):(1<<DEBUG_0);	
};


void setDataDirection(uint8_t output)
{
if (output) 
	//Datenrichtung für Data auf 1 setzen TX
	DHT11DDR |=(1<<DHT11DATA);				
	//DHT11DDR |= (1<<DHT11DATA);	gleichwertig 
	else
	//Datenrichtung für Data auf 0 setzen RX
	DHT11DDR &=~(1<<DHT11DATA);
}


void setDatapin(uint8_t high)
{
	if (high)
	//Output auf high setzen
	DHT11PORT |=(1<<DHT11DATA);
	//DHT11DDR |= (1<<DHT11DATA);	gleichwertig
	else
	//Output auf low setzen
	DHT11PORT &=~(1<<DHT11DATA);
}

void startDHT11()
{
setDataDirection(DHT11TX);
setDatapin(DHT11LOW);
_delay_us(18000);
setDatapin(DHT11HIGH);
setDataDirection(DHT11RX);
}


int main(void)
{
initDHT11();

initDHT11Debug();
    /* Replace with your application code */
    while (1) 
    {
	startDHT11();
	_delay_ms(500);
    }
}

