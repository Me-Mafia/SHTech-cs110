#include <immintrin.h>
#include <stdio.h>

int main (int argc, char* argv[])
{
    float a[] = {0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2};
    float b[8]; 
    //er = _mm_mul_ps(per, ser);
    __m256 er = _mm256_loadu_ps(a);
    //ser = _mm_load_ps(a);
    //per = _mm_load_ps(a);
    er = _mm256_div_ps(er, er);
    _mm256_storeu_ps(b, er);
    for (int i = 0; i < 8; i++)
    {
       printf("b[%d] is %f\n", i, b[i]);
    }
    return 0;
}