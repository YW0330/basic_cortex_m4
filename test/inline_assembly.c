#include <stdint.h>
#define run(i) Example##i()

void Example1()
{
    *(uint32_t *)0x20001000 = 6;
    *(uint32_t *)0x20001004 = 3;
    __asm volatile("LDR R1,=#0x20001000");
    __asm volatile("LDR R2,=#0x20001004");
    __asm volatile("LDR R0,[R1]");
    __asm volatile("LDR R1,[R2]");
    __asm volatile("ADD R0,R0,R1");
    __asm volatile("STR R0,[R2]");
}

void Example2()
{
    int val = 50;
    __asm volatile("MOV R0,%0" ::"r"(val));
}

void Example3()
{
    uint32_t control_reg;
    __asm volatile("MRS %0,CONTROL"
                   : "=r"(control_reg));
}

void Example4()
{
    int var1 = 10, var2;
    __asm volatile("MOV %0,%1"
                   : "=r"(var2)
                   : "r"(var1));
}

void Example5()
{
    int p1, *p2;
    p2 = (int *)0x20000008;
    __asm volatile("LDR %0,[%1]"
                   : "=r"(p1)
                   : "r"(p2)); // p1=*p2
}

int main()
{
    run(1);
    for (;;)
        ;
}
