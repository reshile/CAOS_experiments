  .intel_syntax noprefix

  .text
  .global dot_product

dot_product:
  push rbp
  mov rbp, rsp
  push rsi
  push rdx
  push rbx
  sub rsp, 24
  /*
  [rbp - 32] -- divided by 8 part
  [rbp - 40] -- remined after division
  [rbp - 48] -- just alignment of stack
  */

  // saving size_t N TODO: useless command after
  mov r8, rdi
  mov [rbp - 32], rdi
  mov [rbp - 40], rdi
  // calculating main part for 8 division
  shr QWORD PTR [rbp - 32], 3
  // calculating remainder after 8 division
  and QWORD PTR [rbp - 40], 7
  // calculating count without reminder
  sub r8, [rbp - 40]
  shr r8, 1

  // saving const float* A, B
  mov rax, rsi
  mov rbx, rdx
  
  // starting loop
  xor rcx, rcx
  // all registers put to zero IMPORTANT!!!
  vzeroall
  // check if main part [rbp - 32] after 8 division is zero
  cmp DWORD PTR [rbp - 32], 0
  jz reminder
loop:
  // saving into regs
  vmovups ymm0, [rax + rcx * 8]
  vmovups ymm1, [rbx + rcx * 8]
  // mult
  vmulps ymm0, ymm0, ymm1
  // horiziontal adding
  vhaddps ymm1, ymm0, ymm0
  // again
  vhaddps ymm0, ymm1, ymm1
  // copying from ymm1 to ymm0
  vmovups ymm1, ymm0
  // flipping
  vperm2f128 ymm1, ymm0, ymm0, 1
  // getting final sum in pack
  vaddps ymm0, ymm0, ymm1
  // saving tmp result
  vaddps ymm2, ymm0, ymm4 
  // saving result for next iteration
  vmovups ymm4, ymm2
  // increment rcx
  add rcx, 4
  cmp rcx, r8
  jl loop  

  // checking [rbp - 40] -- reminder after 8 division
  cmp QWORD PTR [rbp - 40], 0
  jz final

reminder:
  //TODO

  // calculating shift address
  mov rcx, [rbp - 32]
  shl rcx, 5

  vmovups ymm0, [rax + rcx]
  vmovups ymm1, [rbx + rcx]
  // mult
  vmulps ymm0, ymm0, ymm1
  // horiziontal adding
  vhaddps ymm1, ymm0, ymm0
  // again
  vhaddps ymm0, ymm1, ymm1
  // copying from ymm1 to ymm0
  vmovups ymm1, ymm0
  // flipping
  vperm2f128 ymm1, ymm0, ymm0, 1
  // getting final sum in pack
  vaddps ymm0, ymm0, ymm1
  // saving into ymm2
  vaddps ymm4, ymm0, ymm4 
  // increment rcx

final:

  // final result
  vmovups ymm0, ymm4

  add rsp, 24
  pop rbx
  pop rdx
  pop rsi
  mov rsp, rbp
  pop rbp
  ret
