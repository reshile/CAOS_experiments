  .text
  .global add_scanf

  .macro push Xn
    sub sp, sp, 16
    str \Xn, [sp]
  .endm

  .macro pop Xn
    ldr \Xn, [sp]
    add sp, sp, 16
  .endm

add_scanf:
  push x30
  sub sp, sp, 16

  ldr x0, =scanf_format_string
  mov x1, sp
  mov x2, sp
  add x2, x2, 8
  bl scanf
  
  ldr x0, [sp, 8]
  ldr x1, [sp]
  add x0, x0, x1

  add sp, sp, 16
  pop x30
  ret

  .section .rodata

scanf_format_string:
  .string "%lld %lld"
