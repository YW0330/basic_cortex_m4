#include <stdint.h>
#include <stdio.h>
#include "myusart.h"

void SVC_Handler_c(uint32_t *pBaseOfStackFrame);

int main(void)
{
    MYUSART_Init();

    __asm("SVC #0x08"); // #svc_number

    register uint32_t data __asm("r0"); // register 關鍵字式告知編譯器該值不在 stack 之中，而是在處理器中的暫存器

    // 下列2航程式碼與上一行功能相同
    // uint32_t data;
    // __asm volatile("MOV %0,R0": "=r"(data)::);

    printf("data = %ld\n", data);

    while (1)
        ;
    return 0;
}

__attribute__((naked)) void SVC_Handler(void)
{
    // 1.get the value of the MSP
    __asm("MRS R0,MSP");

    __asm("B SVC_Handler_c");
}

void SVC_Handler_c(uint32_t *pBaseOfStackFrame)
{
    printf("in SVC handler\n");
    uint8_t *pReturn_addr = (uint8_t *)pBaseOfStackFrame[6];

    // 2. decremen the return address by 2 to point to opcode of the SVC instruction in the program memory
    pReturn_addr -= 2;

    // 3. extract SVC number (LSByte of the opcode)
    uint8_t svc_number = *pReturn_addr;
    printf("SVC number is: %d\n", svc_number);

    svc_number += 4;                   // 變更 SVC number
    pBaseOfStackFrame[0] = svc_number; // 用 R0 回傳至 thread mode
}