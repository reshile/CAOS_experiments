  .intel_syntax noprefix

  .text
  .global add_scanf

add_scanf:
  push rbp
  mov rbp, rsp

  push rdi
  push rsi
  push rdx

  // alignment
  sub rsp, 24

  mov rdi, offset scanf_format_string
  mov rsi, rsp
  add rsi, 16
  mov rdx, rsp
  add rdx, 8
  call scanf

  mov rax, [rsp + 16]
  add rax, [rsp + 8]

  add rsp, 24
  pop rdx
  pop rsi
  pop rdi
  pop rbp
  ret

  .section .rodata

scanf_format_string:
  .string "%lld %lld"
