# IITPSx Architecture Toolchain
A 32-Bit Computer Architecture, Two-Pass Assembler and Virtual Machine

**Author:** Aahan Nayak  
**Roll No:** 2501AI21  
**Course:** CS2102 Computer Architecture  
**Target Architecture:** IITPSx 32-Bit RISC-Like Processor  
**Language Standard:** Modern C++ (C++17)  

---

## 1. Executive Summary

This project implements a full-stack computer architecture toolchain for the 32-bit IITPSx instruction set:

1. **Two-Pass Assembler (`asm.cpp`)**: Tokenizes assembly language, constructs a symbol table in memory, resolves forward and backward PC-relative branch displacements, and generates binary machine code (`.o`), formatted listings (`.lst`), and diagnostics logs (`.log`).
2. **Processor Emulator / Virtual Machine (`emu.cpp`)**: Simulates a 4-register CPU (`A`, `B`, `PC`, `SP`) with 1M-word memory, runtime safety traps (division by zero, segmentation faults, infinite loop guard), and execution tracing.
3. **In-Place Bubble Sort (`bubblesort.asm`)**: A complete sorting algorithm written in native assembly demonstrating stack frame allocation and non-local memory pointer manipulation.
4. **Retro Visual IDE and Simulator (`index.html` + `app.cpp`)**: A standalone desktop interface providing live step-by-step debugging, live mutating memory inspection, and cycle monitoring.

---

## 2. System Architecture

```text
                          +-------------------------+
                          |   Assembly Code (.asm)  |
                          +------------+------------+
                                       |
                                [ PASS 1: SCAN ]
                        Discovers labels & builds Symbol Table
                                       |
                                [ PASS 2: CODEGEN ]
                        Resolves offsets & encodes 32-bit words
                                       |
               +-----------------------+-----------------------+
               |                       |                       |
               v                       v                       v
      +-----------------+     +-----------------+     +-----------------+
      | Binary Code (.o)|     | Listing (.lst)  |     | Diagnostics(.log|
      +--------+--------+     +-----------------+     +-----------------+
               |
               v
      +--------------------------------------------------------+
      |                 IITPSx CPU EMULATOR                    |
      |  +-----------+  +-----------+  +-----------+  +-----+  |
      |  | Reg A     |  | Reg B     |  | Prog Ctr  |  | SP  |  |
      |  | (Acc)     |  | (TopStack)|  | (PC)      |  |     |  |
      |  +-----------+  +-----------+  +-----------+  +-----+  |
      |  [ 1,048,576 32-Bit Words RAM ]  [ Runtime Div/0 Trap ]|
      +--------------------------------------------------------+
```

---

## 3. Machine Word Encoding (32-Bit)

Every hardware instruction fits inside a single 32-bit word:

```text
 31                                       8 7                 0
+------------------------------------------+-------------------+
|        Signed 24-Bit Operand / Offset    |  8-Bit Opcode     |
+------------------------------------------+-------------------+
```

- **Formula:** `Machine Word = ((Operand & 0x00FFFFFF) << 8) | (Opcode & 0xFF)`
- **Branch Offset Formula:** Because the IITPSx CPU pre-increments `PC++` during instruction fetch, branch displacements are computed as:  
  `Offset = Target Address - (Current PC + 1)`

---

## 4. Instruction Set Reference (ISA)

