# Lab 09

## Part 1: Vector addition
- _mm_load_si128: 4 int for 128 bits
- _mm_loadu_si128: memory not neccessarily aligned 
- _mm_load_pd: 2 double for 128 bits
- _mm_load1_pd: 1 double for 2 elements

## Part 2: Matrix multiplication

## Part 3: Loop unrolling
- 减少了分支判断/跳转
- 指令并行

## Part 4: Compiler optimization

### O3
    它整合了O1和O2的左右优化技巧, 还包括以下优化:

    -finline-functions:
    这种优化技术不为函数创建单独的汇编语言代码，
    而是把函数代码包含在调度程序的代码中。 
    对于多次被调用的函数来说, 为每次函数调用复制函数代码。 
    虽然这样对于减少代码长度不利, 但是通过最充分的利用指令缓存代码, 
    而不是在每次函数调用时进行分支操作, 可以提高性能。

    -fweb: 
    构建用于保存变量的伪寄存器网络。 
    伪寄存器包含数据, 就像他们是寄存器一样, 
    但是可以使用各种其他优化技术进行优化, 比如cse和loop优化技术。

    -fgcse-after-reload:
    这种技术在完全重新加载生成的且优化后的汇编语言代码之后执行第二次
    gcse优化,帮助消除不同优化方式创建的任何冗余段。