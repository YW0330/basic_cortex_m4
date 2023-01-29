#include <stdio.h>
#include "myusart.h"

int main()
{
    MYUSART_Init();
    int a = 1;
    while (1)
    {
        printf("Input data:\n");
        scanf("%d", &a);
        printf("a = %d\n", a);
        fflush(stdin);
    }

    return 0;
}
