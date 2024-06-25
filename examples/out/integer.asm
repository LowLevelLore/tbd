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
mov $10, %r10
mov %r10, a(%rip)
lea b(%rip), %rax
mov $20, %r11
mov %r11, b(%rip)
movq $30, a(%rip)
mov a(%rip), %rax
mov %rax, %rdx
mov %rax, %rbx
lea fmt(%rip), %rcx
call printf
mov %rax, %rbx
mov b(%rip), %rax
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
