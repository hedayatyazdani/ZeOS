/*
 * libc.c 
 */

#include <libc.h>
#include <types.h>

#define EBADF 9 
#define EFAULT 14 
#define EINVAL 22 
#define EACCES 13

int errno;

void print(char * array) {
	write(1, array, strlen(array));
}

int write(int fd, char * buffer, int size) {
    int resultat;
    __asm__ __volatile__(

		"movl %1, %%ebx\n"
		"movl %2, %%ecx\n"
		"movl %3, %%edx\n"
		"movl $0x04, %%eax\n"
		"int  $0x80\n"
		"movl %%eax, %0\n"


        :"=g"(resultat)
        :"g" (fd), "g"(buffer), "g"(size) );
	
    if (resultat >= 0) 
        return resultat;
    else {
        errno = -1*resultat;
        return -1;
    }
}


void perror()
{
	if (errno == EFAULT) {
        	char error[] = { "Bad address" };
        	write(1, error, strlen(error));

    } else if (errno == EBADF) {
        	char error[] = { "Bad file descriptor" };
        	write(1, error, strlen(error));

   	} else if (errno == EINVAL) {
        	char error[] = { "Invalid argument" };
        	write(1, error, strlen(error));
        	
    } else if (errno == EACCES) {
    		char error[] = { "Permision denied" };
        	write(1, error, strlen(error));
	}
}


int gettime() {

    int ticks;
    __asm__ __volatile__(
	
		"movl $0x0A, %%eax\n"
		"int  $0x80\n"
		"movl %%eax, %0\n"

	   	:"=g"(ticks) );

	return ticks;
}

void itoa(int a, char *b) {
    int i, i1;
    char c;  
    if (a == 0) { 
        b[0] = '0'; 
        b[1] =  0; 
        return;
    }
  
    i=0;
    while (a > 0) {
        b[i] = (a%10) + '0';
        a = a/10;
        i++;
    }
  
    for (i1 = 0; i1 < i/2; i1++) {
        c = b[i1];
        b[i1] = b[i-i1-1];
        b[i-i1-1] = c;
    }
    
    b[i] = 0;
}

int strlen(char *a)
{
    int i;  
    i=0;  
    while (a[i]!=0) i++;  
    return i;
}
