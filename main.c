/*
 * DHT11_ITAS2022.c
 * Version 2
 * Author : RalfW
 */ 

#include <avr/io.h>

#include <util/delay.h>
#include "C:\Users\RalfW\OneDrive - Emschertal-Berufskolleg der Stadt Herne\Atmel\myLib\Schule\lcd-routines.h"

//###############################################################
#define DHT11DDR	DDRA
#define DHT11PORT	PORTA
#define DHT11PIN	PINA

#define DHT11PWR	PINB5
#define DHT11DATA	PINB6
#define HIGHTEMPERATURE 2



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
#define DHT_ERROR				4
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



uint8_t detectResponse()
{

	_delay_us(14/2); // Zeitliche Hälfte des Impulses
	
	if (!recvPinHigh()) return DHT_NO_RESPONSE;
	_delay_us(14/2+83/2); // Zeitliche Hälfte des Impulses		
	
	if (recvPinHigh()) 	return DHT_NO_RESPONSE;			
	_delay_us(83/2+88/2);     // Zeitliche Hälfte des Impulses
	
	if (!recvPinHigh()) return DHT_NO_RESPONSE;
	//_delay_us(88/2);

return DHT_RESPONSE_START;
}


uint8_t detectBit()
{
	uint8_t time =0;
	//auf low warten
	while(recvPinHigh());
	//auf high warten
	while(!recvPinHigh());

	while(recvPinHigh())
		{
		time++;
		_delay_us(1);	
		}
		return time>30?1:0;
}


void showValues(uint8_t * werte)
{
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
				
		debugvalue(werte[HIGHTEMPERATURE]);	
		//while(1);	
				
		_delay_ms(2000);
		debugvalue(0);
		_delay_ms(2000);	
}

uint8_t getDHT11Values(uint8_t * werte)
{
//Löschen des Datenfeldes
for (int i=0;i<5;i++)
		werte[i]=0;	
	
startDHT11();
	

		if(detectResponse()==DHT_RESPONSE_START)
		{
		DHT11ResponseToggle ();

		for (uint8_t i=0;i<40;i++)
			{
			uint8_t bitwert=detectBit();
				
				DHT11DebugToggle(bitwert);
				
				if (bitwert==1)
					werte[i/8] |=0b10000000 >>(i%8);
			}
			
		return DHT_RESPONSE_START;	
		
		}
		
return DHT_ERROR;		
}

int main(void)
{
initDHT11();
initDHT11Debug();
lcd_init();
lcd_clear();
lcd_string("Start");
_delay_ms(1000);

uint8_t werte[5];

    /* Replace with your application code */
    while (1) 
    {
	if (getDHT11Values(werte)==DHT_RESPONSE_START)
	{
	lcd_clear();	
	showValues(werte);
	}

	_delay_ms(50);	


    }
}

