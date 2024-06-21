;#; This assembly was generated for x86_64 Windows by the MZ Compiler
.data

;#; -------------------------Global Variables START-------------------------

fmt: .asciz "%lld\n"
b: .space 0
a: .space 0

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

lea a(%rip), %rax
movq $69, (%rax)
mov %rax, %rbx
lea fmt(%rip), %rcx
lea a(%rip), %r12
mov (%r12), %rdx
call printf
mov %rax, %rbx
movq $420, a(%rip)
mov %rax, %rbx
lea fmt(%rip), %rcx
lea a(%rip), %r12
mov (%r12), %rdx
call printf
mov %rax, %rbx
lea b(%rip), %rax
movq $0, (%rax)
mov %rax, %rbx
lea fmt(%rip), %rcx
lea b(%rip), %r12
mov (%r12), %rdx
call printf
mov %rax, %rbx
movq $42, b(%rip)
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
