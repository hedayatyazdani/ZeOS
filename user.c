#include <libc.h>

char buff[24];

int pid;

int __attribute__ ((__section__(".text.main")))
  main(void)
{
int ret;
	ret=fork();
	while(1){

	if(ret == 0)print("hijo\n");
	else print("padre\n");

	}
}
