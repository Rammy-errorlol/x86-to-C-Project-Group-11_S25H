default rel

section .data
align 4
; Pre-calculate 1.0 / 255.0f for multiplication 
inv255: dd 0.0039215688

section .text
global imgCvtGrayInttoFloat
imgCvtGrayInttoFloat:
    ; Calculate total_pixels = height * width
    mov eax, ecx                    ; eax = height
    imul eax, edx                   ; eax = total_pixels

    test eax, eax
    jle .done

    ; Load 1/255.0f into XMM1
    movss xmm1, [inv255]

.loop_start:
    ; Read byte pixel
    movzx r10d, byte [r8]

    ; Scalar int -> float
    cvtsi2ss xmm0, r10d

    ; Scalar multiplication (faster than divss)
    mulss xmm0, xmm1

    ; Write float result
    movss [r9], xmm0

    ; Advance pointers
    inc r8                          ; byte pointer += 1
    add r9, 4                       ; float pointer += 4

    dec eax
    jnz .loop_start

.done:
    ret