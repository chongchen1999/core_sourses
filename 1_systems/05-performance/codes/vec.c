#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

/* Create abstract data type for vector */
typedef struct {
    long len;
    void* data;
} vec_rec, *vec_ptr;

// Function type that takes vector, destination, and operation type params
typedef void (*func_ptr)(vec_ptr, void*, int, int, char);

/* Create vector of specified length */
vec_ptr new_vec(long len, int is_float) {
    vec_ptr result = (vec_ptr)malloc(sizeof(vec_rec));
    void* data = NULL;
    if (!result)
        return NULL;

    result->len = len;
    if (len > 0) {
        if (is_float) {
            data = (float*)calloc(len, sizeof(float));
        } else {
            data = (int*)calloc(len, sizeof(int));
        }
        if (!data) {
            free((void*)result);
            return NULL;
        }
    }
    result->data = data;
    return result;
}

/* Retrieve vector element */
int get_vec_element(vec_ptr v, long index, void* dest, int is_float) {
    if (index < 0 || index >= v->len)
        return 0;
    if (is_float) {
        *((float*)dest) = ((float*)v->data)[index];
    } else {
        *((int*)dest) = ((int*)v->data)[index];
    }
    return 1;
}

/* Return length of vector */
long vec_length(vec_ptr v) {
    return v->len;
}

/* Helper function to get vector start pointer */
void* get_vec_start(vec_ptr v) {
    return v->data;
}

/* Combined functions with runtime parameters instead of preprocessor
 * definitions */
void combine1(vec_ptr v, void* dest, int is_float, int ident_val, char op) {
    if (is_float) {
        float* dest_ = (float*)dest;
        *dest_ = (float)ident_val; // Convert the ident_val to float
        for (long i = 0; i < vec_length(v); i++) {
            float val;
            get_vec_element(v, i, &val, is_float);

            // Apply the operation based on the op parameter
            switch (op) {
            case '+':
                *dest_ = *dest_ + val;
                break;
            case '*':
                *dest_ = *dest_ * val;
                break;
                // Add other operations as needed
            }
        }
    } else {
        int* dest_ = (int*)dest;
        *dest_ = ident_val;
        for (long i = 0; i < vec_length(v); i++) {
            int val;
            get_vec_element(v, i, &val, is_float);

            // Apply the operation based on the op parameter
            switch (op) {
            case '+':
                *dest_ = *dest_ + val;
                break;
            case '*':
                *dest_ = *dest_ * val;
                break;
                // Add other operations as needed
            }
        }
    }
}

// Eliminating Loop Inefficiencies
void combine2(vec_ptr v, void* dest, int is_float, int ident_val, char op) {
    if (is_float) {
        float* dest_ = (float*)dest;
        *dest_ = (float)ident_val;
        long length = vec_length(v);
        for (long i = 0; i < length; i++) {
            float val;
            get_vec_element(v, i, &val, is_float);

            switch (op) {
            case '+':
                *dest_ = *dest_ + val;
                break;
            case '*':
                *dest_ = *dest_ * val;
                break;
            }
        }
    } else {
        int* dest_ = (int*)dest;
        *dest_ = ident_val;
        long length = vec_length(v);
        for (long i = 0; i < length; i++) {
            int val;
            get_vec_element(v, i, &val, is_float);

            switch (op) {
            case '+':
                *dest_ = *dest_ + val;
                break;
            case '*':
                *dest_ = *dest_ * val;
                break;
            }
        }
    }
}

// Reducing Procedure Calls
void combine3(vec_ptr v, void* dest, int is_float, int ident_val, char op) {
    if (is_float) {
        float* dest_ = (float*)dest;
        *dest_ = (float)ident_val;
        long length = vec_length(v);
        float* data = (float*)v->data;
        for (long i = 0; i < length; i++) {
            switch (op) {
            case '+':
                *dest_ = *dest_ + data[i];
                break;
            case '*':
                *dest_ = *dest_ * data[i];
                break;
            }
        }
    } else {
        int* dest_ = (int*)dest;
        *dest_ = ident_val;
        long length = vec_length(v);
        int* data = (int*)v->data;
        for (long i = 0; i < length; i++) {
            switch (op) {
            case '+':
                *dest_ = *dest_ + data[i];
                break;
            case '*':
                *dest_ = *dest_ * data[i];
                break;
            }
        }
    }
}

// Eliminating Unneeded Memory References
void combine4(vec_ptr v, void* dest, int is_float, int ident_val, char op) {
    if (is_float) {
        float* dest_ = (float*)dest;
        long length = vec_length(v);
        float* data = (float*)v->data;
        float acc = (float)ident_val;
        for (long i = 0; i < length; i++) {
            switch (op) {
            case '+':
                acc = acc + data[i];
                break;
            case '*':
                acc = acc * data[i];
                break;
            }
        }
        *dest_ = acc;
    } else {
        int* dest_ = (int*)dest;
        long length = vec_length(v);
        int* data = (int*)v->data;
        int acc = ident_val;
        for (long i = 0; i < length; i++) {
            switch (op) {
            case '+':
                acc = acc + data[i];
                break;
            case '*':
                acc = acc * data[i];
                break;
            }
        }
        *dest_ = acc;
    }
}

