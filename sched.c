/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>
#include <list.h>

LIST_HEAD(freequeue);
LIST_HEAD(readyqueue);

struct task_union *idle_task;

union task_union task[NR_TASKS]
  __attribute__((__section__(".data.task")));



struct task_struct *list_head_to_task_struct(struct list_head *l)
{
  return list_entry( l, struct task_struct, list);
}


extern struct list_head blocked;


/* get_DIR - Returns the Page Directory address for task 't' */
page_table_entry * get_DIR (struct task_struct *t) 
{
	return t->dir_pages_baseAddr;
}

/* get_PT - Returns the Page Table address for task 't' */
page_table_entry * get_PT (struct task_struct *t) 
{
	return (page_table_entry *)(((unsigned int)(t->dir_pages_baseAddr->bits.pbase_addr))<<12);
}

void cpu_idle(void)
{
	__asm__ __volatile__("sti": : :"memory");

	while(1)
	{
	;
	}
}

void init_idle (void)
{
	int pid = 0;
	struct task_union *ptr = list_first(&freequeue);
	idle_task = list_head_to_task_struct(ptr);
	list_del(&freequeue);	
	
	idle_task->pid = pid;
	list_add(idle_task->list,&readyqueue);
	
	
	// push &cpu_idle stack
	void *ptr_cpuIdle = &cpu_idle();
	void *ptr_stack = ;////////

	idle_task->esp0 = setPush_cpuIdle(ptr_cpuIdle);
	
	//page_table_entry * dir_pages_baseAddr;	
	
}

void setPush_cpuIdle(void *ptr_cpuIdle) {
	DWord esp;
	__asm__ __volatile__(

		"pushl %1 \n" ; push de &cpu_idle a *ptr_stack
		"pushl $0 \n"
		"movl %%esp,%0 \n"
		 
        : "=g"(esp)
        :"g" (ptr_cpuIdle), "g"(ptr_stack) );			
}

void init_task1(void)
{
}


void init_sched(){
	int i = 0;	
	list_add(&task[0].task.list, &freequeue);
	for (i = 1; i < NR_TASKS; i++) {
		list_add(&task[i].task.list,&freequeue);
	}
}

struct task_struct* current()
{
  int ret_value;
  
  __asm__ __volatile__(
  	"movl %%esp, %0"
	: "=g" (ret_value)
  );
  return (struct task_struct*)(ret_value&0xfffff000);
}

void switch_task() {

	
}

