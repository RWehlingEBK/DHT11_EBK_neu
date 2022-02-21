/*
 * DHT11_ITAS2022.c
 * Version 2
 * Author : RalfW
 */ 

#include <avr/io.h>
#define F_CPU 11059280
#include <util/delay.h>

//###############################################################
#define DHT11DDR	DDRA
#define DHT11PORT	PORTA
#define DHT11PIN	PINA
#define DHT11PWR	PINA5
#define DHT11DATA	PINA6



//###############################################################
//Zum Debuggen

#define DEBUGPORT	PORTD
#define DEBUGDDR	DDRD
#define DEBUGPIN	PIND

#define DEBUG_RESPONSE	PINB0
#define DEBUG_0			PINB1
#define DEBUG_1			PINB2
//###############################################################



#define DHT11TX	1
#define DHT11RX	0

#define DHT11HIGH 1
#define DHT11LOW  0

#define DHT11_18000US 18000


void initDHT11()
{
//Datenrichtung für die Versorgung des DHT11 setzen
DHT11DDR |=(1<<DHT11PWR);
//Ausgang auf 5V setzen für die Versorgung des DHT11
DHT11PORT|=(1<<DHT11PWR); 	
}

//Ausgänge um zu erkennen welche Daten wir empfangen
void initDHT11Debug()
{
//Datenrichtung setzen
DEBUGDDR |=(1<<DEBUG_RESPONSE) | (1<<DEBUG_0) | (1<<DEBUG_1);
}

void DHT11DebugToggle (uint8_t val)
{
//Beschreiben des PIN Registers bewirkt ein Umkippen des jeweiligen Bits
DEBUGPIN =val?(1<<DEBUG_1):(1<<DEBUG_0);	
};

void DHT11ResponseToggle ()
{
	//Beschreiben des PIN Registers bewirkt ein Umkippen des DEBUG_RESPONSE Bits
	DEBUGPIN =1<<DEBUG_RESPONSE;
};


void setDataPinDirection(uint8_t output)
{
if (output) 
	//Datenrichtung fÃ¼r Data auf 1 setzen TX
	DHT11DDR |=(1<<DHT11DATA);				
	else
	//Datenrichtung fÃ¼r Data auf 0 setzen RX
	DHT11DDR &=~(1<<DHT11DATA);
}


void setDataPinLevel(uint8_t high)
{
	if (high)
	//Output auf high setzen
	DHT11PORT |=(1<<DHT11DATA);
	else
	//Output auf low setzen
	DHT11PORT &=~(1<<DHT11DATA);
}

void startDHT11()
{
setDataPinDirection(DHT11TX);
setDataPinLevel(DHT11LOW);
_delay_us(DHT11_18000US);
setDataPinLevel(DHT11HIGH);
setDataPinDirection(DHT11RX);
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

