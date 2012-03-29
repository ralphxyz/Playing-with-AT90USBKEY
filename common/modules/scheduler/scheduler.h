#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#define MAX_TASKS (10)

int scheduler_add_task(int (*task)(uint32_t seconds, uint32_t useconds));
// scheduler does not fail, but tasks inside might
int scheduler(uint32_t seconds, uint32_t useconds);


#endif /* _SCHEUDLER_H_ */
