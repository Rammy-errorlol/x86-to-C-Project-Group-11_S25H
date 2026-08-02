default rel

section .data
align 4
const255:
    dd 255.0

section .text
global imgCvtGrayInttoFloat

; void imgCvtGrayInttoFloat(
;     int height,
;     int width,
;     const unsigned char *input,
;     float *output
; );
;
; Windows x64 Calling Convention (SASM MinGW64)
; RCX = height
; RDX = width
; R8  = input pointer
; R9  = output pointer

imgCvtGrayInttoFloat:

    ; Compute total pixels = height * width
    mov eax, ecx
    imul eax, edx

    ; If total <= 0, return
    test eax, eax
    jle done

    ; Load constant 255.0f into XMM1
    movss xmm1, [const255]

loop_pixels:

    ; Load unsigned byte pixel
    movzx edx, byte [r8]

    ; Integer -> float
    cvtsi2ss xmm0, edx

    ; Divide by 255.0
    divss xmm0, xmm1

    ; Store float result
    movss [r9], xmm0

    ; Advance pointers
    inc r8
    add r9, 4

    ; Loop
    dec eax
    jnz loop_pixels

done:
    ret
    ret
