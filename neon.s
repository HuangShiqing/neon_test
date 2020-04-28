    .text
    .syntax   unified
    .align   4
    .global   mul_float_neon
    .thumb
    .thumb_func 

mul_float_neon:
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    @  r0: *dst & current dst entry's address浮点型指针，存储结果1
    @  r1: *src1 & current src1 entry's address浮点型指针，操作对象
    @  r2: *src2 & current src2 entry's address浮点型指针，操作对象2
    @  r3: count,循环次数
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
.loop:
    cbz             r3, .return
    subs            r3, r3, #1
 
    vld1.32         {q0}, [r1]!
    vld1.32         {q1}, [r2]!        @ for current set
    @ calculate values for current set
    vmul.f32        q3, q0, q1         @ q3 = q0 + q1
    @ store the result for current set
    vst1.32         {q3}, [r0]!

    b               .loop
.return:
    mov               r0, #0
    bx                lr
