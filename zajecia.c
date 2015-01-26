//Dariusz Michalski, Agnieszka Sloma, Mateusz Mikita
#define F_CPU 16000000UL // 16 MHz
#include "HD44780.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>
#include "cooperative_scheduler.h"
//#define NULL ((char *)0)
uint8_t CYFRA[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};
uint32_t seconds = 0;
uint8_t K = 1;
ISR(TIMER0_COMP_vect)
{
	change_segment();
	schedule();
}
uint8_t liczba(uint8_t segment)
{
	switch (segment)
	{
		case 0:
			return seconds % 10;
			break;
		case 1:
			return (seconds / 10) % 10;
			break;
		case 2:
			return (seconds / 100) % 10;
			break;
		case 3:
			return (seconds / 1000) % 10;
			break;
	}
}
uint8_t liczba2(uint8_t segment)
{
	switch (segment)
	{
		case 0:
			return K % 10;
			break;
		case 1:
			return (K / 10) % 10;
			break;
		case 2:
			return (K / 100) % 10;
			break;
		case 3:
			return (K / 1000) % 10;
			break;
	}
}
void change_segment()
{
	static uint8_t segment = 0;
	PORTA = CYFRA[liczba2(segment)];//liczby
	PORTB = PORTB & 0xF0 | (~(1 << segment)) & 0x0f;
	segment = (segment +1) % 4;
}
void timer0_init(void)//Ustawienie przerwan na 1 ms
{
	TCCR0 |= (1 << WGM01);
	TIMSK |= (1 << OCIE0);
	OCR0 = 250;
	sei();
	TCCR0 |= (1 << CS00) | (1 << CS01);
}
/*
void WyswietlSekwencje(uint8_t* SEKWENCJA)
{
//Pamietac o tym, ze ostatnio wyswietlalo nam przeciwienstwa CYFR wiec moze ~SEKWENCJA zadziala!!!
PORTB = ~0x01;
uint8_t numer_wyswietlacza = 0x00;
for(uint8_t i = 0; i < 4; i++)
{
numer_wyswietlacza = numer_wyswietlacza << 1;//przesuwamy się na następny segment 0x01, 0x02, 0x04, 0x08
PORTA = (*(SEKWENCJA++));
PORTB = ~numer_wyswietlacza;
_delay_ms(1000);//do testow klawiatury zwiekszyc, chyba ze przytrzymie sie klawisz;
PORTB = 0xFF;//wylaczamy segmenty
}
}
void ZamienCzasNaSekwencje(void * params)
{
uint8_t static SEKWENCJA[] = {0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t num = seconds, i = 0, *segment = SEKWENCJA;
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
}*/
void KlawiszNaLiczbe(uint8_t klawisz)
{
//tablica Liczb zalezna od kolejnosci podpiecia kabli na plytce
	uint8_t Liczby[] = {0x77, 0x77, 0xB7, 0xD7, 0xE7, 0x7B, 0xBB, 0xDB, 0xEB, 0x7D, 0xBD, 0xDD, 0xED, 0x7E, 0xBE, 0xDE};//, 0xEE};
	uint8_t i = 0;
	for (;i <16;i++)
		if(Liczby[i] == klawisz)
			break;
	K = i;
}
void ObslugaKlawiatury(void * params)
{
	PORTD = 0x00;
	DDRD = 0xF0;
	PORTD = 0x0F;//wiersz
	for (int i=0;i<100;i++);
	uint8_t key = PIND;
	PORTD = 0x00;
	DDRD = 0x0F;
	PORTD = 0xF0;//kolumna lub na odwrot :D
	for (int i=0;i<100;i++);
	key |= PIND;
	DDRC |= 0xFF;
	PORTC = key;
	KlawiszNaLiczbe(key);
	//KlawiszNaLiczbe(K);
}
void AddSecond()
{
	seconds ++;
}
int main(int argc, char * argv[])
{
	DDRA |= 0xFF;
	DDRB |= 0xFF;
	//PORTB = ~0x01;
	//PORTA = 0x00;
	timer0_init();
	LCD_Initalize();
	LCD_GoTo(0,0);
	LCD_WriteText("Dziao!");
	//AddTask(0,1000, AddSecond, NULL);
	AddTask(0, 1, ObslugaKlawiatury, NULL);
	execute();
}
//
//#define UBRR 25
//void USART_Init( void )
//{
///* Set baud rate */
//UBRRH = (unsigned char)(UBRR>>8);
//UBRRL = (unsigned char)UBRR;
///* Enable receiver and transmitter */
//UCSRB = (1<<RXEN)|(1<<TXEN) | (1<<UDRIE);
///* Set frame format: 8data, 2stop bit */
//UCSRC = (1<<URSEL)|(1<<USBS)|(3<<UCSZ0);
//}
//void USART_Transmit( uint8_t data )
//{
///* Wait for empty transmit buffer */
//while ( !( UCSRA & (1<<UDRE)) )
//;
///* Put data into buffer, sends the data */
//UDR = data;
//}
//
//uint8_t USART_Receive( void )
//{
///* Wait for data to be received */
//while ( !(UCSRA & (1<<RXC)) )
//;
///* Get and return received data from buffer */
//return UDR;
//}
//uint8_t len = 10;
//uint8_t znak;
//uint8_t ZDANIE[10];
//uint8_t i = 0;
//ISR(USART_RXC_vect)
//{
//DDRC = 0xFF;
//znak = UDR;
//PORTC = 0xFF;
//ZDANIE[i++] = znak;
//}
//int main(int argc, char * argv[])
//{
//sei();
//DDRC = 0xFF;
//uint8_t znak = 0xFF;
//USART_Init();
//while(1)
//{
////znak = USART_Receive();
////PORTC = znak;
//if(i==8)
//{
//for(int j = 0; j < len;j++)
//USART_Transmit(ZDANIE[j]);
//i = 0;
//}
////USART_Transmit(znak);
//}
//}
//
