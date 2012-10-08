# 1 "entry.S"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "entry.S"
# 9 "entry.S"
# 1 "include/asm.h" 1
# 10 "entry.S" 2

# 1 "include/segment.h" 1
# 12 "entry.S" 2
# 77 "entry.S"
      pushl %gs;

      pushl %fs;

      pushl %es;

      pushl %ds;

      pushl %eax;

      pushl %ebp;

      pushl %edi;

      pushl %esi;

      pushl %edx;

      pushl %ecx;

      pushl %ebx;

      movl $0x18, %edx;

      movl %edx, %ds;

      movl %edx, %es





      popl %ebx;

      popl %ecx;

      popl %edx;

      popl %esi;

      popl %edi;

      popl %ebp;

      popl %eax;

      popl %ds;

      popl %es;

      popl %fs;

      popl %gs;







        movb $0x20, %al ;

        outb %al, $0x20 ;





.globl keyboard_handler; .type keyboard_handler, @function; .align 0; keyboard_handler:

       

       

        CALL keyboard_service_routine

       

        iret





.globl clock_handler; .type clock_handler, @function; .align 0; clock_handler:

       

       

        CALL clock_service_routine

       

        iret
