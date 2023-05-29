#include <emmintrin.h> /* header file for the SSE intrinsics */
#include <time.h>
#include <assert.h>
#include <stdio.h>

int **a;
int **b;
int **c;

void naive_add(void) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < 1000; ++i) {
        for (int j = 0; j < 40; ++j) {
            c[i][j] = a[i][j] + b[i][j];
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("naive: %f\n", (float) (end.tv_sec - start.tv_sec) + (float) (end.tv_nsec - start.tv_nsec) / 1000000000.0f);
}

void simd_add(void) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    __m128i ar, br, cr;
    //int r = 40;
    for (int i = 0; i < 1000; ++i)
    {
        for (int j = 0; j < 40; j += 4)
        {
            ar = _mm_loadu_si128((__m128i_u *)(a[i] + j));
            //printf("simd_add %d ", &a[i] + j);
            br = _mm_loadu_si128((__m128i_u *)(b[i] + j));
            //printf("simd_add %d ", **(&b[i] + j));
            cr = _mm_add_epi32(ar, br);
            _mm_storeu_si128((__m128i_u *)(c[i] + j), cr);
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("simd: %f\n", (float) (end.tv_sec - start.tv_sec) + (float) (end.tv_nsec - start.tv_nsec) / 1000000000.0f);

    // TODO: implement me!
}

int main(void) {
    a = malloc(1000 * sizeof(int *));
    b = malloc(1000 * sizeof(int *));
    c = malloc(1000 * sizeof(int *));
    for (int i = 0; i < 1000; ++i) {
        a[i] = malloc(40 * sizeof(int));
        b[i] = malloc(40 * sizeof(int));
        c[i] = malloc(40 * sizeof(int));
    }

    for (int i = 0; i < 1000; ++i) {
        for (int j = 0; j < 40; ++j) {
            a[i][j] = i * 40 + j;
            b[i][j] = 2 * (i * 40 - j);
        }
    }
    naive_add();
    simd_add();
    for (int i = 0; i < 1000; ++i) {
        for (int j = 0; j < 40; ++j) {
            assert(c[i][j] == (i * 40 + j + 2 * (i * 40 - j)));
        }
    }

    return 0;
}
