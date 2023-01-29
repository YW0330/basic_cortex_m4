#include <stdint.h>
#include <stdio.h>
#include "myusart.h"

#define run(i) Example##i()

// SHCSR bit configuration
#define MEMFAULTENA 16
#define BUSFAULTENA 17
#define USGFAULTENA 18

// CCR bit configuration
#define DIV_0_TRP 4

void UsageFault_Handler_c(uint32_t *pBaseStackFrame);

int func_divide(int x, int y)
{
    return x / y; // SDIV 指令 https://developer.arm.com/documentation/dui0473/m/arm-and-thumb-instructions/sdiv?lang=en
}

// Example
void Example1(void)
{
    //  lets force the processor to execute some undifined instruction
    uint32_t *pSRAM = (uint32_t *)0x20000500; // 未定義的指令 -> UFSR=1
    // uint32_t *pSRAM = (uint32_t *)0x20000501; // 使 some_address 出現偶數 -> T bit 變為 0 -> UFSR=2
    *pSRAM = 0xFFFFFFFF; // 無效指令

    void (*some_address)(void);
    some_address = (void *)(((uint32_t)pSRAM) + 1); // T bit+1
    some_address();
}

void Example2(void)
{
    // enable divide by zero trap
    uint32_t *pCCR = (uint32_t *)0xE000ED14;
    *pCCR |= (1 << DIV_0_TRP);

    // attempt to divide by zero
    func_divide(10, 0); // 除以 0 -> UFSR=200，若沒有致能 usage fault 則會升級成 hard fault
}

int main(void)
{
    MYUSART_Init();

    // 1.enable all configurable exceptions like usage fault, mem manage fault and bus fault
    uint32_t *pSHCSR = (uint32_t *)0xE000ED24; // System Handler Control and State Register
    *pSHCSR |= (1 << MEMFAULTENA);             // mem manage fault
    *pSHCSR |= (1 << BUSFAULTENA);             // bus fault
    *pSHCSR |= (1 << USGFAULTENA);             // usage fault

    // 3. run different cases to generate fault
    run(2);

    // 4. analyze the fault

    while (1)
        ;
    return 0;
}

// 2. implement the fault handlers
void HardFault_Handler(void)
{
    printf("Exception: HardFault\n");
    while (1)
        ;
}

void BusFault_Handler(void)
{
    printf("Exception: BusFault\n");
    while (1)
        ;
}

void MemManage_Handler(void)
{
    printf("Exception: MemManage\n");
    while (1)
        ;
}

__attribute__((naked)) void UsageFault_Handler(void)
{
    // here we extracted the value of MSP which happens to be the
    // base address of the stack frame(thread mode) which got saved during the exception entry
    // from thread mode to handler mode
    __asm("MRS r0, MSP");
    // 根據 AAPCS 在傳遞引數時，順序為 r0, r1, r2, r3，因此 r0 的值會傳到 pBaseStackFrame
    __asm("B UsageFault_Handler_c");
}

void UsageFault_Handler_c(uint32_t *pBaseStackFrame)
{
    uint32_t *pUFSR = (uint32_t *)0xE000ED2A;
    printf("Exception: UsageFault\n");
    printf("UFSR = %lx\n", (*pUFSR) & 0XFFFF);
    printf("pBaseStackFrame = %p\n", pBaseStackFrame);
    printf("Value of R0 = %lx\n", pBaseStackFrame[0]);
    printf("Value of R1 = %lx\n", pBaseStackFrame[1]);
    printf("Value of R2 = %lx\n", pBaseStackFrame[2]);
    printf("Value of R3 = %lx\n", pBaseStackFrame[3]);
    printf("Value of R12 = %lx\n", pBaseStackFrame[4]);
    printf("Value of LR = %lx\n", pBaseStackFrame[5]);
    printf("Value of PC = %lx\n", pBaseStackFrame[6]);
    printf("Value of XPSR = %lx\n", pBaseStackFrame[7]);
    while (1)
        ;
}
