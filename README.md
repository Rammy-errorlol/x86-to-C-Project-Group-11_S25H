# x86-to-C-Project-Group-11_S25H

Converts an array of unsigned 8-bit grayscale pixel values (0–255) into
normalized 32-bit floats (0.0–1.0), implemented in hand-written x86-64
assembly and validated against a C reference implementation.

## Function Signature

```c
void imgCvtGrayInttoFloat(int height, int width, const unsigned char *input, float *output);
```

Each output pixel is computed as `output[i] = (float)input[i] / 255.0f`.

**Calling convention:** Windows x64 (fastcall)
| Argument | Register |
|---|---|
| `height` | RCX |
| `width`  | RDX |
| `input`  | R8  |
| `output` | R9  |

> Note: the current `.asm` targets the **Windows x64 calling convention**
> and the provided `program.exe` is a Windows PE32+ binary. It will not
> run as-is on Linux/macOS. To test on a non-Windows machine, the
> argument registers must be re-mapped to the System V AMD64 ABI
> (RDI/RSI/RDX/RCX) before assembling.

## Files

| File | Description |
|---|---|
| `imgCvtGrayInttoFloat..asm` | Assembly implementation of the conversion routine |
| `main.c` | Test/benchmark driver: demo run, correctness check, and timing benchmarks |
| `imgCvtGrayInttoFloat.obj` | Pre-built object file (Windows) |
| `program.exe` | Pre-built executable (Windows) |

## Build Instructions (Windows / MASM-MinGW64)

```
nasm -f win64 imgCvtGrayInttoFloat..asm -o imgCvtGrayInttoFloat.obj
gcc main.c imgCvtGrayInttoFloat.obj -o program.exe -lm
program.exe
```

(Adjust the assembler invocation to match whichever tool you used —
NASM, MASM, or the SASM IDE mentioned in the source comments.)

## Test Methodology

1. **Demo test** — a fixed 3×4 sample matrix is converted and printed so
   the output can be checked by hand against `pixel / 255`.
2. **Correctness check** — for each benchmark size, the assembly output
   is compared element-by-element against a C reference implementation
   (`c_imgCvtGrayInttoFloat`), with a tolerance of `1e-4`.
3. **Performance benchmark** — the assembly routine is run 30 times per
   input size and the average execution time is reported, for sizes
   10×10, 100×100, and 1000×1000.

## Results

Results below were captured by porting the routine to the System V
calling convention and running it on Linux, since the original binary
is Windows-only and could not be executed in this environment. The
assembly logic itself is unchanged — only the argument registers were
remapped to match the Linux ABI. **These numbers are for reference
only; replace them with the output of your own Windows `program.exe`
run before submitting**, since timing is sensitive to OS and hardware.

**Demo output (3×4 matrix):**
```
0.25 0.35 0.45 0.33
0.55 0.65 0.75 0.33
0.85 0.95 0.15 0.33
```
Matches expected values (e.g. 64/255 ≈ 0.25, 242/255 ≈ 0.95).

**Correctness checks:**

| Size | Result |
|---|---|
| 10×10 | PASSED |
| 100×100 | PASSED |
| 1000×1000 | PASSED |

**Average execution time (30 runs each):**

| Size | Pixels | Avg. Time |
|---|---|---|
| 10×10 | 100 | ~0.0007 ms |
| 100×100 | 10,000 | ~0.050 ms |
| 1000×1000 | 1,000,000 | ~4.87 ms |

Execution time scales approximately linearly with pixel count, which
is expected for a simple single-pixel-per-iteration loop with no
vectorization.

## Design Notes

- Uses SSE scalar instructions (`cvtsi2ss`, `divss`, `movss`) to convert
  and scale each pixel.
- Processes one pixel per loop iteration (no SIMD/vectorized batch
  processing).
- Loop uses a simple decrement-and-branch (`dec` / `jnz`) with no
  unnecessary branching inside the hot path.

## Known Limitations

- Windows-only build as provided (Windows x64 calling convention).
- No SIMD vectorization — could be optimized further with SSE/AVX to
  process multiple pixels per iteration.
- No input validation beyond the `height * width <= 0` early-return
  check.

## Group Members

- [Add names / student IDs here]
