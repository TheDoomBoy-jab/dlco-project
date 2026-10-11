; ==============================================================================
; Test Case: Fibonacci Sequence Generator (Iterative)
; Author: Aahan Nayak
; Roll No: 2501AI21
;
; Description:
;   Generates the first 8 Fibonacci numbers: 0, 1, 1, 2, 3, 5, 8, 13
;   Stores them into the 'fib' array in memory.
; ==============================================================================

        ldc 0x1000
        a2sp
        adj -4

        ; Initialize first two terms: fib[0] = 0, fib[1] = 1
        ldc fib
        ldc 0
        stnl 0

        ldc fib
        ldc 1
        stnl 1

        ; Loop index i = 2
        ldc 2
        stl 0           ; stack[0] = i = 2

fib_loop:
        ; Check if i == count (8)
        ldl 0
        ldc count
        ldnl 0
        sub             ; i - count
        brz fib_done    ; if i == count, exit loop

        ; Compute fib[i-1] + fib[i-2]
        ; Load fib[i-1]
        ldc fib
        ldl 0
        add
        adc -1
        ldnl 0          ; A = fib[i-1]

        ; Load fib[i-2]
        ldc fib
        ldl 0
        add
        adc -2
        ldnl 0          ; B = fib[i-1], A = fib[i-2]

        add             ; A = fib[i-1] + fib[i-2]

        ; Store into fib[i]
        ldc fib
        ldl 0
        add
        stnl 0          ; fib[i] = A

        ; i++
        ldl 0
        adc 1
        stl 0
        br fib_loop

fib_done:
        HALT

; Data Section
count:  data 8
fib:    data 0
        data 0
        data 0
        data 0
        data 0
        data 0
        data 0
        data 0