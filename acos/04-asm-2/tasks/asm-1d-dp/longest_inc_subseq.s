  .text
  .global longest_inc_subseq

longest_inc_subseq:
  // x0 -- src
  // x1 -- help for dyn
  // x2 -- count

  cmp x2, 0
  beq zero_case  

  // x5 -- saves 1
  mov x5, 1
  // x4 -- i counter
  mov x4, 0
mainloop:
  cmp x4, x2
  beq final

  // d[i] = 1
  str x5, [x1, x4, LSL 3]
  // x6 -- j counter
  mov x6, 0
  // saving a[i] into x7
  ldr x7, [x0, x4, LSL 3]
  loop2:
    cmp x6, x4
    bge mainend
    // saving a[j] into x8
    ldr x8, [x0, x6, LSL 3]
    cmp x7, x8
    ble loop2end

    // if branch
    // x8 -- d[j]
    // x9 -- d[i]
    ldr x8, [x1, x6, LSL 3]
    add x8, x8, 1
    ldr x9, [x1, x4, LSL 3]
    cmp x8, x9
    bgt case1
    mov x8, x9

    case1:
    str x8, [x1, x4, LSL 3]

    loop2end:
    add x6, x6, 1
    b loop2

mainend:
  add x4, x4, 1
  b mainloop


final:
  mov x4, 0
  // saving max
  ldr x0, [x1, x4, LSL 3]
search_max:
  cmp x4, x2
  bge answer
  
  ldr x3, [x1, x4, LSL 3]
  cmp x3, x0
  bgt max
  b loop3end

  max:
  mov x0, x3
  
loop3end:
  add x4, x4, 1
  b search_max


answer:

  ret

zero_case:
  mov x0, 0
  b answer
