#include <libc.h>

char buff[24];

int pid;

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

	
	//;
	//int state = write(1,&tick,strlen(&tick));
	


	//char tick[] = {"EL WRITE FUNCIONA!!!!!aixo e suna prova del write"};
	//int stat = write(1, tick, strlen(tick));
/*
	char salto = '\n';
	print(&(salto));
	print(&(salto));

	
	print("/n EL WRITE FUNCIONA");
    

	print ("ara farem el time");
	int ticks = gettime();
	char tick;
	

  	while(ticks < 10) {

		ticks = gettime();


 	}
	
	print(&(salto));
	itoa(ticks, &tick);
	//print (&tick);
	print(&(salto));


	print("/n ARA PROVAREM SI PERROR FUNCIONA CORRECTAMENT");

	char prova[] = {"anem a probar el EBADF"};
	write(0, prova, strlen(prova));
	print(&(salto));
	perror();
	print(&(salto));

	
	
	char prova2[] = {"anem a probar EINVAL"};
	write(1, prova2, 0);
	print(&(salto));
	perror();
	print(&(salto));
*/
	while(1){}
}
