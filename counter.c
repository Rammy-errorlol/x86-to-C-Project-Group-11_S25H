#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// Pure C implementation of the uint8 to float image conversion
void imgCvtGrayInttoFloatC(int height, int width, const unsigned char *input, float *output) {
    int total_pixels = height * width;
    for (int i = 0; i < total_pixels; i++) {
        // Ratio equation: f = i / 255.0f
        output[i] = (float)input[i] / 255.0f;
    }
}

// Verification routine to check calculations against a reference loop
int verify_correctness(int height, int width, const float *test_out, const float *ref_out) {
    int total_pixels = height * width;
    for (int i = 0; i < total_pixels; i++) {
        if (fabsf(test_out[i] - ref_out[i]) > 1e-4f) {
            printf("  [Mismatch] Index %d: Test=%f, Ref=%f\n", i, test_out[i], ref_out[i]);
            return 0; // Failed match
        }
    }
    return 1; // Passed match
}

// Benchmark runner (executes 30 times and calculates average runtime)
void benchmark_c(int height, int width) {
    int total_pixels = height * width;
    unsigned char *input = (unsigned char *)malloc(total_pixels * sizeof(unsigned char));
    float *c_output = (float *)malloc(total_pixels * sizeof(float));
    float *ref_output = (float *)malloc(total_pixels * sizeof(float));

    if (!input || !c_output || !ref_output) {
        printf("Memory allocation failed for %dx%d!\n", height, width);
        return;
    }

    // Populate matrix with random uint8 pixel values [0, 255]
    for (int i = 0; i < total_pixels; i++) {
        input[i] = (unsigned char)(rand() % 256);
    }

    // Compute reference output
    imgCvtGrayInttoFloatC(height, width, input, ref_output);

    // Warmup & Correctness Verification
    imgCvtGrayInttoFloatC(height, width, input, c_output);
    int passed = verify_correctness(height, width, c_output, ref_output);

    printf("[%dx%d Matrix] Correctness Check: %s\n", height, width, passed ? "PASSED" : "FAILED");

    // Timing loop over 30 runs
    int runs = 30;
    clock_t total_clock_cycles = 0;

    for (int r = 0; r < runs; r++) {
        clock_t start = clock();
        imgCvtGrayInttoFloatC(height, width, input, c_output);
        clock_t end = clock();
        total_clock_cycles += (end - start);
    }

    // Calculate average time in milliseconds
    double avg_ms = ((double)total_clock_cycles / CLOCKS_PER_SEC) / runs * 1000.0;
    printf("[%dx%d Matrix] Average Pure C Time (30 runs): %.6f ms\n\n", height, width, avg_ms);

    free(input);
    free(c_output);
    free(ref_output);
}

int main(void) {
    srand((unsigned int)time(NULL));

    int height, width;

    printf("===================================================\n");
    printf("   GRAYSCALE INT-TO-FLOAT CONVERTER (PURE C)      \n");
    printf("===================================================\n\n");

    // 1. Command Line Input for Height and Width
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

    // 2. Command Line Input for Pixel Matrix Values
    printf("\nEnter %d pixel byte values (0-255):\n", total_pixels);
    for (int i = 0; i < total_pixels; i++) {
        int temp_val;
        if (scanf("%d", &temp_val) != 1) {
            printf("Invalid input byte value. Exiting.\n");
            free(user_input);
            free(user_output);
            return 1;
        }
        user_input[i] = (unsigned char)temp_val;
    }

    // Execute Pure C Conversion Routine
    imgCvtGrayInttoFloatC(height, width, user_input, user_output);

    // 3. Print Output Matrix
    printf("\nPure C Output Floating-Point Values:\n");
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            printf("%.2f ", user_output[i * width + j]);
        }
        printf("\n");
    }

    free(user_input);
    free(user_output);

    // 4. Run Standard Automated Benchmarks (30 runs for 10x10, 100x100, 1000x1000)
    printf("\n===================================================\n");
    printf("            PURE C BENCHMARK SUITE (30 RUNS)       \n");
    printf("===================================================\n");
    
    benchmark_c(10, 10);
    benchmark_c(100, 100);
    benchmark_c(1000, 1000);

    return 0;
}