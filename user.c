#include <libc.h>

char buff[24];

int pid;

int __attribute__ ((__section__(".text.main")))
  main(void)
{
int ret;
	
	int pid;
	pid = getpid();
	char aux[30];
	itoa(pid, &aux);
	print(aux);
	int time;
	while(1){
		print("\n");
		print("\n");
		time = gettime();
		if (time%50 ==0) {
			ret=fork();
			
			if(ret == 0) {

				print("El hijo tiene pid = ");
				pid = getpid();
				char aux[30];
				itoa(pid, &aux);
				print(aux);
				print("\n");
				print("ARA mato el fill!!!\n");
				//X PROVAR EXIT CAL DESCOMENTAR LINES A SYS.C PK NO HI HA SCHEDULERexit();

			}else if (ret > 0)  {
				
				print("El PADRE tiene pid = ");
				pid = getpid();
				char aux[30];
				itoa(pid, &aux);
				print(aux);
				print("\n");
				//print("i su hijo tiene pid = ");
				//itoa(ret, aux);
				//print(aux);
				//print("\n");
			}else {
				
				perror();
				char aux[30];
				itoa(ret, &aux);
				print(aux);
			}
		}
	}
}
