//Name: Aahan Nayak
//Roll No: 2501AI21
/*
 * Declaration of Authorship:
 * I declare that this assignment is my own work in accordance
 * with IIT Patna Academic Honesty Policy.
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>
#include <cstdio>

using namespace std;


//1 m bit words initialization
const int MEM_SIZE = 1024 * 1024; 
const uint64_t MAX_CYCLES = 100000000ULL; // infinite loop guard

//intiialization for registers

int32_t memory[MEM_SIZE];
int32_t A = 0;
int32_t B = 0;
int32_t PC = 0;
int32_t SP = 0;

//modes for command

bool trace_mode = false;
bool dump_before = false;
bool dump_after = false;

const char* mnemonics[21] = {
    "ldc", "adc", "ldl", "stl", "ldnl", "stnl",
    "add", "sub", "shl", "shr", "adj",  "a2sp",
    "sp2a", "call", "return", "brz", "brlz", "br",
    "HALT", "mul", "div"
};

// sign extend the 24 bit operand code since c++ cant reckon it
int32_t sign_extend24(int32_t val) {
    if (val & 0x00800000) {
        return val | (~0x00FFFFFF);
    }
    return val & 0x00FFFFFF;
}

// memory dumps using 4 words per line 
void dump_memory(const string &title, int limit) {
    cout << title << endl;
    for (int i = 0; i < limit; i += 4) {
        printf("%08X ", i);
        for (int j = 0; j < 4 && (i + j) < limit; j++) {
            printf("%08X ", (uint32_t)memory[i + j]);
        }
        cout << "\n";
    }
}

// show mnemonic table
void show_isa() {
    cout << "Opcode Mnemonic Operand\n";
    cout << "0      ldc      value\n";
    cout << "1      adc      value\n";
    cout << "2      ldl      offset\n";
    cout << "3      stl      offset\n";
    cout << "4      ldnl     offset\n";
    cout << "5      stnl     offset\n";
    cout << "6      add\n";
    cout << "7      sub\n";
    cout << "8      shl\n";
    cout << "9      shr\n";
    cout << "10     adj      value\n";
    cout << "11     a2sp\n";
    cout << "12     sp2a\n";
    cout << "13     call     offset\n";
    cout << "14     return\n";
    cout << "15     brz      offset\n";
    cout << "16     brlz     offset\n";
    cout << "17     br       offset\n";
    cout << "18     HALT\n";
    cout << "19     mul\n";
    cout << "20     div\n";
}

int main(int argc, char* argv[]) {

    //bin file
    string binfile = "";


    //checks the type of arg 
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "-trace") trace_mode = true;
        else if (arg == "-before") dump_before = true;
        else if (arg == "-after") dump_after = true;
        else if (arg == "-isa") {
            show_isa();
            return 0;
        } else {
            binfile = arg;
        }
    }

    if (binfile.empty()) {
        cout << "Usage: ./emu [options] <file.o>\n";
        cout << "Options: -trace, -before, -after, -isa\n";
        return 1;
    }

    // Load binary object file
    ifstream file(binfile, ios::binary);
    if (!file.is_open()) {
        cerr << "Error: Cannot open " << binfile << endl;
        return 1;
    }

    //reads from memory using 4 bytes at once
    int code_size = 0;
    while (file.read((char*)&memory[code_size], 4)) {
        code_size++;
        if (code_size >= MEM_SIZE) break;
    }
    file.close();

    if (dump_before) {
        dump_memory("Memory dump before execution:", code_size > 40 ? code_size : 40);
    }

    //for not halt

    bool halted = false;
    uint64_t cycles = 0;

    // execution loop
    while (!halted) {
        if (PC < 0 || PC >= MEM_SIZE) {
            cerr << "Fatal error: PC out of bounds (" << PC << ")\n";
            return 1;
        }

        int32_t inst = memory[PC];
        int prev_pc = PC;
        PC++; // Implicit pre-increment of PC
        cycles++;

        int opcode = inst & 0xFF;
        int32_t operand = sign_extend24((inst >> 8) & 0x00FFFFFF);

        //for trace mode shows the hex code

        if (trace_mode) {
            if (opcode >= 0 && opcode <= 20) {
                printf("PC=%08X, SP=%08X, A=%08X, B=%08X %s %08X\n",
                       prev_pc, SP, (uint32_t)A, (uint32_t)B, mnemonics[opcode], (uint32_t)operand);
            } else {
                printf("PC=%08X, SP=%08X, A=%08X, B=%08X unknown(%02X)\n",
                       prev_pc, SP, (uint32_t)A, (uint32_t)B, opcode);
            }
        }


        //switch logic
        switch (opcode) {
            case 0:  B = A; A = operand; break;              // ldc
            case 1:  A = A + operand; break;                 // adc
            case 2:  B = A; A = memory[SP + operand]; break; // ldl
            case 3:  memory[SP + operand] = A; A = B; break; // stl
            case 4:  A = memory[A + operand]; break;         // ldnl
            case 5:  memory[A + operand] = B; break;         // stnl
            case 6:  A = B + A; break;                       // add
            case 7:  A = B - A; break;                       // sub
            case 8:  A = B << A; break;                      // shl
            case 9:  A = B >> A; break;                      // shr
            case 10: SP = SP + operand; break;               // adj
            case 11: SP = A; A = B; break;                   // a2sp
            case 12: B = A; A = SP; break;                   // sp2a
            case 13: B = A; A = PC; PC = PC + operand; break;// call
            case 14: PC = A; A = B; break;                   // return
            case 15: if (A == 0) PC = PC + operand; break;   // brz
            case 16: if (A < 0)  PC = PC + operand; break;   // brlz
            case 17: PC = PC + operand; break;               // br
            case 18: halted = true; break;                   // HALT
            case 19: A = B * A; break;                       // mul
            case 20:                                         // div
                if (A == 0) {
                    //div by zero
                    cerr << "Runtime Error: Division by zero at PC = " << prev_pc << "\n";
                    return 1;
                }
                A = B / A;
                break;
            default:
                cerr << "Fatal error: Unknown opcode " << opcode << " at PC = " << prev_pc << "\n";
                return 1;
        }
        //max cycles possible
        if (cycles >= MAX_CYCLES) {
            cerr << "Error: Cycle limit exceeded (infinite loop detected)\n";
            return 1;
        }
    }

    cout << "Execution halted normally after " << cycles << " instructions.\n";
    //mem dump
    if (dump_after) {
        dump_memory("Memory dump after execution:", code_size > 40 ? code_size : 40);
    }

    return 0;
}