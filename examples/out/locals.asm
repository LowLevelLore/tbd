;#; This assembly was generated for x86_64 Windows by the MZ Compiler
.data

;#; -------------------------Global Variables START-------------------------

fmt: .asciz "%lld\n"
a: .space 8
b: .space 8

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
sub $8, %rsp
movq $20, -40(%rbp)
movq $30, -40(%rbp)
movq $22, -32(%rbp)
mov %rax, %rbx
lea fmt(%rip), %rcx
lea -40(%rbp), %r12
mov (%r12), %rdx
call printf
mov %rax, %rbx
 
add $48, %rsp
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

lea b(%rip), %rax
movq $30, (%rax)
lea a(%rip), %rax
movq $34, (%rax)
mov %rax, %rbx
lea fmt(%rip), %rcx
lea b(%rip), %r12
mov (%r12), %rdx
call printf
mov %rax, %rbx

mov $0, %rax
add $32, %rsp
pop %rbp
ret
;#; ==== ALIGN FOOTER ====
