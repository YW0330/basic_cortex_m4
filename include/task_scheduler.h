#ifndef TASK_SCHEDULER_H_
#define TASK_SCHEDULER_H_

#define MAX_TASKS 4

/* some stack memory calculations */
#define SIZE_TASK_STACK 1024U  // task stack size 1kB
#define SIZE_SCHED_STACK 1024U // scheduler stack size 1kB

#define SRAM_START 0x20000000U
#define SRAM_SIZE ((64) * (1024)) // 64KB
#define SRAM_END ((SRAM_START) + (SRAM_SIZE))

/* 配置各程序的 stack */
// SRAM_END -> T1 stack (1kB) -> T2 stack (1kB) -> T3 stack (1kB)-> T4 stack (1kB)-> scheduler stack (1kB)
#define T1_STACK_START SRAM_END
#define T2_STACK_START ((SRAM_END) - (1 * SIZE_TASK_STACK))
#define T3_STACK_START ((SRAM_END) - (2 * SIZE_TASK_STACK))
#define T4_STACK_START ((SRAM_END) - (3 * SIZE_TASK_STACK))
#define SCHED_STACK_START ((SRAM_END) - (4 * SIZE_TASK_STACK))

/* Systick timer configure */
#define TICK_HZ 1000U             // 1 kHZ
#define HSI_CLOCK 8000000U        // 8 MHz
#define SYSTICK_TIM_CLK HSI_CLOCK // 8 MHz

/* SHCSR bit configuration */
#define MEMFAULTENA 16
#define BUSFAULTENA 17
#define USGFAULTENA 18

#define DUMMY_XPSR 0x01000000U
#endif