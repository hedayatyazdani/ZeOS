#include <libc.h>

char buff[24];

int pid;

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

	
	//;

/*
	char salto = '\n';	
	print(&(salto)); print(&(salto)); 
	
	print ("\n time: ");
	
	int ticks = gettime();
	char tick;	
  	while(ticks < 10) {
		ticks = gettime();
		itoa(ticks, &tick);		
		
		print(&(salto)); print (&tick); print(&(salto));
	}

	print("\n Check perror \n");

	char prova[] = {"\n Check EBADF \n"};
	
	write(0, prova, strlen(prova));
	
	print(&(salto)); perror(); print(&(salto));

	char prova2[] = {"\n Check EINVAL \n"};
	write(1, prova2, 0); 
	
	print(&(salto)); perror(); print(&(salto));
	
	
	
	

	char* t;
	itoa(time,t);
*/


    char *txt;
	int time=10;
	while (time < 200) {
	time = gettime();
	itoa(time,&txt);
	print("\n  \n");
    print (&txt);	
	print("\n  \n");
		
	}
	
	print("\n Check init \n");
	
	while(1) {}
}
