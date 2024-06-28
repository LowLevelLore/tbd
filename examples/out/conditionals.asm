;#; This assembly was generated for x86_64 Windows by the MZ Compiler
	.data

;#; -------------------------Global Variables START-------------------------

	fmt: .asciz "%lld\n"
	k: .space 8

;#; -------------------------Global Variables END-------------------------

	.global _start
	.text

;#; -------------------------Global Functions START-------------------------

	jmp MZ_fn_afterfact
MZ_fn_fact:
;#; ==== ALIGN HEADER ====
	push %rbp
	mov %rsp, %rbp
	sub $32, %rsp

	push %rbx
	push %rsi
	push %rdi
;#; VARIABLE DECLARATION : `a`
	sub $8, %rsp
;#; IF
;#; BINARY OPERATOR : `<`
;#; VARIABLE ACCESS : `n`
	mov 16(%rbp), %rax
;#; INTEGER : `2`
	mov $2, %r10
	mov $0, %r11
	cmp %r10, %rax
	mov $1, %rbx
	cmovl %rbx, %r11
	test %r11, %r11
	jz .L0
;#; VARIABLE REASSIGNMENT : `a`
	movq $1, -32(%rbp)
	jmp .L1
.L0:
;#; VARIABLE REASSIGNMENT : `a`
;#; BINARY OPERATOR : `*`
;#; VARIABLE ACCESS : `n`
	mov 16(%rbp), %rax
;#; FUNCTION CALL : `fact`
	push %rax
;#; BINARY OPERATOR : `-`
;#; VARIABLE ACCESS : `n`
	mov 16(%rbp), %r10
;#; INTEGER : `1`
	mov $1, %r11
	sub %r11, %r10
	push %r10
	call MZ_fn_fact
	add $8, %rsp
	mov %rax, %r10
	pop %rax
	imul %rax, %r10
	mov %r10, -32(%rbp)
.L1:
;#; VARIABLE ACCESS : `a`
	mov -32(%rbp), %rax

	pop %rdi
	pop %rsi
	pop %rbx
	add $40, %rsp
	pop %rbp
	ret
;#; ==== ALIGN FOOTER ====
MZ_fn_afterfact:

;#; -------------------------Global Functions END-------------------------

_start:
;#; ==== ALIGN HEADER ====
	push %rbp
	mov %rsp, %rbp
	sub $32, %rsp

;#; VARIABLE DECLARATION : `k`
	lea k(%rip), %rax
;#; VARIABLE REASSIGNMENT : `k`
;#; FUNCTION CALL : `fact`
	push %rax
;#; INTEGER : `1`
	mov $1, %rax
	push %rax
	call MZ_fn_fact
	add $8, %rsp
	add $8, %rsp
	mov %rax, k(%rip)
;#; DEBUG PRINT INTEGER
;#; VARIABLE ACCESS : `k`
	mov k(%rip), %rax
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
