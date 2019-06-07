#This is print size of n image
#n = $s0
    
		.text
		.globl  main
getstep:
		
		#卡比獸的爬法術關係式子為: P(n) = P(n-1) + P(n-2), int getstep(int a0);
		li      $t1, 1
		li      $t2, 1
		li      $t3, 1
		#for(int t4 = 1;t4<s0;t4++);
		li      $t4, 1
		beq     $t4, $t4, flchk1
forloop1:
		add     $t3, $t1, $t2
		move    $t1, $t2
		move    $t2, $t3
		addi    $t4, 1
flchk1:	
		blt     $t4, $a0, forloop1
		move    $v0, $t3                #return t3
		jr      $ra
		
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
		
		move    $a0, $s0
		li      $t0, 4
		sub     $sp, $sp, $t0
		sw      $ra, 0($sp)
		jal     getstep
		lw      $ra, 0($sp)
		addi    $sp, 4
		move    $s1, $v0
		
		li      $v0, 4
		la      $a0, result
		syscall
		li      $v0, 1
		move    $a0, $s1
		syscall
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
result: .asciiz "Result:"
nl:     .asciiz "\n"
prompt:	.asciiz ">"