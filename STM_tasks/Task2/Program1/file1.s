	.file	"file1.c"
	.text
	.globl	GLOBAL_1
	.data
	.align 4
	.type	GLOBAL_1, @object
	.size	GLOBAL_1, 4
GLOBAL_1:
	.long	5
	.globl	GLOBAL_2
	.type	GLOBAL_2, @object
	.size	GLOBAL_2, 1
GLOBAL_2:
	.byte	97
	.section	.rodata
.LC0:
	.string	"%d\n"
.LC1:
	.string	"%c\n"
	.text
	.globl	FUNCTION
	.type	FUNCTION, @function
FUNCTION:
.LFB0:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	GLOBAL_1(%rip), %eax
	movl	%eax, %esi
	leaq	.LC0(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movzbl	GLOBAL_2(%rip), %eax
	movsbl	%al, %eax
	movl	%eax, %esi
	leaq	.LC1(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	FUNCTION, .-FUNCTION
	.ident	"GCC: (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
