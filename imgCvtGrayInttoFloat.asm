default rel

section .data
align 4
const255: dd 255.0                  ; 32-bit float constant (255.0f)

section .text
global imgCvtGrayInttoFloat

imgCvtGrayInttoFloat:
    ; 1. Calculate total_pixels = height * width
    mov eax, ecx                    ; eax = height (from RCX)
    imul eax, edx                   ; eax = height * width (from RDX)

    ; 2. Check if total pixels <= 0
    test eax, eax
    jle .done

    ; 3. Load 255.0f scalar into XMM1
    movss xmm1, [const255]

.loop_start:
    ; 4. Read unsigned 8-bit byte from R8 pointer (zero-extended into R10D)
    movzx r10d, byte [r8]

    ; 5. Scalar SIMD conversion: int -> single precision float (XMM0)
    cvtsi2ss xmm0, r10d

    ; 6. Scalar SIMD division: xmm0 = pixel / 255.0f
    divss xmm0, xmm1

    ; 7. Write float result to memory address at R9 pointer
    movss [r9], xmm0

    ; 8. Advance pointers
    inc r8                          ; byte pointer ++
    add r9, 4                       ; float pointer += 4 bytes

    ; 9. Loop decrement
    dec eax
    jnz .loop_start

.done:
    ret