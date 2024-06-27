;#; This assembly was generated for x86_64 Windows by the MZ Compiler
	.data

;#; -------------------------Global Variables START-------------------------

	fmt: .asciz "%lld\n"

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

	push %rbx
	push %rsi
	push %rdi
	mov 24(%rbp), %rax
	test %rax, %rax
	jz .L0
	mov 24(%rbp), %rax
	mov %rax, %rdx
	mov %rax, %rbx
	lea fmt(%rip), %rcx
	call printf
	mov %rax, %rbx
	mov 16(%rbp), %r10
	mov $3, %r11
	mov $0, %rbx
	cmp %r10, %r11
	mov $1, %rdi
	cmove %rdi, %rbx
	test %rbx, %rbx
	jz .L1
	mov $69, %r10
	mov %r10, %rdx
	mov %rax, %rbx
	lea fmt(%rip), %rcx
	call printf
	mov %rax, %rbx
.L1:
.L0:

	pop %rdi
	pop %rsi
	pop %rbx
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

	mov $0, %rax
	push %rax
	mov $3, %rax
	push %rax
	call MZ_fn_foo
	add $16, %rsp
	mov $1, %rax
	push %rax
	mov $3, %rax
	push %rax
	call MZ_fn_foo
	add $16, %rsp
	mov $2, %rax
	push %rax
	mov $3, %rax
	push %rax
	call MZ_fn_foo
	add $16, %rsp
	mov $1, %rax
	push %rax
	mov $4, %rax
	push %rax
	call MZ_fn_foo
	add $16, %rsp

	mov $0, %rax
	add $32, %rsp
	pop %rbp
	ret
;#; ==== ALIGN FOOTER ====
