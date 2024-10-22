  .intel_syntax noprefix

  .text
  .global add
add:
  push rbp
  mov rbp, rsp

  mov rax, rdi
  add rax, rsi

  pop rbp
  ret
