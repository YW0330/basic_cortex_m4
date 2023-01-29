#include <stdint.h>
#include <stdio.h>
#include "myusart.h"

#define SPI2_IRQNO 36

int main(void)
{
    MYUSART_Init();
    // 1. Manually pend the pending bit for the SPI2 IRQ number in NVIC
    uint32_t *pISPR1 = (uint32_t *)0XE000E204;
    *pISPR1 |= (1 << (SPI2_IRQNO % 32));

    // 2. Enable the SPI2 IRQ number in NVIC
    uint32_t *pISER1 = (uint32_t *)0xE000E104;
    *pISER1 |= (1 << (SPI2_IRQNO % 32));

    for (;;)
        ;
    return 0;
}

// SPI2 IRQ
void SPI2_IRQHandler(void)
{
    printf("in SPI2 ISR\n");
}