;#; This assembly was generated for x86_64 Windows by the MZ Compiler
	.data

;#; -------------------------Global Variables START-------------------------

	fmt: .asciz "%lld\n"
	i: .space 8

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

	mov 16(%rbp), %rax
	mov $2, %r10
	imul %rax, %r10
	mov %r10, %rax


	add $32, %rsp
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

	lea i(%rip), %rax
	mov $2, %r11
	mov %r11, i(%rip)
	mov i(%rip), %rax
	push %rax
	call MZ_fn_foo
	add $8, %rsp
	mov %rax, i(%rip)
	mov i(%rip), %rax
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
