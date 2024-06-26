;#; This assembly was generated for x86_64 Windows by the MZ Compiler
	.data

;#; -------------------------Global Variables START-------------------------

	fmt: .asciz "%lld\n"
	a: .space 8
	k: .space 8

;#; -------------------------Global Variables END-------------------------

	.global _start
	.text

;#; -------------------------Global Functions START-------------------------

	jmp MZ_fn_afterfoo
MZ_fn_foo:
;#; ==== ALIGN HEADER ====
	push %rbp
	mov %rsp, %rbp
	sub $32, %rsp

	sub $8, %rsp
	movq $10, -32(%rbp)
	mov 16(%rbp), %rax
	mov %rax, %rdx
	mov %rax, %rbx
	lea fmt(%rip), %rcx
	call printf
	mov %rax, %rbx
	mov $2, %r10
	mov $2, %r11
	imul %r10, %r11
	mov %r11, %rax


	add $40, %rsp
	pop %rbp
	ret
;#; ==== ALIGN FOOTER ====
MZ_fn_afterfoo:

;#; -------------------------Global Functions END-------------------------

_start:
;#; ==== ALIGN HEADER ====
	push %rbp
	mov %rsp, %rbp
	sub $32, %rsp

	lea k(%rip), %rax
	movq $3, k(%rip)
	lea a(%rip), %rax
	mov k(%rip), %rax
	mov $2, %r10
	add %rax, %r10
	push %r10
	call MZ_fn_foo
	add $0, %rsp
	mov %rax, a(%rip)
	mov a(%rip), %rax
	mov %rax, %rdx
	mov %rax, %rbx
	lea fmt(%rip), %rcx
	call printf
	mov %rax, %rbx

	mov $0, %rax
	add $32, %rsp
	pop %rbp
	ret
;#; ==== ALIGN FOOTER ====
