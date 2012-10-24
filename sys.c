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
#define ENOMEM
#define EAGAIN

char dest[N];
extern struct list_head freequeue;
extern int zeos_ticks;

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
Adicionalment, no poden ser mapejades directament pk  les pagines logiques del pare son les mateixes.
Primer s' han de mapejar en noves entrades de la TP del pare (temporalemnt), despres fer la copia i despres eliminar-les de la TP del pare.

	A) Usar entrades temporals en la TP del pare (set_ss_pag) (del_ss_pag)
	B) Copiar pagines data+stack 
	C) Alliberar entrades temporals de la TP del pare.
		Flush de la TLB del pare. (si recodo bé, set_cr3 del pare amb la del pare).


/
	
    


   // 2
        //a
        
        struct task_struct *task_child;
        if (!list_empty(&freequeue)) {
                struct list_head *ptr = list_first(&freequeue);
                task_child = list_head_to_task_struct(ptr);
                list_del(ptr);
        } else return -EAGAIN;
        


	//3
	int pagina; int index;
    int paginesFisiques[NUM_PAG_DATA];
	
	for (pagina = 0; pagina < NUM_PAG_DATA; pagina++) {
                paginesFisiques[pagina] = alloc_frames();
                if (paginesFisiques[pagina] == -1) {               
                	for (index = 0; index < pagina-1; index++) {
	                	free_frame(paginesFisiques[index]);	
                	}
                	return -ENOMEM;
                }  
    }

	//6
	page_table_entry *entradaTablaHijo = get_PT(task_child);

	//4  Copiar el PCB del pare al fill
 	copy_data(current(), task_child, 4096);

	//A
	copy_data(current()->dir_pages_baseAddr, entradaTablaHijo, NUM_PAG_CODE*sizeof(page_table_entry));

	//B

	for (index = 0; index < NUM_PAG_DATA; index++) { // NUM_PAG_DATA == pagsFisiqs.size()
		if (paginesFisiques[index] != -1)
			set_ss_pag(entradaTablaHijo, NUM_PAG_CODE+index, paginesFisiques[index]);	
    }



	//7
	
	
	for (pagina = 0; pagina < NUM_PAG_DATA; pagina++) {
                //A
		set_ss_pag(current()->dir_pages_baseAddr,  NUM_PAG_CODE+NUM_PAG_DATA+pagina, paginesFisiques[pagina]);	
                
		//B No se com copiar les dades de la MF
		
			copy_data();	                                
                //C
//C
        	del_ss_pag(current()->page_table_entry,  NUM_PAG_CODE+NUM_PAG_DATA+pagina);
            set_cr3(current()->page_table_entry);
        }

        //e
        task_child.PID = pidTotal;
        PID = pidTotal;
        ++pidTotal;

        // f
        task_child.page_table_entry = entradaTablaHijo;
        //el list s' ha d' actualiutzar pk sigui diferent del list del parE?

        //g
        union task_union *aux_union;
        aux_union = (union task_union *)task_child;
        //potser hauria d' agafar el valor del esp i preparar la pila a partir de la direccio del esp
        aux_union->stack[1022] = 0;
        aux_union->stack[1023] = ret_from_fork; //no est aimplementada no se que hi tinc de posar
        task_child->kernel_esp = &aux_union->stack[1022];
        //list

        //h
        //afegir el fill a al ready queue
        list_add(task_child.list, &readyqueue);

     //i
    return PID;
}

*/

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
