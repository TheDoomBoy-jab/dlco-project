//Name: Aahan Nayak
//Roll No: 2501AI21

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <cstdint>
#include <cctype>
using namespace std;


//base struct for the project

struct line{
    int linenum;
    string raw;
    string label;
    string mnemonic;
    string operand;
    int pc=0;
    int32_t code=0;
    bool hascode=false;
};


//final vectors for emu, passes and log files
vector<line> lines;

map<string, int> labels;
map<string, int> labelline;
map<string, int> labelused;
vector<string> errors;
vector<string> warnings;

//opcodes for all the mnemonics, incase not present then raises an error

map<string, int> opcodes{
    {"ldc", 0},  {"adc", 1},  {"ldl", 2},   {"stl", 3},
    {"ldnl", 4}, {"stnl", 5}, {"add", 6},   {"sub", 7},
    {"shl", 8},  {"shr", 9},  {"adj", 10},  {"a2sp", 11},
    {"sp2a", 12},{"call", 13}, {"return", 14},{"brz", 15},
    {"brlz", 16},{"br", 17},  {"HALT", 18}, {"halt", 18},
    {"mul", 19}, {"div", 20}
};

//isoperand for zero operand based mnemonics

bool isoperand(string m){
    return (m == "add" || m == "sub" || m == "shl" || m == "shr" ||
            m == "a2sp" || m == "sp2a" || m == "return" ||
            m == "HALT" || m == "halt" || m == "mul" || m == "div");
}

//isbranch for branch based mnemonics

bool isbranch(string m){
    return (m == "call" || m == "brz" || m == "brlz" || m == "br");
}


//trims the whitespaces
string trim(string s){
    while(!s.empty() && isspace(s.front())){
        s.erase(s.begin());
    }

    while(!s.empty() && isspace(s.back())){
        s.pop_back();
    }

    return s;
}


//checks if a label is a valid label that is only has '_' and char values


bool isvalidlabel(string name){
    if(name.empty() || (!isalpha(name[0]) && name[0]!='_')) return false;

    for(char c: name){
        if(!isalnum(c) && c!='_') return false;
    }

    return true;
}


//parse number for parsing the entire line  for pass 1 and pass 2
bool parsenumber(string s, int32_t &val){
    try{
        size_t idx=0;
        val=(int32_t)stol(s, &idx, 0);
        return idx == s.size();
    }catch(...){
        return false;
    }
}

//pass 1

void pass1(string filename){

    //reads file as input
    ifstream file(filename);
    if(!file.is_open()){
        cerr<<"error cannot open file "<<filename<<endl;
        exit(1);
    }

    string linetext;
    int linenum=0;
    int currentpc=0;

    //intitialising raw and linenum for each line

    while(getline(file, linetext)){
        linenum++;
        line l;
        l.linenum=linenum;
        l.raw=linetext;

        //checks for comments using ; and removes them from line 

        size_t commentpos=linetext.find(';');
        if(commentpos!=string::npos) linetext=linetext.substr(0, commentpos);
        linetext=trim(linetext);

        if(linetext.empty()){
            lines.push_back(l);
            continue;
        }

        //separates label from mnemonic

        size_t colonpos=linetext.find(':');
        if(colonpos!=string::npos){
            l.label=trim(linetext.substr(0, colonpos));
            linetext=trim(linetext.substr(colonpos+1));

            if(!isvalidlabel(l.label)){
                errors.push_back("line "+to_string(linenum)+": invalid label name '"+ l.label+"'");
            }else if(labels.count(l.label)){
                errors.push_back("line "+to_string(linenum)+ ": duplicate label '"+l.label+"'");
            }
        }

        //separates operand and mnemonic

        stringstream ss(linetext);
        ss>>l.mnemonic;
        if(!l.mnemonic.empty()){
            string rest;
            getline(ss, rest);
            l.operand=trim(rest);
        }
        //intializes cuurent pc
        l.pc=currentpc;
        if(!l.label.empty() && errors.empty()){
            //set instruction for const without affecting pc
            if(l.mnemonic == "SET"){
                int32_t val=0;
                if(parsenumber(l.operand, val)){
                    labels[l.label]=val;
                }else{
                    errors.push_back("line " + to_string(linenum) + ": SET requires a numeric constant");
                }
            }else{
                labels[l.label]=currentpc;
            }
            labelline[l.label]=linenum;
        }
        //for other instructions
        if(!l.mnemonic.empty() && l.mnemonic != "SET"){
            l.hascode=true;
            currentpc++;
        }
        //push all the lines for pass 2 
        lines.push_back(l);
    }
}

