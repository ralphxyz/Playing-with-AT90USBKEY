#include "config.h"
#include "scheduler.h"


uint8_t ntasks = 0;
int (*tasks[MAX_TASKS])(uint32_t seconds, uint32_t useconds);

int scheduler_add_task(int (*task)(uint32_t seconds, uint32_t useconds)){
  tasks[ntasks] = task;
  ntasks++;
  return 0;
}


int scheduler(uint32_t seconds, uint32_t useconds){
  int ret=0;
  int temp;
  uint8_t i;

  // go through the tasks
  for(i = 0; i <ntasks; i++){
    temp = tasks[i](seconds, useconds);
    ret = temp ? temp:ret;
  }

  return ret;
}
