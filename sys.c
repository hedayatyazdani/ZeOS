/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>
#include <utils.h>
#include <io.h>
#include <mm.h>
#include <mm_address.h>
#include <sched.h>
#include <interrupt.h>
#include <entry.h>


#define LECTURA 0
#define ESCRIPTURA 1
#define N 100
#define EBADF 9
#define EFAULT 14
#define EINVAL 22
#define ENOMEM 12
#define EAGAIN 11

char dest[N];
extern struct list_head freequeue;
extern struct list_head readyqueue;
extern struct task_struct *init_task;
extern int zeos_ticks;
int pidTotal = 2;

int check_fd(int fd, int permissions) {
    if (fd!=1) return -9; /*EBADF*/
    if (permissions!=ESCRIPTURA) return -13; /*EACCES*/
    return 0;
}


int sys_ni_syscall() /*ENOSYS*/
	{ return -38; }

int sys_getpid() 
{ 

	return current()->PID; 
}


int sys_fork() {

	int PID;
        struct task_struct *task_dirHijo;
        if (!list_empty(&freequeue)) {
                struct list_head *ptr = list_first(&freequeue);
                task_dirHijo = list_head_to_task_struct(ptr);
                list_del(ptr);
        } else {
		 return -EAGAIN;
	}
        	
	int pagina; int index;
    	int paginesFisiques[NUM_PAG_DATA];
	
	for (pagina = 0; pagina < NUM_PAG_DATA; pagina++) {
                paginesFisiques[pagina] = alloc_frame();
                if (paginesFisiques[pagina] == -1) {               
                	for (index = 0; index < pagina-1; index++) {
	                	free_frame(paginesFisiques[index]);	
                	}
			list_add_tail(&(task_dirHijo->list), &freequeue);
                	return -ENOMEM;
                }  
    	}

	page_table_entry *entradaTablaHijo = get_PT(task_dirHijo);
	page_table_entry *dirHijo = task_dirHijo->dir_pages_baseAddr;
	page_table_entry *PT_padre = get_PT(current());

 	copy_data(current(), task_dirHijo, sizeof(long)*KERNEL_STACK_SIZE);

	copy_data(&PT_padre[PAG_LOG_INIT_CODE_P0],  &entradaTablaHijo[PAG_LOG_INIT_CODE_P0], NUM_PAG_CODE*sizeof(page_table_entry));

	for (pagina = 0; pagina < NUM_PAG_DATA; pagina++) {
                
		set_ss_pag(PT_padre,  PAG_LOG_INIT_DATA_P0+NUM_PAG_DATA+pagina, paginesFisiques[pagina]);	

		
		copy_data((void *)((PAG_LOG_INIT_DATA_P0+pagina)<<12),(void *) ((PAG_LOG_INIT_DATA_P0+NUM_PAG_DATA+pagina)<<12), PAGE_SIZE);	                                
               
        	del_ss_pag(PT_padre,  PAG_LOG_INIT_DATA_P0+NUM_PAG_DATA+pagina);       	
        }	
	for (index = 0; index < NUM_PAG_DATA; index++) { 		
		set_ss_pag(entradaTablaHijo, PAG_LOG_INIT_DATA_P0+index, paginesFisiques[index]);	
	}
	set_cr3(current()->dir_pages_baseAddr);

        task_dirHijo->PID = pidTotal;
        PID = pidTotal;
        ++pidTotal;

        task_dirHijo->dir_pages_baseAddr = dirHijo;

        union task_union *aux_union;
        aux_union = (union task_union *)task_dirHijo;

        aux_union->stack[KERNEL_STACK_SIZE-18] = 0;
        aux_union->stack[KERNEL_STACK_SIZE-17] = ret_from_fork;
        task_dirHijo->kernel_esp = &aux_union->stack[KERNEL_STACK_SIZE-18];
 
	
        list_add_tail(&(task_dirHijo->list), &readyqueue);
	//LINE X PROVAR EXIT MENTRE NO HI HA SCHEDULERtask_switch(aux_union,0);

     
    	return PID;
}

void sys_exit() {

	//Considerar init_proces can not be destroyed!!!
	printk("comenco sys_EXIT");
	struct task_struct *current_task;
        current_task = current();
	if (current_task->PID != 1) {

		//allibero task_struc, cal comprovar k esta ven alliberat
		list_add_tail(&(current_task->list), &freequeue);
	
	
		//allibero TP (part datos) i frames(datos)
		free_user_pages(current_task);

		//USAR EL SCHEDULER X DECIDIR SEGUENT PROCES 
		/*
		task=next_selection();  
  		update_state(task);
		*/
		
		//LINEA X PROVAR EXIT MENTRE NO HI HA SCHEDULERtask_switch(init_task,0);
	}
}


int sys_gettime() {		
	return zeos_ticks;
}


int sys_write(int fd, char * buffer, int size) {
    int result; int escrits; int error;
	result = check_fd(fd, ESCRIPTURA);

	//error bad file descriptor          
	if (result != 0)    return result;

	//error buffer
    if (buffer == NULL)	return -1*EFAULT;         
                    
    //error tamaño invalido
    if (size <= 0) 		return -1*EINVAL;
               
    escrits = 0;
    error = 0;
    
    while (error == 0 && size > 0) { // falta comprovar el tam size !!!!!       
        if (size > N) {        

            if (copy_from_user(buffer, dest, N) == -1)  // copy_from_user error
    			return -1*EFAULT;
        
            error = sys_write_console(dest, N);
            size -= error;
            escrits += error;
        } else {
	        if (copy_from_user(buffer, dest, size) == -1)  // copy_from_user error
    			return -1*EFAULT;
            error = sys_write_console(dest, size);
            size -= error;
            escrits += error;
        }
    }
    
    if (error == 0) return error;
    else            return escrits;
}
