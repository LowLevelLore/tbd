;#; This assembly was generated for x86_64 Windows by the MZ Compiler
	.data

;#; -------------------------Global Variables START-------------------------

	fmt: .asciz "%lld\n"
	a: .space 8

;#; -------------------------Global Variables END-------------------------

	.global _start
	.text

;#; -------------------------Global Functions START-------------------------


;#; -------------------------Global Functions END-------------------------

_start:
;#; ==== ALIGN HEADER ====
	push %rbp
	mov %rsp, %rbp
	sub $32, %rsp

	lea a(%rip), %rax
	movq $2, a(%rip)
	mov a(%rip), %rax
	mov $2, %r10
	sub %r10, %rax
	mov $0, %r10
	mov $0, %r11
	test %rax, %r10
	mov $1, %r8
	cmov %r8, %r11
	test %r11, %r11
	jz .L0
	mov a(%rip), %rax
	mov %rax, %rdx
	mov %rax, %rbx
	lea fmt(%rip), %rcx
	call printf
	mov %rax, %rbx
.L0:

	mov $0, %rax
	add $32, %rsp
	pop %rbp
	ret
;#; ==== ALIGN FOOTER ====
