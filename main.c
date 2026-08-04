#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// Assembly function prototype
extern void imgCvtGrayInttoFloat(int height, int width, const unsigned char *input, float *output);

// Standard C reference conversion
void c_imgCvtGrayInttoFloat(int height, int width, const unsigned char *input, float *output) {
    int total = height * width;
    for (int i = 0; i < total; i++) {
        output[i] = (float)input[i] / 255.0f;
    }
}

// Verification function to compare C vs Assembly results
int verify_correctness(int height, int width, const float *asm_out, const float *c_out) {
    int total = height * width;
    for (int i = 0; i < total; i++) {
        if (fabsf(asm_out[i] - c_out[i]) > 1e-4f) {
            printf("  [Mismatch] Index %d: ASM=%f, C=%f\n", i, asm_out[i], c_out[i]);
            return 0; 
        }
    }
    return 1; 
}

// Benchmark runner (runs 30 times and calculates average time)
void benchmark_asm(int height, int width) {
    int total_pixels = height * width;
    unsigned char *input = (unsigned char *)malloc(total_pixels * sizeof(unsigned char));
    float *asm_output = (float *)malloc(total_pixels * sizeof(float));
    float *c_output = (float *)malloc(total_pixels * sizeof(float));

    if (!input || !asm_output || !c_output) {
        printf("Memory allocation failed for %dx%d!\n", height, width);
        return;
    }

    // Populate with random byte pixel values [0, 255]
    for (int i = 0; i < total_pixels; i++) {
        input[i] = (unsigned char)(rand() % 256);
    }

    // Reference calculation in C
    c_imgCvtGrayInttoFloat(height, width, input, c_output);

    // Assembly execution & verification
    imgCvtGrayInttoFloat(height, width, input, asm_output);
    int passed = verify_correctness(height, width, asm_output, c_output);

    printf("[%dx%d Matrix] Correctness Check: %s\n", height, width, passed ? "PASSED" : "FAILED");

    // Execution timing over 30 runs
    int runs = 30;
    clock_t total_clock_cycles = 0;

    for (int r = 0; r < runs; r++) {
        clock_t start = clock();
        imgCvtGrayInttoFloat(height, width, input, asm_output);
        clock_t end = clock();
        total_clock_cycles += (end - start);
    }

    double avg_ms = ((double)total_clock_cycles / CLOCKS_PER_SEC) / runs * 1000.0;
    printf("[%dx%d Matrix] Average ASM Execution Time (30 runs): %.6f ms\n\n", height, width, avg_ms);

    free(input);
    free(asm_output);
    free(c_output);
}

int main(void) {
    srand((unsigned int)time(NULL));

    int height, width;

    printf("GRAYSCALE INT-TO-FLOAT CONVERTER (x86 SIMD)\n");

    printf("Enter Image Height: ");
    if (scanf("%d", &height) != 1 || height <= 0) {
        printf("Invalid height entered. Exiting.\n");
        return 1;
    }

    printf("Enter Image Width: ");
    if (scanf("%d", &width) != 1 || width <= 0) {
        printf("Invalid width entered. Exiting.\n");
        return 1;
    }

    int total_pixels = height * width;
    unsigned char *user_input = (unsigned char *)malloc(total_pixels * sizeof(unsigned char));
    float *user_output = (float *)malloc(total_pixels * sizeof(float));

    printf("\nEnter %d pixel byte values (0-255):\n", total_pixels);
    for (int i = 0; i < total_pixels; i++) {
        int temp_val;
        if (scanf("%d", &temp_val) != 1) {
            printf("Invalid input byte value.\n");
            free(user_input);
            free(user_output);
            return 1;
        }
        user_input[i] = (unsigned char)temp_val;
    }

    // Call Assembly Routine
    imgCvtGrayInttoFloat(height, width, user_input, user_output);

    // Display Output Single Precision Float Matrix
    printf("\nAssembly Output Floating-Point Values:\n");
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            printf("%.2f ", user_output[i * width + j]);
        }
        printf("\n");
    }

    free(user_input);
    free(user_output);

    // Run Automated Benchmarks
    printf("BENCHMARK SUITE (30 RUNS)\n");
    benchmark_asm(10, 10);
    benchmark_asm(100, 100);
    benchmark_asm(1000, 1000);

    return 0;
}