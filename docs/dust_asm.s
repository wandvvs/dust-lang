	.text
	.file	"dust_prog"
	.globl	main                            # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:                                # %entrypoint
	pushq	%rax
	.cfi_def_cfa_offset 16
	movl	$.L__unnamed_1, %edi
	movl	$.L__unnamed_2, %esi
	xorl	%eax, %eax
	callq	printf@PLT
	movl	$.L__unnamed_3, %edi
	movl	$.L__unnamed_4, %esi
	xorl	%eax, %eax
	callq	printf@PLT
	movl	$.L__unnamed_5, %edi
	movl	$.L__unnamed_6, %esi
	xorl	%eax, %eax
	callq	printf@PLT
	movl	$.L__unnamed_7, %edi
	movl	$.L__unnamed_8, %esi
	xorl	%eax, %eax
	callq	printf@PLT
	movl	$.L__unnamed_9, %edi
	movl	$.L__unnamed_10, %esi
	xorl	%eax, %eax
	callq	printf@PLT
	movl	$.L__unnamed_11, %edi
	movl	$.L__unnamed_12, %esi
	xorl	%eax, %eax
	callq	printf@PLT
	movl	$122, %eax
	popq	%rcx
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        # -- End function
	.type	.L__unnamed_1,@object           # @0
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_1:
	.asciz	"%s\n"
	.size	.L__unnamed_1, 4

	.type	.L__unnamed_2,@object           # @1
.L__unnamed_2:
	.asciz	"From input to LLVM"
	.size	.L__unnamed_2, 19

	.type	.L__unnamed_4,@object           # @2
.L__unnamed_4:
	.asciz	"true"
	.size	.L__unnamed_4, 5

	.type	.L__unnamed_3,@object           # @3
.L__unnamed_3:
	.asciz	"%s\n"
	.size	.L__unnamed_3, 4

	.type	.L__unnamed_6,@object           # @4
.L__unnamed_6:
	.asciz	"false"
	.size	.L__unnamed_6, 6

	.type	.L__unnamed_5,@object           # @5
.L__unnamed_5:
	.asciz	"%s\n"
	.size	.L__unnamed_5, 4

	.type	.L__unnamed_8,@object           # @6
.L__unnamed_8:
	.asciz	"true"
	.size	.L__unnamed_8, 5

	.type	.L__unnamed_7,@object           # @7
.L__unnamed_7:
	.asciz	"%s\n"
	.size	.L__unnamed_7, 4

	.type	.L__unnamed_10,@object          # @8
.L__unnamed_10:
	.asciz	"froom boolean to str"
	.size	.L__unnamed_10, 21

	.type	.L__unnamed_9,@object           # @9
.L__unnamed_9:
	.asciz	"%s\n"
	.size	.L__unnamed_9, 4

	.type	.L__unnamed_12,@object          # @10
.L__unnamed_12:
	.asciz	"true"
	.size	.L__unnamed_12, 5

	.type	.L__unnamed_11,@object          # @11
.L__unnamed_11:
	.asciz	"%s\n"
	.size	.L__unnamed_11, 4

	.section	".note.GNU-stack","",@progbits
