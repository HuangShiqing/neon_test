#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

extern void mul_float_neon(float *dst, float *src1, float *src2, int count);
// extern void mul_float_asm(float *dst, float *src1, float *src2, int count);

double what_time_is_it_now()
{
    struct timeval time;
    if (gettimeofday(&time, NULL))
    {
        return 0;
    }
    return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

void compare(float *dst1, float *dst2, int len, char *name)
{
    for (int i = 0; i < len; i++)
    {
        if (dst1[i] != dst2[i])
        {
            printf("mul_float_c and %s result diff!!!\r\n", name);
            return;
        }
    }
    printf("mul_float_c and %s result same!!!\r\n", name);
}

void mul_float_c(float *dst, float *src1, float *src2, int count)
{
    int i = 0, j = 0;
    for (j = 0; j < count; j++)
        for (i = 0; i < 4; i++)
            *(dst++) = *(src1++) * *(src2++);
}

void mul_float_neon_inline(float *dst, float *src1, float *src2, int count)
{
    asm volatile(
        ".loop:\n"
        "cbz             %[count], .return\n"// 把汇编函数复制过来，每句后面加个\n，同时输入参数r0、r1等都换掉
        "subs            %[count], %[count], #1\n"
        "vld1.32         {q0}, [%[src1]]!\n"
        "vld1.32         {q1}, [%[src2]]!        @ for current set\n"
        "vmul.f32        q3, q0, q1         @ q3 = q0 + q1\n"
        "vst1.32         {q3}, [%[dst]]!\n"
        "b               .loop\n"
        ".return:\n"
        // "mov               %[dst], #0\n"//不需要函数的返回跳转
        // "bx                lr\n"
        ://解释返回参数,如[ dst ] "+r"(dst)，有个加号
        : [ dst ] "r"(dst), [ src1 ] "r"(src1), [ src2 ] "r"(src2), [ count ] "r"(count)// 解释输入参数
        : "memory", "q0", "q1", "q3");// 不太懂，但是要加
}

#include <arm_neon.h>//要用neon内联函数必须要该头文件
void mul_float_neon_intrinsics(float *dst, float *src1, float *src2, int count)
{
    int i;
    for (i = 0; i < count; i++)
    {
        float32x4_t in1, in2, out; //float32x4_t这个类型就像是q
        in1 = vld1q_f32(src1);
        src1 += 4;
        in2 = vld1q_f32(src2);
        src2 += 4;
        out = vmulq_f32(in1, in2);
        vst1q_f32(dst, out);
        dst += 4;
    }
}

int main()
{
    int count = 500;
    float *dst_c = calloc(4 * count, sizeof(float));
    // float *dst_asm = calloc(4 * count, sizeof(float));
    float *dst_neon = calloc(4 * count, sizeof(float));
    float *dst_neon_inline = calloc(4 * count, sizeof(float));
    float *dst_neon_intrinsics = calloc(4 * count, sizeof(float));
    float *src1 = calloc(4 * count, sizeof(float));
    float *src2 = calloc(4 * count, sizeof(float));
    // 赋值
    for (int i = 0; i < count; i++)
    {
        src1[i] = (i + 1) / 1000.0;
        src2[i] = (i + 1) / 1000.0;
    }

    double t1, t2;
    t1 = what_time_is_it_now();
    mul_float_c(dst_c, src1, src2, count);
    t2 = what_time_is_it_now();
    printf("mul_float_c used: %f s\r\n", t2 - t1);

    // t1 = what_time_is_it_now();
    // mul_float_asm(dst_asm, src1, src2, count);
    // t2 = what_time_is_it_now();
    // printf("mul_float_asm used: %f s\r\n", t2 - t1);

    t1 = what_time_is_it_now();
    mul_float_neon(dst_neon, src1, src2, count);
    t2 = what_time_is_it_now();
    printf("mul_float_neon used: %f s\r\n", t2 - t1);

    t1 = what_time_is_it_now();
    mul_float_neon_inline(dst_neon_inline, src1, src2, count);
    t2 = what_time_is_it_now();
    printf("mul_float_neon_inline used: %f s\r\n", t2 - t1);

    t1 = what_time_is_it_now();
    mul_float_neon_intrinsics(dst_neon_intrinsics, src1, src2, count);
    t2 = what_time_is_it_now();
    printf("mul_float_neon_intrinsics used: %f s\r\n", t2 - t1);

    // compare(dst_c, dst_asm, count * 4, "mul_float_asm");
    compare(dst_c, dst_neon, count * 4, "mul_float_neon");
    compare(dst_c, dst_neon_inline, count * 4, "mul_float_inline");
    compare(dst_c, dst_neon_intrinsics, count * 4, "mul_float_intrinsics");

    free(src1);
    free(src2);
    free(dst_c);
    // free(dst_asm);
    free(dst_neon);
    free(dst_neon_inline);
    free(dst_neon_intrinsics);

    return 0;
}