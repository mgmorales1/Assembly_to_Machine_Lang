// Sample file to decode
da000002      //  ble
ca000002      //  bgt
1a000002      //  bne
0a000002      //  beq
ba000002      //  blt
aa000002      //  bge

e0433000      //  sub     r3, r3, r0
e2433005      //  sub     r3, r3, #5
e2844001      //  add     r4, r4, #1
e0823003      //  add     r3, r2, r3
e1a01008      //  mov     r1, r8
e3a03001      //  mov     r3, #1
e1560004      //  cmp     r6, r4
e3520000      //  cmp     r2, #0
e3530006       // cmp     r3, #6

e59f5004       // ldr     r5, [r15, #4]
e5955000       // ldr     r5, [r5]
e5810000       // str     r0, [r1]
