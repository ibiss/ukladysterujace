//author: Dariusz Michalski, Agnieszka Słoma, Mateusz Mikita

#define F_CPU 16000000UL  // 16 MHz 
#include <avr/io.h> 
#include <avr/interrupt.h> 
#include <cooperative_scheduler.h> 
#define MAX_NUMBER_OF_TASK 2 

TASK TASKS[MAX_NUMBER_OF_TASK]; 

void schedule() 
{ 
    for(int i=0; i<MAX_NUMBER_OF_TASK; i++) 
    { 
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
                TASKS[j].ready--; 
                sei(); 
                TASKS[i].foo(); 
                TASKS[i].timetogo = TASKS.timeout; 
                break; 
            } 
        } 
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
