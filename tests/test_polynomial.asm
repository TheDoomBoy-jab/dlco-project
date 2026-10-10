; ==============================================================================
; Test Case: Polynomial Evaluation P(x) = 3*x^2 + 5*x + 7
; Author: Aahan Nayak
; Roll No: 2501AI21
;
; Description:
;   Computes 3*x^2 + 5*x + 7 for x = 4
;   Expected result: 3*(16) + 5*(4) + 7 = 48 + 20 + 7 = 75 (0x0000004B)
; ==============================================================================

        ldc x
        ldnl 0          ; A = x (4)

        ; Compute x^2 (using mul)
        ldc x
        ldnl 0          ; B = 4, A = 4
        mul             ; A = 16 (x^2)

        ; Multiply by 3
        ldc 3           ; B = 16, A = 3
        mul             ; A = 48 (3*x^2)

        ; Save term1 in stack
        ldc 0x1000
        a2sp
        adj -2
        stl 0           ; stack[0] = 48

        ; Compute 5*x
        ldc x
        ldnl 0          ; A = 4
        ldc 5           ; B = 4, A = 5
        mul             ; A = 20 (5*x)

        ; Add 3*x^2 (stored in stack[0])
        ldl 0           ; B = 20, A = 48
        add             ; A = 68

        ; Add constant 7
        adc 7           ; A = 75

        ; Store final result in memory
        ldc result
        stnl 0          ; result = 75
        HALT

; Data Section
x:      data 4
result: data 0