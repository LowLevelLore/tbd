;#; This assembly was generated for x86_64 Windows by the MZ Compiler
.data

;#; -------------------------Global Variables START-------------------------

fmt: .asciz "%lld\n"
b: .space 8
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
movq $10, (%rax)
lea b(%rip), %rax
movq $20, (%rax)
movq $30, a(%rip)
mov %rax, %rbx
lea fmt(%rip), %rcx
lea a(%rip), %r12
mov (%r12), %rdx
call printf
mov %rax, %rbx
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
