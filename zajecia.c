//Dariusz Michalski, Agnieszka Sloma, Mateusz Mikita
#define F_CPU 16000000UL // 16 MHz
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>
#include "cooperative_scheduler.h"
//#define NULL ((char *)0)
uint8_t CYFRA[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};
int seconds = 0;
int miliseconds = 0;

ISR(TIMER0_COMP_vect)
{
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
	OCR0 = 250;
	sei();
	TCCR0 |= (1 << CS00) | (1 << CS01);
}
void WyswietlSekwencje(uint8_t* SEKWENCJA)
{
	//Pamietac o tym, ze ostatnio wyswietlalo nam przeciwienstwa CYFR wiec moze ~SEKWENCJA zadziala!!!
	PORTB = 0x00;
	uint8_t numer_wyswietlacza = 0x00;
	for(uint8_t i = 0; i < 4; i++)
	{
		numer_wyswietlacza = numer_wyswietlacza << 1;//przesuwamy się na następny segment 0x01, 0x02, 0x04, 0x08
		PORTA = *(SEKWENCJA++);
		PORTB = numer_wyswietlacza;
		_delay_ms(10);//do testow klawiatury zwiekszyc, chyba ze przytrzymie sie klawisz;
		PORTB = 0x00;//wylaczamy segmenty
	}
}
void ZamienCzasNaSekwencje(void * params)
{
	uint8_t SEKWENCJA[] = {0x00, 0x00, 0x00, 0x00, 0x00}, num = seconds, i = 0, *segment = SEKWENCJA;
	segment[0] = CYFRA[miliseconds / 100];//setki milisekund
	while(num > 9)
	{
		segment[++i] = CYFRA[num % 10];//za pomoca operacji modulo mozemy odczytac cyfre
		num /= 10;
	}
	if(seconds < 999)
	{
		segment[1] = 0xFF ^ (0x80 ^ segment[1]);// XORy dodajace kropke do liczby bedacej na 3 pozycji.
		WyswietlSekwencje(segment);
	}
	else
	WyswietlSekwencje(++segment);//umiescilismy milisekundy na poczatku tablicy dzieki czemu mozemy latwo pominac milisekundy i kropke
}
uint8_t * KlawiszNaSekwencje(uint8_t klawisz, uint8_t* segment)
{
	//tablica Liczb zalezna od kolejnosci podpiecia kabli na plytce
	
	uint8_t Liczby[] = {0xFF, 0x77, 0xB7, 0xD7, 0xE7, 0x7B, 0xBB, 0xDB, 0xEB, 0x7D, 0xBD, 0xDD, 0xED, 0x7E, 0xBE, 0xDE, 0xEE};
	uint8_t i = 0;
	for (;i <17;i++)
	if(Liczby[i] == klawisz)
	break;
	uint8_t num =  i;
	i = 0;
	while(num > 9)
	{
		segment[i++] = CYFRA[num % 10];
		num /= 10;
	}
	return segment;
}
void ObslugaKlawiatury(void * params)
{
	DDRB |= 0xFF;
	PORTA = 0x00;
	DDRA = 0x0F;
	PORTA = 0xF0;//wiersz
	for (int i=0;i<10;i++);
	uint8_t K = PINA;
	PORTA = 0x00;
	DDRA = 0xF0;
	PORTA = 0x0F;//kolumna lub na odwrot :D
	for (int i=0;i<10;i++);
	K |= PINA;// wykorzystujac alternatywe laczymy odczyt z wierszy i kolumn
	uint8_t SEKWENCJA[] = {0x00, 0x00, 0x00, 0x00};
	WyswietlSekwencje( KlawiszNaSekwencje( K, SEKWENCJA ) );
}

int main(int argc, char * argv[])
{
	timer0_init();
	AddTask(0,1, ZamienCzasNaSekwencje, NULL);
	//AddTask(0, 1, ObslugaKlawiatury, NULL);
	execute();
}