/* 2 x 1 loop unrolling */
void combine5(vec_ptr v, void* dest, int is_float, int ident_val, char op) {
    if (is_float) {
        float* dest_ = (float*)dest;
        long length = vec_length(v);
        long limit = length - 1;
        float* data = (float*)get_vec_start(v);
        float acc = (float)ident_val;

        /* Combine 2 elements at a time */
        for (long i = 0; i < limit; i += 2) {
            switch (op) {
            case '+':
                acc = (acc + data[i]) + data[i + 1];
                break;
            case '*':
                acc = (acc * data[i]) * data[i + 1];
                break;
            }
        }

        /* Finish any remaining elements */
        for (long i = limit - (limit % 2); i < length; i++) {
            switch (op) {
            case '+':
                acc = acc + data[i];
                break;
            case '*':
                acc = acc * data[i];
                break;
            }
        }
        *dest_ = acc;
    } else {
        int* dest_ = (int*)dest;
        long length = vec_length(v);
        long limit = length - 1;
        int* data = (int*)get_vec_start(v);
        int acc = ident_val;

        /* Combine 2 elements at a time */
        for (long i = 0; i < limit; i += 2) {
            switch (op) {
            case '+':
                acc = (acc + data[i]) + data[i + 1];
                break;
            case '*':
                acc = (acc * data[i]) * data[i + 1];
                break;
            }
        }

        /* Finish any remaining elements */
        for (long i = limit - (limit % 2); i < length; i++) {
            switch (op) {
            case '+':
                acc = acc + data[i];
                break;
            case '*':
                acc = acc * data[i];
                break;
            }
        }
        *dest_ = acc;
    }
}

/* 2 x 2 loop unrolling */
void combine6(vec_ptr v, void* dest, int is_float, int ident_val, char op) {
    if (is_float) {
        float* dest_ = (float*)dest;
        long length = vec_length(v);
        long limit = length - 1;
        float* data = (float*)get_vec_start(v);
        float acc0 = (float)ident_val;
        float acc1 = (float)ident_val;

        /* Combine 2 elements at a time with 2 accumulators */
        for (long i = 0; i < limit; i += 2) {
            switch (op) {
            case '+':
                acc0 = acc0 + data[i];
                acc1 = acc1 + data[i + 1];
                break;
            case '*':
                acc0 = acc0 * data[i];
                acc1 = acc1 * data[i + 1];
                break;
            }
        }

        /* Finish any remaining elements */
        for (long i = limit - (limit % 2); i < length; i++) {
            switch (op) {
            case '+':
                acc0 = acc0 + data[i];
                break;
            case '*':
                acc0 = acc0 * data[i];
                break;
            }
        }

        /* Combine the accumulators */
        switch (op) {
        case '+':
            *dest_ = acc0 + acc1;
            break;
        case '*':
            *dest_ = acc0 * acc1;
            break;
        }
    } else {
        int* dest_ = (int*)dest;
        long length = vec_length(v);
        long limit = length - 1;
        int* data = (int*)get_vec_start(v);
        int acc0 = ident_val;
        int acc1 = ident_val;

        /* Combine 2 elements at a time with 2 accumulators */
        for (long i = 0; i < limit; i += 2) {
            switch (op) {
            case '+':
                acc0 = acc0 + data[i];
                acc1 = acc1 + data[i + 1];
                break;
            case '*':
                acc0 = acc0 * data[i];
                acc1 = acc1 * data[i + 1];
                break;
            }
        }

        /* Finish any remaining elements */
        for (long i = limit - (limit % 2); i < length; i++) {
            switch (op) {
            case '+':
                acc0 = acc0 + data[i];
                break;
            case '*':
                acc0 = acc0 * data[i];
                break;
            }
        }

        /* Combine the accumulators */
        switch (op) {
        case '+':
            *dest_ = acc0 + acc1;
            break;
        case '*':
            *dest_ = acc0 * acc1;
            break;
        }
    }
}

