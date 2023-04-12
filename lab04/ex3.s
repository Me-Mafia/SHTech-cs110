.data
n: .word 3 # number of test data
data: # input, answer
.word 0x12345678, 0x1E6A2C48
.word 0x71C924BF, 0xFD24938E
.word 0x19260817, 0xE8106498

.text
# === main and helper functions ===
# You don't need to understand these, but reading them may be useful
main:
    la t0, n
    lw s0, 0(t0)
    la s1, data
    main_loop: # run each of the n tests
        beq s0, zero, main_loop_end
        lw a0, 0(s1)
        jal print_hex_and_space # print input
        lw a0, 4(s1)
        jal print_hex_and_space # print answer
        lw a0, 0(s1)
        jal bitrev1
        jal print_hex_and_space # print result of bitrev1
        lw a0, 0(s1)
        jal bitrev2
        jal print_hex_and_space # print result of bitrev2
        jal print_newline
        addi s0, s0, -1
        addi s1, s1, 8
        j main_loop
    main_loop_end:
    li a0, 10
    ecall # exit

print_hex_and_space:
    mv a1, a0
    li a0, 34
    ecall
    li a1, ' '
    li a0, 11
    ecall
    ret

print_newline:
    li a1, '\n'
    li a0, 11
    ecall
    ret

# === The first version ===
# Reverse the bits in a0 with a loop
bitrev1:
    ### TODO: YOUR CODE HERE ###
    addi sp,sp, -12
    sw s0,4(sp)
    sw s1,8(sp)
    sw s2,0(sp)
    li s0 31
    mv s1,x0
loop1:
    beq s0,x0,done
    andi s2,a0,1
    srli a0,a0,1
    add s1,s1,s2
    slli s1,s1,1
    addi s0,s0,-1
    j loop1
done:
	mv a0,s1 
    lw s0,4(sp)
    lw s1,8(sp)
    lw s2,0(sp)
    addi sp,sp, 12
	ret

# === The second version ===
# Reverse the bits in a0. Only use li, and, or, slli, srli!
bitrev2:
    ### TODO: YOUR CODE HERE ###
        li t0, 0xffffffff # 1111....
    and t1, a0, t0
    and t2, a0, t0
    slli t1, t1, 16
    srli t2, t2, 16
    or a0, t1, t2

    # for each 16 digits, swap the first 8 and last 8
    li, t0, 0x00ff00ff
    and t1, a0, t0
    li, t0, 0xff00ff00
    and t2, a0, t0
    slli t1, t1, 8
    srli t2, t2, 8
    or a0, t1, t2

    # for each 8 digits, swap the first 4 and last 4
    li, t0, 0x0f0f0f0f
    and t1, a0, t0
    li, t0, 0xf0f0f0f0
    and t2, a0, t0
    slli t1, t1, 4
    srli t2, t2, 4
    or a0, t1, t2

    # for each 4 digits, swap the first 2 and last 2
    li, t0, 0x33333333 # 0011 0011 0011 0011...
    and t1, a0, t0
    li, t0, 0xcccccccc # 1100 1100 1100 1100...
    and t2, a0, t0
    slli t1, t1, 2
    srli t2, t2, 2
    or a0, t1, t2

    # for each 2 digits, swap the first 1 and last 1
    li, t0, 0x55555555 # 0101 0101 0101 0101...
    and t1, a0, t0
    li, t0, 0xaaaaaaaa # 1010 1010 1010 1010...
    and t2, a0, t0
    slli t1, t1, 1
    srli t2, t2, 1
    or a0, t1, t2

    ret
    ret
