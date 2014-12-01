//Dariusz Michalski, Agnieszka Sloma, Mateusz Mikita


#define F_CPU 16000000UL // 16 MHz
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "cooperative_scheduler.h"
#define NULL ((char *)0)
uint8_t CYFRA[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};

int seconds = 0;
int miliseconds = 0;

ISR(TIMER0_COMP_vect)
{
	//dzieki takiemu rozwiazaniu nasza funkcja showDigit odswieza sie co milisekunde. Trzeba 
	miliseconds++;
	if(miliseconds >= 1000)
	{
		seconds++;
		miliseconds = 0;
	}
	schedule();
}
void timer0_init(void)//Ustawienie przerwan na 1 ms
{
	TCCR0 |= (1 << WGM01);
	TIMSK |= (1 << OCIE0);
	OCR0  = 250;
	sei();
	TCCR0 |= (1 << CS00) | (1 << CS01);
	
}
void showDigit(void * params)
{	
	//funkcja moze pwinna sie wlaczac co 25 milisekund moze albo jeszcze dluzszy odstep czasowy bo 1000milisekund to duzo
	//if(miliseconds % 25) return;
	uint8_t SEGMENT[] = {0, 0, 0 , 0};
	uint8_t num = seconds;
	uint8 i;
	//w petlach while dzieki operacji modulo i dzieleniu mozemy odczytac kazda cyfre w liczbie
	if(seconds <= 999)// do 999 sekund wyswietlamy kropke i setki milisekund
	{
		SEGMENT[3] = CYFRA[miliseconds / 100]
		i = 2;
		while(num > 9)
		{
			SEGMENT[i] = CYFRA[num % 10]; 
			i--;
			num /= 10;
		}
		SEGMENT[2] = 0xFF ^ (0x80 ^ SEGMENT[2]);// XORy dodajace kropke do liczby bedacej na 3 pozycji.
	}
	else // po 1000 sekundach wyswietlamy same sekundy
	{
		i = 3;
		while(num > 9)
		{
			SEGMENT[i] = CYFRA[num % 10]; 
			i--;
			num /= 10;
		}

	}
	//Nie wiem jaka kolejnosc powinna być oczywiscie to tylko szkielet funkcjionalnosci, pamietac trzeba o zerowaniu aby zapobiedz efektu smużenia
	//gasimy segment i zaswiecamy nastepny
	PORTA = SEGMENT[0];
	PORTB = 0x01;
	PORTB = 0x00;
	PORTA = SEGMENT[1];
	PORTB = 0x02;
	PORTB = 0x00;
	PORTA = SEGMENT[2];
	PORTB = 0x04;
	PORTB = 0x00;
	PORTA = SEGMENT[3];
	PORTB = 0x08;
	PORTB = 0x00;
	
}
int main(int argc, char * argv[])
{
	DDRB |= 0xFF;
	DDRA |= 0xFF;
	timer0_init();
	AddTask(0,1, showDigit, CYFRA);
	execute();
}
