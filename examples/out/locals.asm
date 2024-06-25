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
mov -40(%rbp), %rax
mov %rax, %rdx
mov %rax, %rbx
lea fmt(%rip), %rcx
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

lea b(%rip), %r10
mov $30, %r11
mov %r11, b(%rip)
lea a(%rip), %rax
mov $34, %r10
mov %r10, a(%rip)
mov b(%rip), %rax
mov %rax, %rdx
mov %rax, %rbx
lea fmt(%rip), %rcx
call printf
mov %rax, %rbx
mov $3, %rax
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
