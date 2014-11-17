//author: Dariusz Michalski, Agnieszka S?oma, Mateusz Mikita
#define F_CPU 16000000UL  // 16 MHz
#include <avr/io.h>
#include <avr/interrupt.h>
#include "cooperative_scheduler.h"

TASK TASKS[MAX_NUMBER_OF_TASK];

void schedule()
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

void execute()
{
	while(1)
	{
		cli();
		for(int i=0; i<MAX_NUMBER_OF_TASK; i++)
		{
			if(TASKS[i].ready!=0){
				TASKS[i].ready--;
				sei();
				TASKS[i].foo(TASKS[i].params);
				TASKS[i].timetogo = TASKS[i].timeout;
				break;
			}
		}
		sei(); //???
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

void timer0_init(void)
{
	sei();
	TCCR0 |= (1 << WGM01);
	TIMSK |= (1<<OCIE0);
	OCR0  = 250;
	TCCR0 |= (1 << CS00) | (1 << CS01);
}

ISR(TIMER0_COMP_vect)
{
	schedule();
}
int i = 0;
int j = 0;
int c = 0;
int main(void)
{
	timer0_init();
	AddTask(0,50, keyboardTask, NULL);
	AddTask(1, 20, serialReciveTask, NULL);
	AddTask(7, 500, watchDogTask, NULL);
	execute();
}

void keyboardTask(void * params){
	i++;
}
void serialReciveTask(void * params){
	j++;
}
void watchDogTask(void * params){
	c++;
}
