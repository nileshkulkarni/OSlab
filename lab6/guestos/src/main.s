	.file	"main.c"
	.text
.globl main
	.type	main, @function
main:
	pushl	%ebp
	movl	%esp, %ebp
	andl	$-16, %esp
	subl	$16, %esp
	movl	$1, 4(%esp)
	movl	$1, (%esp)
	call	syscall
	leave
	ret
	.size	main, .-main
	.ident	"GCC: (GNU) 4.4.3"
	.section	.note.GNU-stack,"",@progbits
