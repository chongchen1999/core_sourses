#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <vector>

// Template Vector class to replace the C-style vec_rec struct
template <typename T>
class Vector {
private:
    std::vector<T> data;

public:
    // Constructor to create a vector of specified length
    explicit Vector(size_t len) : data(len, T()) {}

    // Get vector element at index
    bool get_element(size_t index, T& dest) const {
        if (index >= data.size())
            return false;
        dest = data[index];
        return true;
    }

    // Return length of vector
    size_t length() const { return data.size(); }

    // Get pointer to the start of the vector data
    T* get_start() { return data.data(); }

    // Access vector data directly
    const T* get_start() const { return data.data(); }

    // Fill vector with random values
    void fill_random(T min, T max) {
        std::random_device rd;
        std::mt19937 gen(rd());

        if constexpr (std::is_integral<T>::value) {
            std::uniform_int_distribution<T> dist(min, max);
            for (auto& val : data) {
                val = dist(gen);
            }
        } else {
            std::uniform_real_distribution<T> dist(min, max);
            for (auto& val : data) {
                val = dist(gen);
            }
        }
    }

    // Get element directly
    T& operator[](size_t index) { return data[index]; }

    // Get element directly (const version)
    const T& operator[](size_t index) const { return data[index]; }
};

// Template function type for combine operations
template <typename T>
using CombineFunction = std::function<void(const Vector<T>&, T&, char)>;

// Combine implementations - each is now a template function

// Original implementation
template <typename T>
void combine1(const Vector<T>& v, T& dest, char op) {
    // Initialize with identity value based on operation
    dest = (op == '+') ? T(0) : T(1);

    for (size_t i = 0; i < v.length(); i++) {
        T val;
        v.get_element(i, val);

        // Apply operation
        switch (op) {
        case '+':
            dest = dest + val;
            break;
        case '*':
            dest = dest * val;
            break;
        }
    }
}

// Eliminating Loop Inefficiencies
template <typename T>
void combine2(const Vector<T>& v, T& dest, char op) {
    // Initialize with identity value based on operation
    dest = (op == '+') ? T(0) : T(1);

    size_t length = v.length();
    for (size_t i = 0; i < length; i++) {
        T val;
        v.get_element(i, val);

        switch (op) {
        case '+':
            dest = dest + val;
            break;
        case '*':
            dest = dest * val;
            break;
        }
    }
}

// Reducing Procedure Calls
template <typename T>
void combine3(const Vector<T>& v, T& dest, char op) {
    // Initialize with identity value based on operation
    dest = (op == '+') ? T(0) : T(1);

    size_t length = v.length();
    const T* data = v.get_start();

    for (size_t i = 0; i < length; i++) {
        switch (op) {
        case '+':
            dest = dest + data[i];
            break;
        case '*':
            dest = dest * data[i];
            break;
        }
    }
}

// Eliminating Unneeded Memory References
template <typename T>
void combine4(const Vector<T>& v, T& dest, char op) {
    size_t length = v.length();
    const T* data = v.get_start();

    // Use accumulator to avoid repeated memory references
    T acc = (op == '+') ? T(0) : T(1);

    for (size_t i = 0; i < length; i++) {
        switch (op) {
        case '+':
            acc = acc + data[i];
            break;
        case '*':
            acc = acc * data[i];
            break;
        }
    }

    dest = acc;
}

// 2 x 1 loop unrolling
template <typename T>
void combine5(const Vector<T>& v, T& dest, char op) {
    size_t length = v.length();
    size_t limit = length - 1;
    const T* data = v.get_start();

    // Use accumulator
    T acc = (op == '+') ? T(0) : T(1);

    // Combine 2 elements at a time
    for (size_t i = 0; i < limit; i += 2) {
        switch (op) {
        case '+':
            acc = (acc + data[i]) + data[i + 1];
            break;
        case '*':
            acc = (acc * data[i]) * data[i + 1];
            break;
        }
    }

    // Handle remaining elements
    for (size_t i = limit - (limit % 2); i < length; i++) {
        switch (op) {
        case '+':
            acc = acc + data[i];
            break;
        case '*':
            acc = acc * data[i];
            break;
        }
    }

    dest = acc;
}

