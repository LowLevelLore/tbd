;#; This assembly was generated for x86_64 Windows by the MZ Compiler
.data

;#; -------------------------Global Variables START-------------------------

fmt: .asciz "%lld\n"
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
mov $2, %rax
mov a(%rip), %r11
