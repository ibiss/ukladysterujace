//author: Dariusz Michalski, Agnieszka S?oma, Mateusz Mikita
#define F_CPU 16000000UL  // 16 MHz
#include <avr/io.h>
#include <avr/interrupt.h>
#include "cooperative_scheduler.h"

TASK TASKS[MAX_NUMBER_OF_TASK];

void schedule()//Funkcja wywolywana przez przerwanie, dekrementuje timetogo taska i ustawia jego gotowosc
{
	for(int i=0; i<MAX_NUMBER_OF_TASK; i++)
	{
		if(TASKS[i].foo == 0)
			continue;
		TASKS[i].timetogo--;
		if(TASKS[i].timetogo==0)
		{
			TASKS[i].ready++;
		}
	}
}

void execute()//Sprawdza tablice taskow czy sa gotowe, jesli tak wykonuje taska, zmienia jego gotowosc, resetuje timetogo, blokuje i odblokowuje przerwania
{
	while(1)
	{
		cli();
		for(int i=0; i<MAX_NUMBER_OF_TASK; i++)
		{
			if(TASKS[i].ready!=0){
				TASKS[i].ready--;
				sei();//poniewaz znalezlismy odpowiedniego taska do uruchomienia odblokowujemy przerwania
				TASKS[i].foo(TASKS[i].params);
				TASKS[i].timetogo = TASKS[i].timeout;
				break;
			}
		}
		sei(); //jeśli nie znalezlismy taska odblokowujemy przerwania 
	}
}

void AddTask(uint32_t priority, uint32_t period, func_ptr foo, void * params)
{
	TASKS[priority].foo = foo;
	TASKS[priority].ready = 0;
	TASKS[priority].timeout = period;
	TASKS[priority].timetogo = period;
	TASKS[priority].params = params;
}

void timer0_init(void)//Ustawienie przerwan na 1 ms
{
	TCCR0 |= (1 << WGM01);
	TIMSK |= (1 << OCIE0);
	OCR0  = 250;
	sei();
	TCCR0 |= (1 << CS00) | (1 << CS01);

}

ISR(TIMER0_COMP_vect)
{
	schedule();
}
