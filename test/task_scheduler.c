#include <stdint.h>
#include <stdio.h>
#include "myusart.h"
#include "task_sheduler.h"

void task1_handler(void);
void task2_handler(void);
void task3_handler(void);
void task4_handler(void);

void init_systick_timer(uint32_t tick_hz);
__attribute__((naked)) void init_scheduler_stack(uint32_t sched_top_of_stack);
void init_task_stack(void);
void enable_processor_faults(void);
__attribute__((naked)) void swich_sp_to_psp(void);
uint32_t get_psp_value(void);
void save_psp_value(uint32_t current_psp_addr);
void update_next_task(void);

/* global variables */
uint32_t psp_of_tasks[MAX_TASKS] = {T1_STACK_START, T2_STACK_START, T3_STACK_START, T4_STACK_START};
uint32_t task_handlers[MAX_TASKS];
uint8_t current_task = 0; // task1 is running

int main(void)
{
    MYUSART_Init();
    enable_processor_faults();

    init_scheduler_stack(SCHED_STACK_START);
    task_handlers[0] = (uint32_t)task1_handler;
    task_handlers[1] = (uint32_t)task2_handler;
    task_handlers[2] = (uint32_t)task3_handler;
    task_handlers[3] = (uint32_t)task4_handler;

    init_task_stack();
    init_systick_timer(TICK_HZ);
    swich_sp_to_psp();
    task1_handler();
    while (1)
        ;
    return 0;
}

void task1_handler(void)
{
    while (1)
    {
        printf("This is task1\n");
    }
}
void task2_handler(void)
{
    while (1)
    {
        printf("This is task2\n");
    }
}
void task3_handler(void)
{
    while (1)
    {
        printf("This is task3\n");
    }
}
void task4_handler(void)
{
    while (1)
    {
        printf("This is task4\n");
    }
}

void init_systick_timer(uint32_t tick_hz)
{
    uint32_t *pSRVR = (uint32_t *)0xE000E014;               // SYST_RVR address
    uint32_t *pSCSR = (uint32_t *)0xE000E010;               // SYST_CSR address
    uint32_t count_value = (SYSTICK_TIM_CLK / tick_hz) - 1; // must be N-1，可以參考 SysTick Reload Value Register 的說明

    // clear the value in to SYST_RVR
    *pSRVR &= ~(0x00FFFFFF);
    // load the value in to SYST_RVR
    *pSRVR |= count_value;

    // do some setting
    *pSCSR |= (1 << 2); // Indicates the clock source, processor clock
    *pSCSR |= (1 << 1); // Enables SysTick exception request
    *pSCSR |= (1 << 0); // Enables the counter
}

__attribute__((naked)) void init_scheduler_stack(uint32_t sched_top_of_stack)
{
    // sched_top_of_stack assign to MSP
    __asm volatile("MSR MSP,%0" ::"r"(sched_top_of_stack)
                   :);
    // BX LR: Return from function call
    __asm volatile("BX LR");
}

void init_task_stack(void)
{
    uint32_t *pPSP;
    for (int i = 0; i < MAX_TASKS; i++)
    {
        pPSP = (uint32_t *)psp_of_tasks[i];
        pPSP--;             // xPSR
        *pPSP = DUMMY_XPSR; //
        pPSP--;             // PC
        *pPSP = task_handlers[i];
        pPSP--; // LR
        *pPSP = 0xFFFFFFFD;

        // R0~R12
        for (int j = 0; j < 13; j++)
        {
            pPSP--;
            *pPSP = 0;
        }
        psp_of_tasks[i] = (uint32_t)pPSP; // store PSP
    }
}

uint32_t get_psp_value(void)
{
    return psp_of_tasks[current_task];
}

void save_psp_value(uint32_t current_psp_addr)
{
    psp_of_tasks[current_task] = current_psp_addr;
}

void update_next_task(void)
{
    current_task++;
    current_task %= MAX_TASKS;
}

__attribute__((naked)) void swich_sp_to_psp(void)
{
    // 1. initialize the PSP with TASK1 stack start address

    // get the value of psp of current_task
    __asm volatile("PUSH {LR}"); // preserve LR which connects back to main
    __asm volatile("BL get_psp_value");
    __asm volatile("MSR PSP,R0"); // initialize psp
    __asm volatile("POP {LR}");   // pops back LR value

    // 2. change SP to PSP using CONTROL register
    __asm volatile("MOV R0,#0x02");
    __asm volatile("MSR CONTROL,R0");
    __asm volatile("BX LR");
}

__attribute__((naked)) SysTick_Handler(void)
{
    __asm volatile("PUSH {LR}");

    /* Save the context of current task*/
    // 1. Get current running task's PSP value
    __asm volatile("MRS R0,PSP");
    // 2. Using that PSP value stire SF2(R4 to R11)
    __asm volatile("STMDB R0!,{R4-R11}");
    // 3. Save the current value of PSP
    __asm volatile("BL save_psp_value");

    /* Retrieve the context of next task */
    // 1. Decide next task to run
    __asm volatile("BL update_next_task");
    // 2. Get its past PSP value
    __asm volatile("BL get_psp_value");
    // 3. Using that PSP value Retrieve SF2(R4 to R11)
    __asm volatile("LDMIA R0!,{R4-R11}");
    // 4.update PSP and exit
    __asm volatile("MSR PSP,R0");

    __asm volatile("POP {LR}");
    __asm volatile("BX LR");
}

void enable_processor_faults(void)
{
    uint32_t *pSHCSR = (uint32_t *)0xE000ED24; // System Handler Control and State Register
    *pSHCSR |= (1 << MEMFAULTENA);             // mem manage fault
    *pSHCSR |= (1 << BUSFAULTENA);             // bus fault
    *pSHCSR |= (1 << USGFAULTENA);             // usage fault
}

// implement the fault handlers
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
