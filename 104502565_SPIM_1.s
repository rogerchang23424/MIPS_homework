#This is print size of n image
#n = $s0
    
		.text
		.globl  main
main:
		
read:   
		li      $v0, 4
		la      $a0, prompt
		syscall
		li      $v0, 5
		syscall
		move    $s0, $v0
		
		#if s0(n) == 0, end this program
		li      $t0, 0
		beq     $s0, $t0, endproc
		add     $s1, $s0, $s0
		
		#print:       *
		#            ***
		#           * * *
		#          *  *  *
		li      $t1, 0
		move    $t2, $s0
		move    $t3, $s0
		move    $t4, $s0
		beq     $t1, $t1, flchk1_t1
forloop1_t1:
		li      $t5, 1
		move    $t6, $s1
		beq     $t5, $t5, flchk1_t5
		forloop1_t5:
				beq     $t5, $t2, trueresult1
				beq     $t5, $t3, trueresult1
				beq     $t5, $t4, trueresult1
				li      $v0, 11
				lbu     $a0, space
				syscall
				beq     $t5, $t5, endif1
		trueresult1:
				li      $v0, 11
				lbu     $a0, star
				syscall
		endif1: 
				addi    $t5, 1
		flchk1_t5:
				blt     $t5, $t6, forloop1_t5
		addi    $t3, -1
		addi    $t4, 1
		li      $v0, 4
		la      $a0, nl
		syscall
		addi    $t1, 1
flchk1_t1: 
		blt     $t1, $s0, forloop1_t1
		
		li      $t1, 0
		move    $t2, $s0
		beq     $t1, $t1, flchk2_t1
forloop2_t1:
		li      $t5, 1
		add     $t6, $s0, $t5
		beq     $t5, $t5, flchk2_t5
		forloop2_t5:
				beq     $t5, $t2, trueresult2
				li      $v0, 11
				lbu     $a0, space
				syscall
				beq     $t5, $t5, endif2
		trueresult2:
				li      $v0, 11
				lbu     $a0, star
				syscall
		endif2: 
				addi    $t5, 1
		flchk2_t5:
				blt     $t5, $t6, forloop2_t5
		li      $v0, 4
		la      $a0, nl
		syscall
		addi    $t1, 1
flchk2_t1: 
		blt     $t1, $s0, forloop2_t1
		
		li      $t1, 1
		beq     $t1, $t1, flchk3_t1
forloop3_t1:
		li      $v0, 11
		lbu     $a0, star
		syscall
		addi    $t1, 1
flchk3_t1:
		blt     $t1, $s1, forloop3_t1
		li      $v0, 4
		la      $a0, nl
		syscall
		
		li      $t1, 3
		li      $t2, 1
		li      $t3, 2
		addi    $t4, $s1, -2
		addi    $t5, $s1, -1
		beq     $t1, $t1, flchk4_t1
forloop4_t1:
		li      $t6, 1
		beq     $t6, $t6, flchk4_t6
		forloop4_t6:
				beq     $t2, $t6, trueresult4
				beq     $t3, $t6, trueresult4
				beq     $t4, $t6, trueresult4
				beq     $t5, $t6, trueresult4
				li      $v0, 11
				lbu     $a0, space
				syscall
				beq     $t5, $t5, endif4
		trueresult4:
				li      $v0, 11
				lbu     $a0, star
				syscall
		endif4: 
				addi    $t6, 1
		flchk4_t6:
				blt     $t6, $s1, forloop4_t6
		addi    $t3, 1
		addi    $t4, -1
		li      $v0, 4
		la      $a0, nl
		syscall
		addi    $t1, 1
flchk4_t1:
		blt     $t1, $s1, forloop4_t1
		
		li      $t1, 1
		beq     $t1, $t1, flchk5_t1
forloop5_t1:
		li      $v0, 11
		lbu     $a0, star
		syscall
		addi    $t1, 1
flchk5_t1:
		blt     $t1, $s1, forloop5_t1
		li      $v0, 4
		la      $a0, nl
		syscall
		
		j       read
		
endproc:
		li      $v0, 10                #exit program
		syscall
		
		.data
star:	.byte   '*'
space:	.byte   ' '
prompt:	.asciiz ">"
nl:     .asciiz "\n"