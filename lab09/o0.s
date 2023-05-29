a:
        .zero   4
modify(int):
        addi    sp,sp,-32
        sw      s0,28(sp)
        addi    s0,sp,32
        sw      a0,-20(s0)
        lui     a5,%hi(a)
        lw      a4,%lo(a)(a5)
        lw      a5,-20(s0)
        add     a4,a4,a5
        lui     a5,%hi(a)
        sw      a4,%lo(a)(a5)
        nop
        lw      s0,28(sp)
        addi    sp,sp,32
        jr      ra
main:
        addi    sp,sp,-32
        sw      s0,28(sp)
        addi    s0,sp,32
        sw      zero,-20(s0)
        j       .L3
.L4:
        lui     a5,%hi(a)
        lw      a5,%lo(a)(a5)
        addi    a4,a5,1
        lui     a5,%hi(a)
        sw      a4,%lo(a)(a5)
        lw      a5,-20(s0)
        addi    a5,a5,1
        sw      a5,-20(s0)
.L3:
        lw      a4,-20(s0)
        li      a5,999
        ble     a4,a5,.L4
        sw      zero,-24(s0)
        j       .L5
.L6:
        lui     a5,%hi(a)
        lw      a4,%lo(a)(a5)
        lw      a5,-24(s0)
        add     a4,a4,a5
        lui     a5,%hi(a)
        sw      a4,%lo(a)(a5)
        lw      a5,-24(s0)
        addi    a5,a5,1
        sw      a5,-24(s0)
.L5:
        lw      a4,-24(s0)
        li      a5,999
        ble     a4,a5,.L6
        lui     a5,%hi(a)
        lw      a5,%lo(a)(a5)
        mv      a0,a5
        lw      s0,28(sp)
        addi    sp,sp,32
        jr      ra