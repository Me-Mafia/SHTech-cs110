#include <stdio.h>

void inc_ptr(int *p)
{ p = p + 5; }

int main()
{
    int A[3] = {50, 60, 70};
    int *q = A;
    q = q + 6;
    inc_ptr( q);
    printf( "q = %d\n",*q);
    return 0;
}
