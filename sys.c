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
#define ENOMEM 12
#define EAGAIN 11

char dest[N];
extern struct list_head freequeue;
extern struct list_head readyqueue;
extern int zeos_ticks;
int pidTotal = 2;

int check_fd(int fd, int permissions) {
    if (fd!=1) return -9; /*EBADF*/
    if (permissions!=ESCRIPTURA) return -13; /*EACCES*/
    return 0;
}

void ret_from_fork() {

     __asm__ __volatile__(

      "popl %ebx \n"
      "popl %ecx\n"
      "popl %edx\n"
      "popl %esi\n"
      "popl %edi\n"
      "popl %ebp\n"
      "popl %eax\n"
      "popl %ds\n"
      "popl %es\n"
      "popl %fs\n"
      "popl %gs\n"
      "movl $0, %eax \n"
      "iret"
        );
	
}

int sys_ni_syscall() /*ENOSYS*/
	{ return -38; }

int sys_getpid() 
{ 

	return current()->PID; 
}


int sys_fork() {


	int PID = -1;

/*
	1) Wrapper 
	2) aoncseguir task_struck pel fill
	3) buscar Mem fisica x dades del fill
	4) task_union PARE ----> task_union fill (copy_data)
	5) Determinar si es necesari modificar TP pare x accedir a dades del sistema del fill
	
	6) Crear nou espai d' usuari --> accedir a TP fill a traces del task_struct (get_PT)
		A) Entrades TP fill per codi, data i user code poden ser copiades de la  				TPpare
		B) data+struck -> apunta a nou espai alocatat

7) Copia les pagines de dades+pila del pare al fill.

Traducio del text:
Les pagines del fill no poden ser accedides directament pk no estan mapejades en la 	TP del pare.
Adicionalment, no poden ubuntu codigo errorser mapejades directament pk  les pagines logiques del pare son les mateixes.
Primer s' han de mapejar en noves entrades de la TP del pare (temporalemnt), despres fer la copia i despres eliminar-les de la TP del pare.

	A) Usar entrades temporals en la TP del pare (set_ss_pag) (del_ss_pag)
	B) Copiar pagines data+stack pidTotal
	C) Alliberar entrades temporals de la TP del pare.
		Flush de la TLB del pare. (si recodo bé, set_cr3 del pare amb la del pare).


/dirHijo
	
    */


   // 2ubuntu codigo error
        //a
        
	
        struct task_struct *task_dirHijo;
        if (!list_empty(&freequeue)) {
                struct list_head *ptr = list_first(&freequeue);
                task_dirHijo = list_head_to_task_struct(ptr);
                list_del(ptr);
        } else {
		 return -EAGAIN;
	}
        

	//3
	int pagina; int index;
    int paginesFisiques[NUM_PAG_DATA];
	
	for (pagina = 0; pagina < NUM_PAG_DATA; pagina++) {
                paginesFisiques[pagina] = alloc_frame();
                if (paginesFisiques[pagina] == -1) {               
                	for (index = 0; index < pagina-1; index++) {
	                	free_frame(paginesFisiques[index]);	
                	}
			list_add(&(task_dirHijo->list), &freequeue);
                	return -ENOMEM;
                }  
    	}


	//6 hgigjuh
	page_table_entry *entradaTablaHijo = get_PT(task_dirHijo);
	page_table_entry *dirHijo = task_dirHijo->dir_pages_baseAddr;
	page_table_entry *PT_padre = get_PT(current());

	//4  Copiar el PCB del pare al fill
 	copy_data(current(), task_dirHijo, sizeof(long)*KERNEL_STACK_SIZE);

	//A
	copy_data(&PT_padre[PAG_LOG_INIT_CODE_P0],  &entradaTablaHijo[PAG_LOG_INIT_CODE_P0], NUM_PAG_CODE*sizeof(page_table_entry));

	//B



	for (pagina = 0; pagina < NUM_PAG_DATA; pagina++) {
                
set_ss_pag(PT_padre,  PAG_LOG_INIT_DATA_P0+NUM_PAG_DATA+pagina, paginesFisiques[pagina]);	

		
		copy_data((void *)((PAG_LOG_INIT_DATA_P0+pagina)<<12),(void *) ((PAG_LOG_INIT_DATA_P0+NUM_PAG_DATA+pagina)<<12), PAGE_SIZE);	                                
                //C
		
        	del_ss_pag(PT_padre,  PAG_LOG_INIT_DATA_P0+NUM_PAG_DATA+pagina);       	
        }	
for (index = 0; index < NUM_PAG_DATA; index++) { 		
set_ss_pag(entradaTablaHijo, PAG_LOG_INIT_DATA_P0+index, paginesFisiques[index]);	
	}
	set_cr3(current()->dir_pages_baseAddr);

        //e
        task_dirHijo->PID = pidTotal;
        PID = pidTotal;
        ++pidTotal;

        // f
        task_dirHijo->dir_pages_baseAddr = dirHijo;
        //el list s' ha d' actualiutzar pk sigui diferent del list del parE?

        //g
        union task_union *aux_union;
        aux_union = (union task_union *)task_dirHijo;

        //potser hauria d' agafar el valor del esp i preparar la pila a partir de la direccio del esp
        aux_union->stack[KERNEL_STACK_SIZE-18] = 0;
        aux_union->stack[KERNEL_STACK_SIZE-17] = ret_from_fork;
        task_dirHijo->kernel_esp = &aux_union->stack[KERNEL_STACK_SIZE-18];
        //list

        //h
        //afegir el fill al ready queue
	
        list_add(&(task_dirHijo->list), &readyqueue);
	task_switch(task_dirHijo, 0);

     //i
    return PID;
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
