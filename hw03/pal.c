#include <stdio.h>

char *sample_char = "aaaaa";
int length;
int tmp = 1;
int max_pal = 0;
int main()
{
    printf("%d\n", length);
    for (int i = 1; sample_char[i] != '\0'; i++)
    {
        printf("odd: ");
        printf("%d %d %d\n", i, sample_char[i], tmp);
        if (sample_char[i] == sample_char[i - tmp])
        {
            tmp += 2;
            if (tmp - 1 > max_pal) max_pal = tmp - 1;
        }  
        else 
        {
            tmp = 1;
        }
    }
    tmp = 2;
    for (int i = 2; sample_char[i] != '\0'; i++)
    {
        printf("even: ");
        printf("%d %d %d\n", i, sample_char[i], tmp);
        if (sample_char[i] == sample_char[i - tmp])
        {
            tmp += 2;
            if (tmp - 1 > max_pal) max_pal = tmp - 1;
        }  
        else 
        {
            tmp = 2;
            if (tmp - 1 > max_pal) max_pal = tmp - 1;
        }
            
    }
    printf("%08d", max_pal);
    return 0;
}