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

push %rbx
push %rsi
push %rdi
;#; BINARY OPERATOR : `*`
;#; VARIABLE ACCESS : `i`
mov 16(%rbp), %rax
;#; INTEGER : `2`
mov $2, %r10
imul %rax, %r10
mov %r10, %rax

 
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

;#; VARIABLE DECLARATION : `i`
lea i(%rip), %rax
;#; VARIABLE REASSIGNMENT : `i`
movq $2, i(%rip)
;#; VARIABLE REASSIGNMENT : `i`
;#; FUNCTION CALL : `foo`
push %rax
;#; VARIABLE ACCESS : `i`
mov i(%rip), %rax
push %rax
call MZ_fn_foo
add $8, %rsp
add $8, %rsp
mov %rax, i(%rip)
;#; DEBUG PRINT INTEGER
;#; VARIABLE ACCESS : `i`
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
