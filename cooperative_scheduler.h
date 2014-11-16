//author: Dariusz Michalski, Agnieszka Słoma, Mateusz Mikita

#ifndef COOPERATIVE_SCHEDULER_H_ 
#define COOPERATIVE_SCHEDULER_H_ 
 
typedef void (*func_ptr) (void *); 
typedef struct 
{ 
    func_ptr foo; 
    uint32_t timeout; 
    uint32_t timetogo; 
    uint32_t ready; 
    void* params; 
} TASK; 
 
void schedule(); 
void execute(); 
void AddTask(priority, period, func_ptr, void * params); 
 
#endif /* COOPERATIVE_SCHEDULER_H_ */
