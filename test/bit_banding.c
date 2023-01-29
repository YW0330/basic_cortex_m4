#include <stdint.h>

#define ALIAS_BASE 0x22000000U
#define BITBAND_BASE 0x20000000U
#define BITBAND_MEMORY_ADDR 0x20000200U
#define BIT_POSITION 7U

int main(void)
{
    uint8_t *ptr = (uint8_t *)0x20000200;
    *ptr = 0xff;

    // normal method
    // clearing 7th bit position
    *ptr &= ~(1 << 7);

    // reset to 0xff
    *ptr = 0xff;

    // bit band method
    uint8_t *alias_addr = (uint8_t *)(ALIAS_BASE + (32 * (BITBAND_MEMORY_ADDR - BITBAND_BASE)) + BIT_POSITION * 4);
    // clearing 7th bit of address
    *alias_addr = 0;
}