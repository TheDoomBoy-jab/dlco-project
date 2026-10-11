; ==============================================================================
; Test Case: Arithmetic Mean & Polynomial using 'mul' and 'div'
; Author: Aahan Nayak
;
; Description:
;   Computes:
;   1. Sum = array[0] + array[1] + array[2] + array[3]
;   2. Mean = Sum / count (using IITPSx 'div')
;   3. Scaled = Mean * Factor (using IITPSx 'mul')
; ==============================================================================

        ldc 0x1000
        a2sp

        ; Compute sum
        ldc array
        ldnl 0          ; A = 10
        ldc array
        ldnl 1          ; A = 20, B = 10
        add             ; A = 30
        ldc array
        ldnl 2          ; A = 30, B = 30
        add             ; A = 60
        ldc array
        ldnl 3          ; A = 40, B = 60
        add             ; A = 100
        ldc sum
        stnl 0          ; sum = 100

        ; Mean = Sum / count
        ldc sum
        ldnl 0          ; A = 100
        ldc count
        ldnl 0          ; A = 4, B = 100
        div             ; A = 100 / 4 = 25
        ldc mean
        stnl 0          ; mean = 25

        ; Scaled = Mean * 3
        ldc mean
        ldnl 0          ; A = 25
        ldc factor
        ldnl 0          ; A = 3, B = 25
        mul             ; A = 25 * 3 = 75
        ldc scaled
        stnl 0          ; scaled = 75

        HALT

count:  data 4
factor: data 3
array:  data 10
        data 20
        data 30
        data 40
sum:    data 0
mean:   data 0
scaled: data 0