/* 2 x 1a loop unrolling */
void combine7(vec_ptr v, void* dest, int is_float, int ident_val, char op) {
    if (is_float) {
        float* dest_ = (float*)dest;
        long length = vec_length(v);
        long limit = length - 1;
        float* data = (float*)get_vec_start(v);
        float acc = (float)ident_val;

        /* Combine 2 elements at a time */
        for (long i = 0; i < limit; i += 2) {
            switch (op) {
            case '+':
                acc = acc + (data[i] + data[i + 1]);
                break;
            case '*':
                acc = acc * (data[i] * data[i + 1]);
                break;
            }
        }

        /* Finish any remaining elements */
        for (long i = limit - (limit % 2); i < length; i++) {
            switch (op) {
            case '+':
                acc = acc + data[i];
                break;
            case '*':
                acc = acc * data[i];
                break;
            }
        }
        *dest_ = acc;
    } else {
        int* dest_ = (int*)dest;
        long length = vec_length(v);
        long limit = length - 1;
        int* data = (int*)get_vec_start(v);
        int acc = ident_val;

        /* Combine 2 elements at a time */
        for (long i = 0; i < limit; i += 2) {
            switch (op) {
            case '+':
                acc = acc + (data[i] + data[i + 1]);
                break;
            case '*':
                acc = acc * (data[i] * data[i + 1]);
                break;
            }
        }

        /* Finish any remaining elements */
        for (long i = limit - (limit % 2); i < length; i++) {
            switch (op) {
            case '+':
                acc = acc + data[i];
                break;
            case '*':
                acc = acc * data[i];
                break;
            }
        }
        *dest_ = acc;
    }
}

/* Performance testing function - now accepts parameters for the operation */
double test_performance(vec_ptr v, int test_count, func_ptr combine,
                        int is_float, int ident_val, char op) {
    struct timeval start, end;

    // Create appropriate result variable based on data type
    union {
        int i;
        float f;
    } result;

    long total_elements = v->len * test_count;

    // Warm up cache
    combine(v, &result, is_float, ident_val, op);

    gettimeofday(&start, NULL);
    for (int i = 0; i < test_count; i++) {
        combine(v, &result, is_float, ident_val, op);
    }
    gettimeofday(&end, NULL);

    double elapsed =
        (end.tv_sec - start.tv_sec) * 1000000.0 + (end.tv_usec - start.tv_usec);

    // Convert to seconds
    elapsed /= 1000000.0;

    // Get CPU frequency (adjust this value for your CPU)
    double cpu_freq_ghz = 2.6; // Example: 2.6 GHz
    double cpu_freq_hz = cpu_freq_ghz * 1e9;

    // Calculate cycles per element
    double cycles = elapsed * cpu_freq_hz;
    double cpe = cycles / total_elements;

    return cpe;
}

int main() {
    const long vec_len = 1000000; // 1 million elements
    const int test_count = 100;   // Number of test runs

    // Array of combine functions to test
    func_ptr combine_functions[] = {&combine1, &combine2, &combine3, &combine4,
                                    &combine5, &combine6, &combine7};
    const char* combine_names[] = {"combine1 (original)",
                                   "combine2 (length caching)",
                                   "combine3 (procedure call reducing)",
                                   "combine4 (memory access reducing)",
                                   "combine5 (2x1 loop unrolling)",
                                   "combine6 (2x2 loop unrolling)",
                                   "combine7 (2x1a loop unrolling)"};
    int num_combine_funcs =
        sizeof(combine_functions) / sizeof(combine_functions[0]);

    // Test cases: operation definitions and names
    struct {
        const char* name;
        int is_float;
        char op;
        int ident_val;
    } test_cases[] = {{"integer addition", 0, '+', 0},
                      {"integer multiplication", 0, '*', 1},
                      {"float addition", 1, '+', 0},
                      {"float multiplication", 1, '*', 1}};
    int num_test_cases = sizeof(test_cases) / sizeof(test_cases[0]);

    // Create and initialize vectors
    vec_ptr v_int = new_vec(vec_len, 0);
    vec_ptr v_float = new_vec(vec_len, 1);
    if (!v_int || !v_float) {
        fprintf(stderr, "Failed to allocate vectors\n");
        if (v_int) {
            free(v_int->data);
            free(v_int);
        }
        if (v_float) {
            free(v_float->data);
            free(v_float);
        }
        return 1;
    }

    // Fill vectors with random data
    srand(time(NULL));
    for (long i = 0; i < vec_len; i++) {
        ((int*)v_int->data)[i] = rand() % 100;
        ((float*)v_float->data)[i] = (float)rand() / RAND_MAX * 100.0;
    }

    // Test all combinations
    for (int func_idx = 0; func_idx < num_combine_funcs; func_idx++) {
        printf("\n=== Testing function: %s ===\n", combine_names[func_idx]);

        for (int case_idx = 0; case_idx < num_test_cases; case_idx++) {
            // Get current test case parameters
            int is_float = test_cases[case_idx].is_float;
            char op = test_cases[case_idx].op;
            int ident_val = test_cases[case_idx].ident_val;

            // Select appropriate vector
            vec_ptr current_vec = is_float ? v_float : v_int;

            printf("\nTesting %s:\n", test_cases[case_idx].name);

            // Pass the parameters to the performance test function
            double cpe = test_performance(current_vec, test_count,
                                          combine_functions[func_idx], is_float,
                                          ident_val, op);

            printf("CPE: %.2f cycles/element\n", cpe);
        }
    }

    // Clean up
    free(v_int->data);
    free(v_int);
    free(v_float->data);
    free(v_float);

    return 0;
}