/* 
* GccApplication1.c 
* 
* Created: 2014-11-26 16:17:46 
*  Author: student 
*/  
 
 
#define F_CPU 16000000UL // 16 MHz 
#include <avr/io.h> 
#include <avr/interrupt.h> 
#include "cooperative_scheduler.h" 
#define NULL ((char *)0) 
uint8_t CYFRA[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90}; 
uint8_t SEGMENT[] = {0, 0, 0 , 0}; 
int c = 0; 
ISR(TIMER0_COMP_vect) 
{ 
    if(c ==100) 
    { 
        schedule(); 
        c = 0; 
    } 
    else c++;         
} 
void timer0_init(void)//Ustawienie przerwan na 1 ms 
{ 
    TCCR0 |= (1 << WGM01); 
    TIMSK |= (1 << OCIE0); 
    OCR0  = 250; 
    sei(); 
    TCCR0 |= (1 << CS00) | (1 << CS01); 
 
} 
int w = 0; 
 
void showDigit(void * params) 
{ 
    PORTA = CYFRA[w]; 
    w++; 
    if(w == 10) w = 0; 
     
} 
int main(int argc, char * argv[]) 
{ 
    DDRB |= 0xFF; 
    DDRA |= 0xFF; 
    PORTA = CYFRA[9]; 
    timer0_init(); 
    AddTask(0,10, showDigit, CYFRA); 
    execute(); 
}
