#include <immintrin.h>
#include <stdio.h>
 
float aa[] = {10, 20, 30, 40, 50, 60, 70, 80};
float bb[] = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
float cc[] = {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5};
 
int main(int argc, char* argv[])
{
 
    __m256 first  = _mm256_loadu_ps (aa);
    __m256 second = _mm256_loadu_ps (bb);
    __m256 result = _mm256_add_ps (first, second);
                    _mm256_storeu_ps (cc, result);
                     
    printf("==%ld \n", sizeof(float));
    for (int i = 0;i < 8; i++)
    {
        printf("%f\n", cc[i]);
    }
 
    return 0;
}