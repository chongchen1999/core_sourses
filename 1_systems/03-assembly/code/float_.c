#include <math.h>
#include <stdint.h>
#include <stdio.h>

// 函数声明
float float_ops(float a, float b);
double double_ops(double a, double b);
int float_to_int(float f);
float int_to_float(int i);
void bitwise_float(float f);
void compare_floats(float a, float b);

// 打印浮点数的二进制表示
void print_float_bits(float f) {
    uint32_t* p = (uint32_t*)&f;
    printf("Float %.2f (0x%08x): ", f, *p);
    for (int i = 31; i >= 0; i--) {
        printf("%d", (*p >> i) & 1);
        if (i == 31 || i == 23)
            printf(" ");
    }
    printf("\n");
}

// 打印双精度浮点数的二进制表示
void print_double_bits(double d) {
    uint64_t* p = (uint64_t*)&d;
    printf("Double %.2lf (0x%016lx): ", d, *p);
    for (int i = 63; i >= 0; i--) {
        printf("%lu", (*p >> i) & 1);
        if (i == 63 || i == 52)
            printf(" ");
    }
    printf("\n");
}

int main() {
    // 1. 基本浮点运算 - 对应 addss/subss/mulss/divss 等指令
    float f1 = 3.14f;
    float f2 = 2.71f;
    float f_result = float_ops(f1, f2);
    printf("Float ops result: %.2f\n", f_result);

    // 2. 双精度浮点运算 - 对应 addsd/subsd/mulsd/divsd 等指令
    double d1 = 3.1415926535;
    double d2 = 2.7182818284;
    double d_result = double_ops(d1, d2);
    printf("Double ops result: %.10lf\n", d_result);

    // 3. 浮点与整数转换 - 对应 cvttss2si/cvtsi2ss 等指令
    int i = float_to_int(123.456f);
    printf("Float to int: %d\n", i);

    float f = int_to_float(42);
    printf("Int to float: %.2f\n", f);

    // 4. 浮点位级操作 - 对应 andps/orps/xorps 等指令
    bitwise_float(3.14f);

    // 5. 浮点数比较 - 对应 ucomiss/comiss 等指令
    compare_floats(1.0f, 2.0f);
    compare_floats(2.0f, 2.0f);
    compare_floats(3.0f, 2.0f);
    compare_floats(0.0f, 0.0f / 0.0f); // NaN比较

    return 0;
}

// 单精度浮点运算函数 - 观察对应的SSE指令
float float_ops(float a, float b) {
    float result;

    // 加法 - 对应 addss 指令
    result = a + b;
    printf("%.2f + %.2f = %.2f\n", a, b, result);

    // 减法 - 对应 subss 指令
    result = a - b;
    printf("%.2f - %.2f = %.2f\n", a, b, result);

    // 乘法 - 对应 mulss 指令
    result = a * b;
    printf("%.2f * %.2f = %.2f\n", a, b, result);

    // 除法 - 对应 divss 指令
    result = a / b;
    printf("%.2f / %.2f = %.2f\n", a, b, result);

    // 平方根 - 对应 sqrtss 指令
    result = sqrtf(a);
    printf("sqrt(%.2f) = %.2f\n", a, result);

    return result;
}

// 双精度浮点运算函数 - 观察对应的SSE2指令
double double_ops(double a, double b) {
    double result;

    // 加法 - 对应 addsd 指令
    result = a + b;
    printf("%.10lf + %.10lf = %.10lf\n", a, b, result);

    // 减法 - 对应 subsd 指令
    result = a - b;
    printf("%.10lf - %.10lf = %.10lf\n", a, b, result);

    // 乘法 - 对应 mulsd 指令
    result = a * b;
    printf("%.10lf * %.10lf = %.10lf\n", a, b, result);

    // 除法 - 对应 divsd 指令
    result = a / b;
    printf("%.10lf / %.10lf = %.10lf\n", a, b, result);

    // 平方根 - 对应 sqrtsd 指令
    result = sqrt(a);
    printf("sqrt(%.10lf) = %.10lf\n", a, result);

    return result;
}

// 浮点到整数转换 - 观察 cvttss2si 指令
int float_to_int(float f) {
    print_float_bits(f);
    int i = (int)f; // 截断转换
    printf("Float %.2f -> int %d\n", f, i);
    return i;
}

// 整数到浮点转换 - 观察 cvtsi2ss 指令
float int_to_float(int i) {
    printf("Int %d -> float %.2f\n", i, (float)i);
    return (float)i;
}

// 浮点位级操作 - 观察 andps/orps/xorps 指令
void bitwise_float(float f) {
    print_float_bits(f);

    // 获取浮点数的位模式
    uint32_t bits = *(uint32_t*)&f;

    // 符号位掩码
    uint32_t sign_mask = 0x80000000;

    // 指数位掩码
    uint32_t exp_mask = 0x7F800000;

    // 尾数位掩码
    uint32_t frac_mask = 0x007FFFFF;

    // 提取各部分
    uint32_t sign = bits & sign_mask;
    uint32_t exp = bits & exp_mask;
    uint32_t frac = bits & frac_mask;

    printf("Sign: 0x%08x, Exp: 0x%08x, Frac: 0x%08x\n", sign, exp, frac);

    // 位操作 - 对应 andps/orps/xorps 指令
    uint32_t inverted_bits = ~bits;
    float inverted_float = *(float*)&inverted_bits;
    printf("Bitwise NOT: ");
    print_float_bits(inverted_float);
}

// 浮点数比较 - 观察 ucomiss/comiss 指令
void compare_floats(float a, float b) {
    print_float_bits(a);
    print_float_bits(b);

    if (a < b) {
        printf("%.2f < %.2f\n", a, b);
    } else if (a > b) {
        printf("%.2f > %.2f\n", a, b);
    } else if (a == b) {
        printf("%.2f == %.2f\n", a, b);
    } else {
        printf("%.2f and %.2f are unordered (at least one is NaN)\n", a, b);
    }
}