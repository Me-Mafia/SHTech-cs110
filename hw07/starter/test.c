#include <stdio.h>
#include <stdint.h>

uint64_t ln(uint64_t x);

int main() {
    uint64_t a = 1024;
    uint64_t b = ln(a);
    //printf("%d\n", b);
    for (size_t i = 0; i < 10; ++i)
    {
      printf("%d\n", i);
    }
    
    return 0;
}

uint64_t ln(uint64_t x)
{
  int ret = 0; int y = 1;
  for (ret = 0; x != y; ret++)
    y *= 2;
  return ret;
}