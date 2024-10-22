  .intel_syntax noprefix

  .text
  .global my_memcpy

my_memcpy:
  push rbp
  mov rbp, rsp

  // saving params for 8 division
  push 0
  push 0

  add [rbp - 8], edx
  add [rbp - 16], edx

  // evaluating params for 8 div
  shr DWORD PTR [rbp - 8], 3
  and DWORD PTR [rbp - 16], 7

  cmp QWORD PTR [rbp - 8], 0
  jz after
  xor rcx, rcx
loop:
  mov rax, [rsi + rcx * 8]
  mov [rdi + rcx * 8], rax
  inc rcx
  cmp rcx, QWORD PTR [rbp - 8]
  jnz loop

  mov rax, rdi
  cmp QWORD PTR [rbp - 16], 0
  jz after2

after:

  mov rcx, [rbp - 8]
  shl rcx, 3

  mov r9, rsi
  mov r10, rdi

  add r9, rcx
  add r10, rcx

  xor rax, rax

  // need to save [rdi]

  mov rax, [r9]
  mov r8, 1
  shl DWORD PTR [rbp - 16], 3
  mov rcx, [rbp - 16]
  shl r8, cl
  sub r8, 1
  // here we have needed bytes from rsi
  and rax, r8

  mov r11, 0xffffffffffffffff
  sub r11, r8

  and [r10], r11
  add [r10], rax


  mov rax, rdi

after2:

  add rsp, 16
  mov rsp, rbp
  pop rbp
  ret

printf_format_string:
  .string "%p\n"

printf_format_string2:
  .string "%d\n"

