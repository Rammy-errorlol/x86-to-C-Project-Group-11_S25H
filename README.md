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

> ⚠️ **Placeholder numbers below.** These were captured by porting the
> routine to the System V calling convention and running it on Linux,
> since the original binary is Windows-only and could not be executed
> in this environment. The assembly logic itself is unchanged — only
> the argument registers were remapped to match the Linux ABI. **Replace
> these with the output of your own Windows `program.exe` run before
> submitting**, since timing is sensitive to OS and hardware, and the
> rubric requires a real screenshot (see below), not just typed numbers.

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

| Size | Pixels | Avg. Time | Time/Pixel |
|---|---|---|---|
| 10×10 | 100 | ~0.0007 ms | ~7 ns |
| 100×100 | 10,000 | ~0.050 ms | ~5 ns |
| 1000×1000 | 1,000,000 | ~4.87 ms | ~4.9 ns |

**Screenshot of program output (with correctness check):**

`[Insert screenshot of your Windows program.exe run here, e.g. ![output](docs/output.png)]`

### Performance Analysis

Execution time scales approximately linearly with pixel count, which
is expected: the assembly loop processes exactly one pixel per
iteration (`movzx` → `cvtsi2ss` → `divss` → `movss`, then increment
pointers and loop), with no batching or vectorization across multiple
pixels. Since each iteration does a fixed, constant amount of work
regardless of input size, total time is proportional to `height *
width`. The per-pixel cost stays roughly flat (~5–7 ns/pixel) across
all three input sizes, which is consistent with there being no
significant fixed overhead outside the loop (e.g. no large setup cost
that would dominate at small sizes, and no cache/memory bottleneck
that would inflate cost at large sizes for this data volume). A likely
next optimization would be to use packed SIMD instructions (e.g.
`cvtdq2ps`/`divps` on 4 pixels at once) instead of scalar SIMD, which
could reduce the per-pixel cost by processing multiple pixels per
instruction.

### Demo Video

`[Insert link to your 5–10 minute video here, showing source code, compilation, and execution of the C and x86-64 program]`

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
