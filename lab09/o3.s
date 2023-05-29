modify(int):
        lui     a4,%hi(a)
        lw      a5,%lo(a)(a4)
        add     a5,a5,a0
        sw      a5,%lo(a)(a4)
        ret
main:
        lui     a4,%hi(a)
        lw      a0,%lo(a)(a4)
        li      a5,499712
        addi    a5,a5,788
        add     a0,a0,a5
        sw      a0,%lo(a)(a4)
        ret
a:
        .zero   4