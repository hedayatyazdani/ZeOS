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


int check_fd(int fd, int permissions)
{
  if (fd!=1) return -9; /*EBADF*/
  if (permissions!=ESCRIPTURA) return -13; /*EACCES*/
  return 0;
}

int sys_ni_syscall()
{
	return -38; /*ENOSYS*/
}

int sys_getpid()
{
	return current()->PID;
}

int sys_fork()
{
  int PID=-1;

  // creates the child process
  
  return PID;
}

int sys_gettime() {	
	int ticks = get_zeos_ticks();
	return ticks;
}


int sys_write(int fd, char * buffer, int size) {
        int result;
	result = check_fd(fd, ESCRIPTURA);
	if (result != 0) {
            //error bad file descriptor
            return result;
        }
        if ( buffer == NULL ) {
            //error buffer
            return -1*EFAULT;
        }
        if (size <= 0) {
            //error tamaño invalido
            return -1*EINVAL;
        }

        int escrits = 0;
        int error = 0;
        while (error == 0 && size > 0) {
                if (copy_from_user(buffer, dest, size) == -1) {
                        //error al hacer el copy_from_user
                        return -1*EFAULT;
                }
                if (size > N){
                        error = sys_write_console(dest, N);
                        size -= error;
                        escrits += error;
                }else {
                        error = sys_write_console(dest, size);
                        size -= error;
                        escrits += error;
                }
        }
        if (error == 0)return error;
        else return escrits;
}
