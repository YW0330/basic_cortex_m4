#include <stdint.h>
#include <stdio.h>
#include "myusart.h"

void SVC_Handler_c(uint32_t *pBaseOfStackFrame);

int32_t add_numbers(int32_t x, int32_t y)
{
    int32_t res;
    __asm volatile("SVC #36");
    __asm volatile("MOV %0,R0"
                   : "=r"(res)::);
    return res;
}

int32_t sub_numbers(int32_t x, int32_t y)
{
    int32_t res;
    __asm volatile("SVC #37");
    __asm volatile("MOV %0,R0"
                   : "=r"(res)::);
    return res;
}

int32_t mul_numbers(int32_t x, int32_t y)
{
    int32_t res;
    __asm volatile("SVC #38");
    __asm volatile("MOV %0,R0"
                   : "=r"(res)::);
    return res;
}

int32_t div_numbers(int32_t x, int32_t y)
{
    int32_t res;
    __asm volatile("SVC #39");
    __asm volatile("MOV %0,R0"
                   : "=r"(res)::);
    return res;
}

int main(void)
{
    MYUSART_Init();

    int32_t res;

    res = add_numbers(40, -90);
    printf("Add result = %ld\n", res);

    res = sub_numbers(25, 150);
    printf("Sub result = %ld\n", res);

    res = mul_numbers(374, 890);
    printf("mul result = %ld\n", res);

    res = div_numbers(67, -3);
    printf("div result = %ld\n", res);
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

    int32_t res;
    int32_t arg1 = pBaseOfStackFrame[0]; // R0
    int32_t arg2 = pBaseOfStackFrame[1]; // R1
    switch (svc_number)
    {
    case 36:
        res = arg1 + arg2;
        break;
    case 37:
        res = arg1 - arg2;
        break;
    case 38:
        res = arg1 * arg2;
        break;
    case 39:
        res = arg1 / arg2;
        break;
    default:
        printf("invalid svc code\n");
        break;
    }
    pBaseOfStackFrame[0] = res;
}