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

#define DHT_RESPONSE_START		2
#define DHT_RESPONSE_0			0
#define DHT_RESPONSE_1			1
#define DHT_NO_RESPONSE			3
#define DHT_TELEGRAM_BYTES		5
#define DHT_TELEGRAM_HIGH_BIT  39

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

uint8_t decodeSignal()
{

	uint16_t time_out=65000;
	
	//warten bis das Signal 0 wird
	while(recvPinHigh())
	{
		if(time_out-- ==0)
		return 	DHT_NO_RESPONSE;
	};
	//warten bis das Signal 1 wird
	while(!recvPinHigh())
	{
		if(time_out-- ==0)
		return 	DHT_NO_RESPONSE;
	};
	_delay_us(26);
	if(!recvPinHigh()) return DHT_RESPONSE_0;//26-28us  17us
	
	_delay_us(71-26);
	if(!recvPinHigh()) return DHT_RESPONSE_1;//70us   49us gemessen
	
	_delay_us(88-71);
	if(!recvPinHigh()) return DHT_RESPONSE_START;//80us 61us gemessen
	
return DHT_NO_RESPONSE;
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

