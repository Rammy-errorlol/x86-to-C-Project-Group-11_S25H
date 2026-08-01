#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// External assembly declaration
extern void imgCvtGrayInttoFloat(int height, int width, const unsigned char *input, float *output);

// C reference conversion for correctness checking
void c_imgCvtGrayInttoFloat(int height, int width, const unsigned char *input, float *output) {
    int total = height * width;
    for (int i = 0; i < total; i++) {
        output[i] = (float)input[i] / 255.0f;
    }
}

// Correctness Verification Routine
int verify_correctness(int height, int width, const float *asm_out, const float *c_out) {
    int total = height * width;
    for (int i = 0; i < total; i++) {
        if (fabsf(asm_out[i] - c_out[i]) > 1e-4f) {
            printf("Mismatch at index %d: ASM=%f, C=%f\n", i, asm_out[i], c_out[i]);
            return 0; // Fail
        }
    }
    return 1; // Pass
}

// Benchmark execution time (30 runs average)
void benchmark_asm(int height, int width) {
    int total_pixels = height * width;
    unsigned char *input = (unsigned char *)malloc(total_pixels * sizeof(unsigned char));
    float *asm_output = (float *)malloc(total_pixels * sizeof(float));
    float *c_output = (float *)malloc(total_pixels * sizeof(float));

    // Populate with random byte values [0, 255]
    for (int i = 0; i < total_pixels; i++) {
        input[i] = (unsigned char)(rand() % 256);
    }

    // Compute reference output via C
    c_imgCvtGrayInttoFloat(height, width, input, c_output);

    // Warmup & Correctness Check
    imgCvtGrayInttoFloat(height, width, input, asm_output);
    if (!verify_correctness(height, width, asm_output, c_output)) {
        printf("[%dx%d] Correctness Check: FAILED!\n", height, width);
    } else {
        printf("[%dx%d] Correctness Check: PASSED!\n", height, width);
    }

    // Benchmark 30 runs
    int runs = 30;
    clock_t total_time = 0;

    for (int r = 0; r < runs; r++) {
        clock_t start = clock();
        imgCvtGrayInttoFloat(height, width, input, asm_output);
        clock_t end = clock();
        total_time += (end - start);
    }

    double avg_ms = ((double)total_time / CLOCKS_PER_SEC) / runs * 1000.0;
    printf("[%dx%d] Average Execution Time (%d runs): %.6f ms\n\n", height, width, runs, avg_ms);

    free(input);
    free(asm_output);
    free(c_output);
}

int main() {
    srand((unsigned int)time(NULL));

    printf("=== DEMO EXAMPLE TEST ===\n");
    int demo_h = 3, demo_w = 4;
    unsigned char demo_input[] = {
         64,  89, 114, 84,
        140, 166, 191, 84,
        216, 242,  38, 84
    };
    float demo_output[12];

    imgCvtGrayInttoFloat(demo_h, demo_w, demo_input, demo_output);

    printf("Assembly Output Matrix (3x4):\n");
    for (int i = 0; i < demo_h; i++) {
        for (int j = 0; j < demo_w; j++) {
            printf("%.2f ", demo_output[i * demo_w + j]);
        }
        printf("\n");
    }
    printf("\n");

    printf("=== PERFORMANCE BENCHMARKS ===\n");
    benchmark_asm(10, 10);
    benchmark_asm(100, 100);
    benchmark_asm(1000, 1000);

    return 0;
}