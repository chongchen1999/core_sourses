#include <dlfcn.h> // 提供 dlopen, dlsym, dlclose, dlerror 等动态链接相关函数
#include <stdio.h>  // 提供输入输出函数，如 printf, fprintf
#include <stdlib.h> // 提供 exit 和 EXIT_FAILURE 等宏

// 定义函数指针类型：接受两个输入向量，一个输出向量，以及长度
typedef void (*vec_func)(int*, int*, int*, int);

int main() {
    void* handle;     // 用于保存 dlopen 返回的共享库句柄
    vec_func addvec;  // 指向 addvec 函数的函数指针
    vec_func multvec; // 指向 multvec 函数的函数指针
    char* error;      // 存储错误信息字符串

    // ------------------------
    // 动态加载共享库 libvector.so
    // ------------------------
    handle = dlopen("./libvector.so", RTLD_LAZY);
    /*
     * dlopen 用于动态加载共享库
     * 参数 "./libvector.so" 是共享库路径
     * 参数 RTLD_LAZY 表示符号在使用时再解析（懒加载）
     * 成功时返回库的句柄，失败时返回 NULL
     */
    if (!handle) {
        fprintf(stderr, "dlopen error: %s\n", dlerror());
        exit(EXIT_FAILURE); // 加载失败，退出程序
    }

    // 清除之前可能存在的错误（为调用 dlsym 做准备）
    dlerror();

    // ------------------------
    // 加载 addvec 函数
    // ------------------------
    addvec = (vec_func)dlsym(handle, "addvec");
    /*
     * dlsym 用于获取指定符号（函数/变量）的地址
     * 参数 handle 是 dlopen 返回的句柄
     * 参数 "addvec" 是要查找的符号名
     * 返回值需要强制转换为适当的函数指针类型
     */
    if ((error = dlerror()) != NULL) {
        fprintf(stderr, "dlsym addvec error: %s\n", error);
        dlclose(handle); // 释放资源
        exit(EXIT_FAILURE);
    }

    // ------------------------
    // 加载 multvec 函数
    // ------------------------
    multvec = (vec_func)dlsym(handle, "multvec");
    /*
     * 与上面相同，用于加载另一个函数 multvec
     */
    if ((error = dlerror()) != NULL) {
        fprintf(stderr, "dlsym multvec error: %s\n", error);
        dlclose(handle);
        exit(EXIT_FAILURE);
    }

    // ------------------------
    // 测试数据
    // ------------------------
    int x[4] = {1, 2, 3, 4};
    int y[4] = {10, 20, 30, 40};
    int z[4]; // 存放结果

    // ------------------------
    // 调用 addvec 函数
    // ------------------------
    addvec(x, y, z, 4);
    printf("addvec result: ");
    for (int i = 0; i < 4; i++) {
        printf("%d ", z[i]);
    }
    printf("\n");

    // ------------------------
    // 调用 multvec 函数
    // ------------------------
    multvec(x, y, z, 4);
    printf("multvec result: ");
    for (int i = 0; i < 4; i++) {
        printf("%d ", z[i]);
    }
    printf("\n");

    // ------------------------
    // 关闭共享库，释放资源
    // ------------------------
    dlclose(handle);
    /*
     * dlclose 用于关闭共享库，释放相关资源
     * 参数是 dlopen 返回的句柄
     */

    return 0;
}
