#include <string>
#include <vector>
#include "parse.h"
#include <tuple>
#include <assert.h>
#include <bitset>
#include <iostream>

using namespace std;

typedef struct control_signals {
    unsigned int line;
    unsigned int funct7 : 7;
    unsigned int rs2 : 5;
    unsigned int rs1 : 5;
    unsigned int funct3: 3;
    unsigned int rd : 5;
    unsigned int opcode : 7;
    unsigned int immed : 20;
    unsigned int squash : 1; 

    control_signals(unsigned int line, unsigned int funct7, unsigned int rs2, 
                    unsigned int rs1, unsigned int funct3, unsigned int rd, 
                    unsigned int opcode, unsigned int immed = 0, unsigned int squash = 0) 
        : line(line), funct7(funct7), rs2(rs2), rs1(rs1), funct3(funct3), rd(rd), 
          opcode(opcode), immed(immed), squash(squash) {}

    control_signals() 
        : line(-1), funct7(127), rs2(31), rs1(31), funct3(7), rd(31), 
          opcode(127), immed(1048575), squash(1) {}

} control_signals;

typedef struct latch_if {
    unsigned int line;
    int stall;
} latch_if;

typedef struct latch_id {
    control_signals control;
    int stall;
} latch_id;

typedef struct latch_ex {
    control_signals control;
    int alu_output;
} latch_ex;

typedef struct latch_m {
    control_signals control;
    int mem_output;
} latch_m;

typedef struct latch_wb {
    control_signals control;
} latch_wb;

class Processor {
    public:

    Processor(string filename) {

        stack = new int[2010000];
        this->filename = filename;

        tuple<vector<string>, vector<string>> instruc_tuple = get_code(filename);

        bin_instruc = get<0>(instruc_tuple);
        pretty_instruc = get<1>(instruc_tuple);

        latch_if_l.line = 0;
        latch_if_l.stall = 0;
    }

    void run() {
        

        for (; clock <= 2; clock++) {
            latch_set();
            run_if();
            run_id();
        }
    }

    void print () {
        for (auto &a : pretty_instruc) {
            cout << a << "\n";
        }
    }

    private:

    string filename;
    int* stack;
    int registers[32];

    vector<string> bin_instruc;
    vector<string> pretty_instruc;

    int id_stall = 0;
    int if_stall = 0;

    int clock = 1;

    latch_if latch_if_l, latch_if_r;
    latch_id latch_id_l, latch_id_r;
    latch_ex latch_ex_l, latch_ex_r;
    latch_m latch_m_l, latch_m_r;
    latch_wb latch_wb_l, latch_wb_r;

    void latch_set () {
        latch_if_r = latch_if_l;
        latch_id_r = latch_id_l;
        latch_ex_r = latch_ex_l;
        latch_m_r = latch_m_l;
        latch_wb_r = latch_wb_l;
    }

    void run_if() {
        unsigned int line = latch_if_r.line;
        string instruc_hex = bin_instruc[line/4];

    

        unsigned int instruc_dec = stoi(instruc_hex, nullptr, 16);


        bitset<32> instruc_bin(instruc_dec);

        string instruc = instruc_bin.to_string();
        string opcode_str = instruc.substr(25, 7);  // opcode [0:6]
        string rd_str = instruc.substr(20, 5);      // rd [7:11]
        string funct3_str = instruc.substr(17, 3);  // funct3 [12:14]
        string rs1_str = instruc.substr(12, 5);     // rs1 [15:19]
        string rs2_str = instruc.substr(7, 5);      // rs2 [20:24]
        string funct7_str = instruc.substr(0, 7);   // funct7 [25:31]

        unsigned int opcode = stoi(opcode_str, nullptr, 2);
        unsigned int rd = stoi(rd_str, nullptr, 2);
        unsigned int funct3 = stoi(funct3_str, nullptr, 2);
        unsigned int rs1 = stoi(rs1_str, nullptr, 2);
        unsigned int rs2 = stoi(rs2_str, nullptr, 2);
        unsigned int funct7 = stoi(funct7_str, nullptr, 2);

        


        switch (opcode) {
            case 51: {
                control_signals control_signal = {
                    line, funct7, rs2, rs1, funct3, rd, opcode
                };
                latch_id_l = {
                    control_signal
                };
                break;
            }
            case 3: case 19: case 103: {
                string immed_str = instruc.substr(0, 12);
                unsigned int immed = stoi(immed_str, nullptr, 2);

                control_signals control_signal = {
                    line, 0, 0, rs1, funct3, rd, opcode, immed, 0
                };
                latch_id_l = {
                    control_signal
                };
                break;
            }
            case 35: {
                string immed_str_1 = instruc.substr(0, 7);
                string immed_str_2 = instruc.substr(20, 5);

                int immed_1 = stoi(immed_str_1, nullptr, 2);
                int immed_2 = stoi(immed_str_2, nullptr, 2);

                unsigned int immed = (immed_1 << 5) | immed_2;

                control_signals control_signal = {
                    line, rs2, rs1, funct3, 0, opcode, immed, 0
                };

                latch_id_l = {
                    control_signal
                };
                break;
            }
            default:
                printf("Incorrect Opcode");
                assert(0);
        }

        string start_instruc = "";
        for (int i = 1; i < clock; i++) {
            start_instruc += " ;";
        }

        pretty_instruc[line/4].append(";"+start_instruc+"IF");

        latch_if_l.line += 4;

    }

    void run_id () {
        unsigned int line = latch_id_r.control.line;
        unsigned int opcode = latch_id_r.control.opcode;
        unsigned int squash = latch_id_r.control.squash;

        if (squash) return;

        pretty_instruc[line/4].append(";ID");
    }
};

int main () {
    control_signals test;

    printf("%d\n", test.squash);
    printf("%lu\n", sizeof(control_signals));

    Processor processor("../inputfiles/strlen.txt");
    processor.run();
    processor.print();   
}