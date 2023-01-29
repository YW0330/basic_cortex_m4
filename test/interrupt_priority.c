#include <stdint.h>
#include <stdio.h>
#include "myusart.h"

#define IRQNO_TIMER2 28
#define IRQNO_I2C1_EV 31

uint32_t *pNVIC_IPRBase = (uint32_t *)0xE000E400;
uint32_t *pNVIC_ISERBase = (uint32_t *)0xE000E100;
uint32_t *pNVIC_ISPRBase = (uint32_t *)0xE000E200;

void configure_priority_for_irqs(uint32_t irq_no, uint8_t priority_value)
{
    // 1. find out IPRn
    uint8_t iprn = irq_no / 4;
    uint32_t *ipr = pNVIC_IPRBase + iprn;
    // 2.position in IPRn
    uint8_t pos = (irq_no % 4) * 8;
    // 3.configure the priority
    *ipr &= ~(0xFF << pos); // clear
    *ipr |= (priority_value << pos);
}

int main(void)
{
    MYUSART_Init();
    // 1. Lets configure the priority for the peripherals
    configure_priority_for_irqs(IRQNO_TIMER2, 0x80);
    // configure_priority_for_irqs(IRQNO_I2C1_EV, 0x80);
    configure_priority_for_irqs(IRQNO_I2C1_EV, 0x70);

    // 2. Set the interrupt pending bit in the NVIC PR
    *pNVIC_ISPRBase |= (1 << IRQNO_TIMER2);

    // 3. Enable the IRQs in NVIC ISER
    *pNVIC_ISERBase |= (1 << IRQNO_I2C1_EV);
    *pNVIC_ISERBase |= (1 << IRQNO_TIMER2);

    while (1)
        ;
    return 0;
}

// ISRs
void TIM2_IRQHandler(void)
{
    printf("[TIM2_IRQHandler]\n");
    *pNVIC_ISPRBase |= (1 << IRQNO_I2C1_EV);
    while (1)
        ;
}

void I2C1_EV_EXTI23_IRQHandler(void)
{
    printf("[I2C1_EV_EXTI23_IRQHandler]\n");
}
