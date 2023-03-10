#LAB 3 ANSWER

## Exercise 1: Familiarizing yourself with Venus
- .data .word .text ?
    - .data 数据段,存放程序中已被初始化的全局变量(DATA)
    - .word 定义一个字，并分配空间
    - .text 存放程序执行代码(CODE)
    - (STACK)
    - (HEAP)
- output: fib(9) == 34
- n: stored at 0x10000010 after 2, 4, 6, 8

## Exercise 2: Translating from C to RISC-V
- t0: k
- t3: pointer to source; t4: pointer to dest
- for
    - addi t0, t0, 1 `k++`
    - beq t5, x0, exit `source[k] != 0`
    - addi t0, x0, 0 `int k = 0`
- slli t3, t3, 2 