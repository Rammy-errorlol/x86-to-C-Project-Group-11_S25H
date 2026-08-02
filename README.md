# x86-to-C-Project-Group-11_S25H

This project converts an array of unsigned 8-bit grayscale pixel values (0 to 255) into normalized 32-bit floats (0.0 to 1.0). The conversion itself is done in hand-written x86-64 assembly, and we checked the results against a C reference implementation to make sure everything lines up.

## Function Signature

```c
void imgCvtGrayInttoFloat(int height, int width, const unsigned char *input, float *output);
```

Each output pixel is calculated as `output[i] = (float)input[i] / 255.0f`.

**Calling convention:** Windows x64 (fastcall)
| Argument | Register |
|---|---|
| `height` | RCX |
| `width`  | RDX |
| `input`  | R8  |
| `output` | R9  |

Note that the `.asm` file is written for the Windows x64 calling convention, and `program.exe` is a Windows PE32+ binary. It won't run as is on Linux or macOS. If you want to test it on a non-Windows machine, you'd need to remap the argument registers to the System V AMD64 ABI (RDI, RSI, RDX, RCX) before assembling.

## Files

| File | Description |
|---|---|
| `imgCvtGrayInttoFloat..asm` | Assembly implementation of the conversion routine |
| `main.c` | Test and benchmark driver: runs the demo, checks correctness, and times performance |
| `imgCvtGrayInttoFloat.obj` | Pre-built object file (Windows) |
| `program.exe` | Pre-built executable (Windows) |

## Build Instructions (Windows / MASM-MinGW64)

```
nasm -f win64 imgCvtGrayInttoFloat..asm -o imgCvtGrayInttoFloat.obj OR
"C:\Program Files (x86)\SASM\NASM\nasm.exe" -f win64 "imgCvtGrayInttoFloat..asm" -o imgCvtGrayInttoFloat.obj
gcc main.c imgCvtGrayInttoFloat.obj -o program.exe -lm
program.exe
```

Adjust the assembler command depending on which tool you're using (NASM, MASM, or the SASM IDE mentioned in the source comments).

## Test Methodology

1. **Demo test**: a fixed 3x4 sample matrix gets converted and printed so you can check the output by hand against `pixel / 255`.
2. **Correctness check**: for each benchmark size, the assembly output is compared pixel by pixel against a C reference implementation (`c_imgCvtGrayInttoFloat`), allowing a tolerance of `1e-4`.
3. **Performance benchmark**: the assembly routine runs 30 times per input size, and we report the average execution time for 10x10, 100x100, and 1000x1000 inputs.

## Results

Placeholder numbers below. These came from porting the routine to the System V calling convention and running it on Linux, since the original binary is Windows only and couldn't be run in that environment. The assembly logic itself wasn't changed, only the argument registers were remapped to fit the Linux ABI. Swap these out with the output from your own Windows `program.exe` run before submitting, since timing depends a lot on the OS and hardware, and the rubric asks for an actual screenshot rather than typed numbers.

**Demo output (3x4 matrix):**
```
0.25 0.35 0.45 0.33
0.55 0.65 0.75 0.33
0.85 0.95 0.15 0.33
```
This matches what we'd expect (for example, 64/255 is about 0.25, and 242/255 is about 0.95).

**Correctness checks:**

| Size | Result |
|---|---|
| 10x10 | PASSED |
| 100x100 | PASSED |
| 1000x1000 | PASSED |

**Average execution time (30 runs each):**

| Size | Pixels | Avg. Time | Time per Pixel |
|---|---|---|---|
| 10x10 | 100 | ~0.0007 ms | ~7 ns |
| 100x100 | 10,000 | ~0.050 ms | ~5 ns |
| 1000x1000 | 1,000,000 | ~4.87 ms | ~4.9 ns |

**Screenshot of program output (with correctness check):**

<img width="2318" height="1004" alt="image" src="https://github.com/user-attachments/assets/7cbad04b-8cdc-49ba-b0c1-402fab7a8fdb" />


### Performance Analysis

Execution time scales pretty much linearly with pixel count, which makes sense given how the loop is written. The assembly processes exactly one pixel per iteration (`movzx`, then `cvtsi2ss`, `divss`, `movss`, then it bumps the pointers and loops again), and there's no batching or vectorization across multiple pixels. Since each iteration does the same fixed amount of work no matter how big the input is, total time ends up proportional to `height * width`. The time per pixel stays fairly flat, somewhere around 5 to 7 ns, across all three sizes we tested. That tells us there isn't much fixed overhead outside the loop weighing things down at small sizes, and no real memory or cache bottleneck showing up at the larger size either, at least not at this data volume. If we wanted to speed this up further, switching from scalar SIMD to packed SIMD instructions (like `cvtdq2ps` and `divps` to handle four pixels at once) would be the natural next step, since it should cut the per-pixel cost by processing several pixels per instruction instead of one.

### Demo Video

`[Insert link to your 5-10 minute video here, showing source code, compilation, and execution of the C and x86-64 program]`

## Design Notes

- Uses SSE scalar instructions (`cvtsi2ss`, `divss`, `movss`) to convert and scale each pixel.
- Processes one pixel per loop iteration, no SIMD or vectorized batch processing.
- The loop uses a simple decrement and branch (`dec` / `jnz`) with no unnecessary branching in the hot path.

## Known Limitations

- Builds for Windows only as provided, since it uses the Windows x64 calling convention.
- No SIMD vectorization yet. This could be optimized further with SSE or AVX to handle multiple pixels per iteration.
- No input validation beyond the `height * width <= 0` early return check.

## Group Members

- Joshua Carlos Samonte
- Ram Miguel Liwanag
