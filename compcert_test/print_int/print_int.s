# File generated by CompCert 3.15
# Command line: -D __bpf_helper_as_extern__ -S -o compcert_test/print_int/print_int.s compcert_test/print_int/print_int.c
	.section	.rodata.str1.1,"aMS",@progbits,1
	.balign 1
__stringlit_1:
	.ascii	"The integer value is %d\000"
	.type	__stringlit_1, @object
	.size	__stringlit_1, . - __stringlit_1
	.text
	.balign 2
	.globl main
main:
	.cfi_startproc
	r0 = r10
	r10 -= 16
	*(u64 *)(r10 + 0) = r0
	w2 = 5
	r1 = "__stringlit_1" + 0 ll
	call printf
	w0 = 0
	r10 += 16
	exit
	.cfi_endproc
	.type	main, @function
	.size	main, . - main
