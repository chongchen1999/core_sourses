#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N (1L << 24) // 16 million elements

void clear_array(long* dest, long n) {
    long i;
    for (i = 0; i < n; i++) {
        dest[i] = 0;
    }
}

void write_read(long* src, long* dest, long n) {
    long cnt = n;
    long val = 0;
    while (cnt--) {
        *dest = val;
        val = (*src) + 1;
    }
}

void copy_array(long* src, long* dest, long n) {
    long i;
    for (i = 0; i < n; i++) {
        dest[i] = src[i];
    }
}

int main() {
    long* a = malloc((N + 5) * sizeof(long));
    if (!a) {
        perror("malloc failed");
        return 1;
    }

    clear_array(a, N);
    clock_t start = clock();
    copy_array(a + 1, a, N);
    clock_t end = clock();
    double time1 = (double)(end - start) / CLOCKS_PER_SEC;
    double cpe1 = time1 / N * 1e9; // Convert to nanoseconds per element

    clear_array(a, N);
    start = clock();
    copy_array(a, a + 1, N);
    end = clock();
    double time2 = (double)(end - start) / CLOCKS_PER_SEC;
    double cpe2 = time2 / N * 1e9;

    clear_array(a, N);
    start = clock();
    copy_array(a, a, N);
    end = clock();
    double time3 = (double)(end - start) / CLOCKS_PER_SEC;
    double cpe3 = time3 / N * 1e9;

    printf("copy_array(a+1, a, N):\n");
    printf("  Time: %.2f sec\n", time1);
    printf("  CPE:  %.2f ns\n", cpe1);

    printf("\ncopy_array(a, a+1, N):\n");
    printf("  Time: %.2f sec\n", time2);
    printf("  CPE:  %.2f ns\n", cpe2);

    printf("\ncopy_array(a, a, N):\n");
    printf("  Time: %.2f sec\n", time3);
    printf("  CPE:  %.2f ns\n", cpe3);

    // printf("\nPerformance difference: %.2fx\n", cpe2 / cpe1);

    free(a);
    return 0;
}