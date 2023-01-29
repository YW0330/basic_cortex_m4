#include <stdio.h>
#include "myusart.h"

#define SRAM_START 0x20000000U
#define SRAM_SIZE (64 * 1024)
#define SRAM_END ((SRAM_START) + (SRAM_SIZE))
#define STACK_START SRAM_END

#define STACK_MSP_STACK STACK_START
#define STACK_MSP_END (STACK_MSP_STACK - 512)
#define STACK_PSP_STACK STACK_MSP_END

int func_add(int a, int b, int c, int d)
{
    return a + b + c + d;
}

__attribute__((naked)) void change_sp_to_psp(void)
{
    __asm volatile(".equ SRAM_END,(0x20000000+(64*1024))");
    __asm volatile(".equ PSP_STACK,(SRAM_END-512)");
    __asm volatile("LDR R0,=PSP_STACK");
    __asm volatile("MSR PSP,R0");     // PSP init as R0
    __asm volatile("MOV R0,#0x02");   // Set the SPSEL bit to 1
    __asm volatile("MSR CONTROL,R0"); // Let SP change to PSP
    __asm volatile("BX LR");          // return to main
}

void generate_exception(void)
{
    __asm volatile("SVC #0x2"); // Create SVC exception
}

int main(void)
{
    MYUSART_Init();
    change_sp_to_psp();
    int ret;
    ret = func_add(1, 4, 5, 6);
    printf("result = %d\n", ret);
    generate_exception();
    return 0;
}

void SVC_Handler(void)
{
    printf("In SVC handler\n"); // Handler mode, MSP must be a SP
}