/*
 * libc.c 
 */

#include <libc.h>

#include <types.h>

int errno;
int write(int fd, char * buffer, int size) {
        int resultat;
        __asm__ __volatile__(

        "movl %1, %%ebx\n"
        "movl %2, %%ecx\n"
        "movl %3, %%edx\n"
        "movl $0x04, %%eax\n"
        "int  $0x80"
        "movl %%eax %0 \n"


      :"g"(resultat)
      : "g" (fd), "g" (buffer), "g" (size) );

        if (resultat >= 0) return resultat; //no optim, pero ensamblador ho corregirà
        else {
                errno = -1*resultat;
                return -1;
        }
}

int gettime(){
// wrapper
}

void itoa(int a, char *b)
{
  int i, i1;
  char c;
  
  if (a==0) { b[0]='0'; b[1]=0; return ;}
  
  i=0;
  while (a>0)
  {
    b[i]=(a%10)+'0';
    a=a/10;
    i++;
  }
  
  for (i1=0; i1<i/2; i1++)
  {
    c=b[i1];
    b[i1]=b[i-i1-1];
    b[i-i1-1]=c;
  }
  b[i]=0;
}

int strlen(char *a)
{
  int i;
  
  i=0;
  
  while (a[i]!=0) i++;
  
  return i;
}

