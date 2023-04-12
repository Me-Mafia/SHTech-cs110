# DO NOT REMOVE THIS LINE WHEN SUBMITTING
.globl LPS

LPS:
    # YOUR CODE HERE
    addi sp sp -4 # sp = sp1 - 16
    sw s1 0(sp)

    addi a3 x0 1 # i = 1
    addi a2 x0 1 # tmp = 1
    addi a1 x0 0 # max_pal = 1
    mv s1 ra # save ra to s1
    jal check_odd  # jump to check_odd and save position to ra

LPS_odd:
    addi a3 x0 2 # i = 2 
    addi a2 x0 2 # tmp = 2
    jal check_even  # jump to check_even and save position to ra
   
LPS_exit:
    mv a0 a1 # move a1 to a0
    mv ra s1 # load ra from s1
    lw s1 0(sp)
    addi sp sp 4
    jr ra # jump to ra


check_odd: # compute the maximum length of strings of odd lengths
    add a4 a0 a3 # the absolute address
    lb a5 0(a4) # sample_char[i]
    sub a4 a4 a2 # absolute address [i - tmp]
    lb a6 0(a4) # sample_char[i - tmp]
    beq a5 x0 LPS_odd # if sample_char[i] != 0 then return to LPS
    bne a5 a6 tmpset2_odd # if sample_char[i] != sample_char[i-tmp]
    j tmpadd2_odd # else
check_odd_2:
    addi a3 a3 1 # i++
    j check_odd  # jump to check_odd


check_even: # compute the maximum length of strings of even lengths
    # compute the maximum length of strings of even lengths
    add a4 a0 a3 # the absolute address = a0 + i
    lb a5 0(a4) # sample_char[i]
    sub a4 a4 a2 # absolute address [i - tmp]
    lb a6 0(a4) # sample_char[i - tmp]
    beq a5 x0 LPS_exit # if sample_char[i] != 0 then return to LPS
    bne a5 a6 tmpset2_even # if sample_char[i] != sample_char[i-tmp]
    j tmpadd2_even # else
check_even_2:
    addi a3 a3 1 # i++
    j check_even  # jump to check_even

tmpadd2_odd:
    addi a2 a2 2 # a2 += 2
    addi a2 a2 -1 # tmp -= 1
    blt a1 a2 setmax_odd # if tmp-1 > max_pal
    addi a2 a2 1 # tmp += 1
    j check_odd_2 # return    
tmpset2_odd:
    li a2 1 # tmp = 1
    add a4 a0 a3
    sub a4 a4 a2
    lb a6 0(a4)
    beq a5 a6 tmpadd2_odd
    j check_odd_2
setmax_odd:
    mv a1 a2 # max_pal = tmp
    addi a2 a2 1 # tmp += 1
    j check_odd_2


tmpadd2_even:
    addi a2 a2 2 # a2 += 2
    addi a2 a2 -1 # tmp -= 1
    blt a1 a2 setmax_even # if tmp-1 > max_pal
    addi a2 a2 1 # tmp += 1
    j check_even_2 # return 
tmpset2_even:
    li a2 2 # tmp = 2
    add a4 a0 a3
    sub a4 a4 a2
    lb a6 0(a4)
    beq a5 a6 tmpadd2_even
    j check_even_2 
setmax_even:
    mv a1 a2 # max_pal = tmp
    addi a2 a2 1 # tmp += 1
    j check_even_2