| Opcode | Mnemonic | Operand Type | Formal Operation | Description |
| :---: | :--- | :--- | :--- | :--- |
| **0** | `ldc` | Value | B := A; A := value | Load constant into A, push old A to B |
| **1** | `adc` | Value | A := A + value | Add constant to A |
| **2** | `ldl` | Stack Offset | B := A; A := memory[SP + offset] | Load local stack variable |
| **3** | `stl` | Stack Offset | memory[SP + offset] := A; A := B | Store local stack variable |
| **4** | `ldnl`| Pointer Offset | A := memory[A + offset] | Load indirect via pointer A |
| **5** | `stnl`| Pointer Offset | memory[A + offset] := B | Store indirect via pointer A from B |
| **6** | `add` | None | A := B + A | Add registers |
| **7** | `sub` | None | A := B - A | Subtract registers |
| **8** | `shl` | None | A := B << A | Logical shift left |
| **9** | `shr` | None | A := B >> A | Logical shift right |
| **10**| `adj` | Value | SP := SP + value | Adjust stack pointer frame |
| **11**| `a2sp`| None | SP := A; A := B | Transfer A to Stack Pointer |
| **12**| `sp2a`| None | B := A; A := SP | Transfer Stack Pointer to A |
| **13**| `call`| PC Offset | B := A; A := PC; PC := PC + offset | Subroutine call |
| **14**| `return`| None | PC := A; A := B | Subroutine return |
| **15**| `brz` | PC Offset | if A == 0 then PC := PC + offset | Conditional branch if zero |
| **16**| `brlz`| PC Offset | if A < 0 then PC := PC + offset | Conditional branch if negative |
| **17**| `br`  | PC Offset | PC := PC + offset | Unconditional branch |
| **18**| `HALT`| None | Stops processor | Halts instruction execution cycle |
| **19**| `mul` | None | A := B * A | Signed multiplication (Extension) |
| **20**| `div` | None | A := B / A (Traps if A == 0) | Signed division (Extension) |

---

## 5. Build and Compilation

Build everything with one command using the included `Makefile`:

```bash
# 1. Compile both the Assembler and Emulator
make

# 2. Run the complete automated test suite
make test

# 3. Clean all binaries and generated files
make clean
```

---

## 6. Manual CLI Usage

### Running the Assembler (`asm`)
```bash
./asm bubblesort.asm
```
Generates `bubblesort.o` (Binary), `bubblesort.lst` (Listing), and `bubblesort.log` (Diagnostics).

### Running the Emulator (`emu`)
```bash
# Execute with step-by-step register trace:
./emu -trace tests/test5.o

# Run and inspect memory before and after execution:
./emu -before -after bubblesort.o

# View ISA reference:
./emu -isa
```

---

## 7. Visual Simulator and IDE (Extra Credit)

The project includes an interactive visual development environment:

- **Real-Time Register Dashboard**: Watch `A`, `B`, `PC`, and `SP` update live on each clock cycle.
- **Mutating RAM Grid**: Visual highlight of memory cells changing (e.g. watch array elements physically swap during bubble sort).
- **Execution Stepper**: Single-step into instructions, pause, or adjust the clock frequency slider from 1 Hz to maximum speed.
- **Safety Gating**: Execution controls are disabled until source code is compiled and verified.

### Launching the Desktop App:
```bash
./app
```
Opens the standalone window across macOS, Windows, and Linux.

---

## 8. Verification and Test Suite

| Suite | File | Focus Area | Expected Behavior |
| :--- | :--- | :--- | :--- |
| **Algorithm** | `bubblesort.asm` | In-place array sorting | Sorts `[25, -4, 10, 0, -15, 8]` into `[-15, -4, 0, 8, 10, 25]` |
| **Math** | `test_math.asm` | Extension opcodes `mul` & `div` | Arithmetic mean calculation & scaling |
| **Trap Test** | `test_div0.asm` | Divide-by-zero protection | CPU halts gracefully with runtime error trap |
| **Labels** | `tests/test2.asm` | Syntax error trapping | Catches duplicate labels, bogus names (`1badlabel`) |
| **Subroutines**| `tests/test4.asm` | Stack operations | Validates `call`, `return`, and frame adjustment (`adj`) |
| **Stress Test**| `tests/test7.asm` | Full assembler torture | Verifies detection of 7 distinct syntax violations |

---

## 9. Repository File Map

```text
├── Makefile             # Automated build, test, and clean pipeline
├── claims               # Official submission claims document
├── asm.cpp              # Two-pass assembler implementation
├── emu.cpp              # 32-bit CPU emulator implementation
├── bubblesort.asm       # In-place bubble sort in IITPSx assembly
├── app.cpp              # Native cross-platform desktop launcher
├── index.html           # Retro pixel architecture simulator UI
└── tests/               # Comprehensive regression test suite
```