void pass2(){

    //handles data
    for(auto &l : lines){
        if(!l.hascode) continue;
        int opcode = 0;
        int32_t operandval = 0;
        if(l.mnemonic == "data"){
            if(!parsenumber(l.operand, operandval)){
                //if label is present
                if(labels.count(l.operand)){
                    operandval = labels[l.operand];
                    labelused[l.operand]++;
                }else{
                    errors.push_back("line " + to_string(l.linenum) + ": undefined symbol '" + l.operand + "'");
                }
            }
            l.code = operandval;
            continue;
        }

        //if not present
        if(!opcodes.count(l.mnemonic)){
            errors.push_back("line " + to_string(l.linenum) + ": unknown instruction '" + l.mnemonic + "'");
            continue;
        }

        //if present
        opcode = opcodes[l.mnemonic];
        if(isoperand(l.mnemonic)){
            if(!l.operand.empty()){
                errors.push_back("line " + to_string(l.linenum) + ": unexpected operand for " + l.mnemonic);
            }

            //handles opcode for operand by bit shifting
            l.code = opcode & 0xFF;
            continue;
        }
        if(l.operand.empty()){
            errors.push_back("line " + to_string(l.linenum) + ": missing operand for " + l.mnemonic);
            continue;
        }
        if(parsenumber(l.operand, operandval)){
        }else if(labels.count(l.operand)){
            labelused[l.operand]++;
            if(isbranch(l.mnemonic)){

                //for branch opeartions calculates the offset
                operandval = labels[l.operand] - (l.pc + 1);

                //if offset<=0 creates warrning for jump instruction
                if(operandval <= 0 && l.mnemonic == "br"){
                    warnings.push_back("line " + to_string(l.linenum) + ": possible infinite loop (backward jump)");
                }
            }else{
                //for other operations(basic +1 pc)
                operandval = labels[l.operand];
            }
        }else{
            errors.push_back("line " + to_string(l.linenum) + ": undefined label '" + l.operand + "'");
            continue;
        }
        l.code = (int32_t)(((uint32_t)(operandval & 0x00FFFFFF) << 8) | (uint32_t)(opcode & 0xFF));
    }
    for(auto &pair : labels){
        if(labelused[pair.first] == 0){
            warnings.push_back("line " + to_string(labelline[pair.first]) + ": unused label '" + pair.first + "'");
        }
    }
}

void savefiles(string base){

    //for .o reads 4 bytes for encoding in little endian
    if(errors.empty()){
        ofstream objfile(base + ".o", ios::binary);
        for(auto &l : lines){
            if(l.hascode){
                uint32_t w = (uint32_t)l.code;
                char bytes[4] = {
                    (char)(w & 0xFF),
                    (char)((w >> 8) & 0xFF),
                    (char)((w >> 16) & 0xFF),
                    (char)((w >> 24) & 0xFF)
                };
                objfile.write(bytes, 4);
            }
        }
    }


    //for lst file handles based on has code for comments skips 20 spaces
    ofstream lstfile(base + ".lst");
    for(auto &l : lines){
        char buf[64];
        if(l.hascode){
            sprintf(buf, "%08X %08X  ", l.pc, (uint32_t)l.code);
            lstfile << buf << l.raw << "\n";
        }else{
            lstfile << "                   " << l.raw << "\n";
        }
        //lists error as well(based on extra credit points)
        for(auto &e : errors){
            string lineTag = "line " + to_string(l.linenum) + ":";
            if(e.find(lineTag) != string::npos){
                lstfile << "        *** ERROR: " << e << " ***\n";
            }
        }
    }

    ofstream logfile(base + ".log");
    for(auto &e : errors) logfile << "ERROR: " << e << "\n";
    for(auto &w : warnings) logfile << "WARNING: " << w << "\n";
    logfile << "\n--- Symbol Table ---\n";
    for(auto &pair : labels){
        logfile << pair.first << " : " << pair.second << "\n";
    }
    if(errors.empty()) logfile << "\nProgram assembled successfully.\n";
}

int main(int argc, char* argv[]){

    //checks the number of args for command
    if(argc < 2){
        cout << "Usage: ./asm <file.asm>" << endl;
        return 1;
    }


    //main handles the emu adn checks what arg is passed 
    string sourcefile = argv[1];
    string base = sourcefile.substr(0, sourcefile.find_last_of('.'));

    pass1(sourcefile);
    pass2();
    savefiles(base);

    if(errors.empty()){
        cout << "Assembly successful: " << base << ".o, " << base << ".lst, " << base << ".log" << endl;
        return 0;
    }else{
        cout << "Assembly failed with " << errors.size() << " error(s):" << endl;
        for(auto &e : errors) cout << "  " << e << endl;
        return 1;
    }
}