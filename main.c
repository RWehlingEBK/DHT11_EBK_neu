/*
 * DHT11_ITAS2022.c
 * Version 2
 * Author : RalfW
 */ 

#include <avr/io.h>

#include <util/delay.h>
#include "lcd-routines.h"

//###############################################################
#define DHT11DDR	DDRA
#define DHT11PORT	PORTA
#define DHT11PIN	PINA

#define DHT11PWR	PINB5
#define DHT11DATA	PINB6



//###############################################################
//Zum Debuggen

#define DEBUGPORT		PORTA
#define DEBUGDDR		DDRA
#define DEBUGPIN		PINA

#define DEBUG_RESPONSE	PINB0
#define DEBUG_0			PINB1
#define DEBUG_1			PINB2
//###############################################################

#define DHT_RESPONSE_START		2
#define DHT_RESPONSE_0			0
#define DHT_RESPONSE_1			1
#define DHT_NO_RESPONSE			3
#define DHT_TELEGRAM_BYTES		5
#define DHT_TELEGRAM_HIGH_BIT  39


//###############################################################

#define DHT11TX	1
#define DHT11RX	0

#define DHT11HIGH 1
#define DHT11LOW  0

#define DHT11_18000US 18000

//###############################################################
void debugvalue(uint8_t wert)
{
	DDRC=0xff;
	PORTC=wert;
	_delay_ms(200);
}


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
	//Datenrichtung für Data auf 1 setzen TX
	DHT11DDR |=(1<<DHT11DATA);				
	else
	//Datenrichtung für Data auf 0 setzen RX
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

//liefert wahr zurück wenn das signal auf dem IO-Pin high ist
uint8_t recvPinHigh()
{
	return DHT11PIN & (1<< DHT11DATA);
}


uint8_t decodeSignal_alt()
{
	uint8_t t_high=0;
	uint8_t t_low=0;
	uint16_t time_out=65000;
	
	//warten bis das Signal 0 wird
	while(recvPinHigh())
		{

		if(time_out-- ==0) 
							return 	DHT_NO_RESPONSE;
		};
	//Zeit messen wie lange das Signal 0 ist
	while(!recvPinHigh())
	{
		t_low++;
		_delay_us(1);
	}

	//Zeit messen wie lange das Signal 1 ist
	while(recvPinHigh())
	{
		t_high++;
		_delay_us(1);
	}
	//debugtime(t_high);
	if(t_high > 50)				//80us 61us gemessen
	return DHT_RESPONSE_START;	//

	if(t_high > 40)
	
	return DHT_RESPONSE_1;		//70us   49us gemessen
	//debugtime(t_high);
	return DHT_RESPONSE_0;		//26-28us  17us
}
uint8_t decodeSignalSchule()
{
	//warten bis das Signal 0 wird
	while(DHT11PIN & (1<< DHT11DATA));
	//warten bis das Signal 1 wird
	while(!(DHT11PIN & (1<< DHT11DATA)));
	_delay_us(26);
	if(!(DHT11PIN & (1<< DHT11DATA)))
		return 0;
	_delay_us(44);	
	if(!(DHT11PIN & (1<< DHT11DATA)))
		return 1;
	_delay_us(10);		
	return 3;

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

void DHT11getData( uint8_t * werte)
{
uint8_t bitwert;
	for (uint8_t i=0;i<40;i++)
		{
			bitwert=decodeSignal();
			//if(!((i/8)%2))
			DHT11DebugToggle(bitwert);
			
			if (bitwert==1)
				werte[i/8]|=0b10000000 >>(i%8);//werte[i/8]|=(bitwert==1?1:0)<<(i%8);
		}

}

int main(void)
{
initDHT11();
initDHT11Debug();
lcd_init();
lcd_clear();
lcd_string("Start");
_delay_ms(1000);
uint8_t bitwert;
uint8_t werte[5];

    /* Replace with your application code */
    while (1) 
    {
	for (int i=0;i<5;i++)
		werte[i]=0;	
	startDHT11();
	

					
		if(decodeSignal()==DHT_RESPONSE_START)
		{
		DHT11ResponseToggle ();

		DHT11getData(werte);
		}
	lcd_clear();

	
		
	for (int i=0;i<5;i++)
			{	
			lcd_int(werte[i]);
			lcd_string(" ");
			}
uint8_t summe=0;
	for (int i=0;i<4;i++)
	{
		summe+=werte[i];

	}			
lcd_int(summe);			
			
	debugvalue(werte[2]);	
	_delay_ms(5000);
    }
}

