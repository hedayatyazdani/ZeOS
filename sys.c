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

#define LECTURA 0
#define ESCRIPTURA 1
#define N 100
#define EBADF 9
#define EFAULT 14
#define EINVAL 22

char dest[N];
extern struct list_head freequeue;

int check_fd(int fd, int permissions) {
    if (fd!=1) return -9; /*EBADF*/
    if (permissions!=ESCRIPTURA) return -13; /*EACCES*/
    return 0;
}


int sys_ni_syscall() /*ENOSYS*/
	{ return -38; }

int sys_getpid() 
	{ return current()->PID; }


/*
int sys_fork() {
    int PID=-1;
    
   //2
	//a FALTA COMPROVAR K KEDEN TASK_STRUCTS LLIURES, I ACCEDIR CORRECTAMENT A FREQUEUE
	
	struct task_struct *new_task;
	if (!list_empty(&freequeue)) {
		struct list_head *ptr = list_first(&freequeue);
		new_task = list_head_to_task_struct(ptr);
		list_del(ptr);
	} else return ERROR_NO_KEDEN_TASKSTRUCT_LLIURES;

	//b 
	int numeroPaginaFisica = alloc_frames();
	if (numeroPaginaFisica == -1) return ERROR_NO_PAGINES_LLIURES;

	//c
	void copy_data(void *start, void *dest, int size)
	copy_data(current()->dir_pages_baseAddr???);
	//a system.c es fa servir, pero els parametres son varaibles definides, cal mirar com accedir a la pagina fisica del PARE I COPIARLA A LA PAGINA FISICA numeroPAGINAFISICA,
	
	//CALDRÀ modificar la taula de pagines del pare pk hi pugui accedir

	
	

    // creates the child process  
    return PID;
}
*/
int sys_gettime() {	
	int ticks = get_zeos_ticks();
	return ticks;
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
