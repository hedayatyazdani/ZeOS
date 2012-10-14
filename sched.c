/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>
#include <list.h>

LIST_HEAD(freequeue);
LIST_HEAD(readyqueue);

struct task_struct *idle_task;
struct task_struct *init_task;
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
	struct list_head *ptr = list_first(&freequeue);
	idle_task = list_head_to_task_struct(ptr);
	list_del(ptr);	

	idle_task->PID = 0;
	
	union task_union *aux_union;
    aux_union = (union task_union *)idle_task;
	aux_union->stack[1022] = 0;
	aux_union->stack[1023] = cpu_idle;
	
	idle_task->kernel_esp = &aux_union->stack[1022];	
}

void init_task1(void)
{

	struct list_head *ptr = list_first(&freequeue);
	init_task = list_head_to_task_struct(ptr);
	list_del(ptr);	
	idle_task->PID = 1;		
	
	set_user_pages(init_task);
	set_cr3(init_task->dir_pages_baseAddr);			
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

void task_switch(union task_union *new) {

 	struct task_struct *current_task;
 	current_task = current();


	tss.esp0 = (DWord) &new->stack[1024];
	set_cr3(new.task.dir_pages_baseAddr);	
	
 	current_task.kernel_esp = direccio_ebp();
 	
 	struct task_struct aux = new->task;
 	int new_esp = aux->kernel_esp;
 	restore_new_task(new_esp);
 	
}

int direccio_ebp() {
	 int ret_value;
    __asm__ __volatile__(
		"movl %%ebp,%0\n"     	:"=g"(ret_value)
    );
    return ret_value;
}

void restore_new_task(int new_esp) {

    __asm__ __volatile__(
		"movl %0, %%esp\n"
		"popl %%ebp \n"
		"ret"     	:
     	:"g" (new_esp)
    );
}


