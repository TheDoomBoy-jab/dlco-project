; ==============================================================================
; Bubble Sort Implementation in IITPSx Assembly
; Name: Aahan Nayak
; Roll No: 2501AI21
;
; Description:
;   Sorts an array of 32-bit signed integers in ascending order in-place
;   using the Bubble Sort algorithm.
;
; Memory Layout:
;   count: size of array (N)
;   array: start of N 32-bit integers
; ==============================================================================

        ldc 0x1000
        a2sp
        call sort
        HALT

; ==============================================================================
; sort subroutine
; Stack Frame (SP):
;   SP + 0: i (outer loop index)
;   SP + 1: j (inner loop index)
;   SP + 2: return address
;   SP + 3: ptr = &array[j]
;   SP + 4: val_j
;   SP + 5: val_j_plus_1
; ==============================================================================
sort:   adj -6
        stl 2               ; Save return address

        ldc 0
        stl 0               ; i = 0

outer:  ldc count
        ldnl 0
        adc -1              ; count - 1
        ldl 0
        sub                 ; (count - 1) - i
        brlz done
        brz done

        ldc 0
        stl 1               ; j = 0

inner:  ldc count
        ldnl 0
        adc -1
        ldl 0
        sub                 ; limit = count - 1 - i
        ldl 1
        sub                 ; limit - j
        brlz next_i
        brz next_i

        ; Compute ptr = &array[j] and store in SP[3]
        ldc array
        ldl 1
        add
        stl 3               ; SP[3] = ptr

        ; Load val_j = memory[ptr + 0] into SP[4]
        ldl 3
        ldnl 0
        stl 4               ; SP[4] = val_j

        ; Load val_j_plus_1 = memory[ptr + 1] into SP[5]
        ldl 3
        ldnl 1
        stl 5               ; SP[5] = val_j_plus_1

        ; Compare: we want val_j <= val_j_plus_1
        ; Compute val_j - val_j_plus_1:
        ; If val_j > val_j_plus_1, we must swap.
        ; To compute val_j - val_j_plus_1:
        ; Put val_j into B, and val_j_plus_1 into A, then sub.
        ldl 4               ; A = val_j
        ldl 5               ; B = val_j, A = val_j_plus_1
        sub                 ; A = B - A = val_j - val_j_plus_1
        ; If val_j - val_j_plus_1 > 0, swap!
        ; Notice that if A > 0, then:
        ; A is NOT zero, and A is NOT less than zero.
        brz no_swap         ; If equal, don't swap
        brlz no_swap        ; If val_j < val_j_plus_1, don't swap

        ; SWAP elements in memory:
        ; memory[ptr + 1] := val_j
        ldl 4               ; A = val_j
        ldl 3               ; B = val_j, A = ptr
        stnl 1              ; memory[ptr + 1] := val_j

        ; memory[ptr + 0] := val_j_plus_1
        ldl 5               ; A = val_j_plus_1
        ldl 3               ; B = val_j_plus_1, A = ptr
        stnl 0              ; memory[ptr + 0] := val_j_plus_1

no_swap:
        ldl 1
        adc 1
        stl 1               ; j = j + 1
        br inner

next_i: ldl 0
        adc 1
        stl 0               ; i = i + 1
        br outer

done:   ldl 2
        adj 6
        return

; ==============================================================================
; Data Section
; ==============================================================================
count:  data 6
array:  data 25
        data -4
        data 10
        data 0
        data -15
        data 8