// 2 x 2 loop unrolling
template <typename T>
void combine6(const Vector<T>& v, T& dest, char op) {
    size_t length = v.length();
    size_t limit = length - 1;
    const T* data = v.get_start();

    // Use two accumulators
    T acc0 = (op == '+') ? T(0) : T(1);
    T acc1 = (op == '+') ? T(0) : T(1);

    // Combine 2 elements at a time with 2 accumulators
    for (size_t i = 0; i < limit; i += 2) {
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

    // Handle remaining elements
    for (size_t i = limit - (limit % 2); i < length; i++) {
        switch (op) {
        case '+':
            acc0 = acc0 + data[i];
            break;
        case '*':
            acc0 = acc0 * data[i];
            break;
        }
    }

    // Combine accumulators
    switch (op) {
    case '+':
        dest = acc0 + acc1;
        break;
    case '*':
        dest = acc0 * acc1;
        break;
    }
}

// 2 x 1a loop unrolling
template <typename T>
void combine7(const Vector<T>& v, T& dest, char op) {
    size_t length = v.length();
    size_t limit = length - 1;
    const T* data = v.get_start();

    // Use accumulator
    T acc = (op == '+') ? T(0) : T(1);

    // Combine 2 elements at a time
    for (size_t i = 0; i < limit; i += 2) {
        switch (op) {
        case '+':
            acc = acc + (data[i] + data[i + 1]);
            break;
        case '*':
            acc = acc * (data[i] * data[i + 1]);
            break;
        }
    }

    // Handle remaining elements
    for (size_t i = limit - (limit % 2); i < length; i++) {
        switch (op) {
        case '+':
            acc = acc + data[i];
            break;
        case '*':
            acc = acc * data[i];
            break;
        }
    }

    dest = acc;
}

// Performance testing function
template <typename T>
double test_performance(const Vector<T>& v, int test_count,
                        CombineFunction<T> combine_func, char op) {
    T result;
    size_t total_elements = v.length() * test_count;

    // Warm up cache
    combine_func(v, result, op);

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < test_count; i++) {
        combine_func(v, result, op);
    }

    auto end = std::chrono::high_resolution_clock::now();

    // Calculate elapsed time in seconds
    std::chrono::duration<double> elapsed = end - start;

    // Get CPU frequency (adjust this value for your CPU)
    double cpu_freq_ghz = 2.6; // Example: 2.6 GHz
    double cpu_freq_hz = cpu_freq_ghz * 1e9;

    // Calculate cycles per element
    double cycles = elapsed.count() * cpu_freq_hz;
    double cpe = cycles / total_elements;

    return cpe;
}

int main() {
    const size_t vec_len = 1000000; // 1 million elements
    const int test_count = 100;     // Number of test runs

    // Create vectors for int and float types
    Vector<int> v_int(vec_len);
    Vector<float> v_float(vec_len);

    // Fill vectors with random data
    v_int.fill_random(0, 99);
    v_float.fill_random(0.0f, 100.0f);

    // Vector of combine functions for integer operations
    std::vector<std::pair<CombineFunction<int>, std::string>>
        int_combine_functions = {
            {combine1<int>, "combine1 (original)"},
            {combine2<int>, "combine2 (length caching)"},
            {combine3<int>, "combine3 (procedure call reducing)"},
            {combine4<int>, "combine4 (memory access reducing)"},
            {combine5<int>, "combine5 (2x1 loop unrolling)"},
            {combine6<int>, "combine6 (2x2 loop unrolling)"},
            {combine7<int>, "combine7 (2x1a loop unrolling)"}};

    // Vector of combine functions for float operations
    std::vector<std::pair<CombineFunction<float>, std::string>>
        float_combine_functions = {
            {combine1<float>, "combine1 (original)"},
            {combine2<float>, "combine2 (length caching)"},
            {combine3<float>, "combine3 (procedure call reducing)"},
            {combine4<float>, "combine4 (memory access reducing)"},
            {combine5<float>, "combine5 (2x1 loop unrolling)"},
            {combine6<float>, "combine6 (2x2 loop unrolling)"},
            {combine7<float>, "combine7 (2x1a loop unrolling)"}};

    // Test cases: operation definitions and names
    struct TestCase {
        std::string name;
        char op;
    };

    std::vector<TestCase> test_cases = {{"addition", '+'},
                                        {"multiplication", '*'}};

    // Test all integer combinations
    std::cout << "\n=== Testing Integer Operations ===\n";
    for (const auto& [func, name] : int_combine_functions) {
        std::cout << "\n=== Testing function: " << name << " ===\n";

        for (const auto& test_case : test_cases) {
            std::cout << "\nTesting integer " << test_case.name << ":\n";

            double cpe =
                test_performance(v_int, test_count, func, test_case.op);
            std::cout << "CPE: " << std::fixed << std::setprecision(2) << cpe
                      << " cycles/element\n";
        }
    }

    // Test all float combinations
    std::cout << "\n=== Testing Float Operations ===\n";
    for (const auto& [func, name] : float_combine_functions) {
        std::cout << "\n=== Testing function: " << name << " ===\n";

        for (const auto& test_case : test_cases) {
            std::cout << "\nTesting float " << test_case.name << ":\n";

            double cpe =
                test_performance(v_float, test_count, func, test_case.op);
            std::cout << "CPE: " << std::fixed << std::setprecision(2) << cpe
                      << " cycles/element\n";
        }
    }

    return 0;
}