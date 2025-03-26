#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <string>
#include <vector>
#include <tuple>
#include <bitset>
#include <iostream>
#include <assert.h>
#include "parse.h"
#include "print_csv.h"

#define R_TYPE 51
#define I_TYPE1 3
#define I_TYPE2 19
#define I_TYPE3 103
#define S_TYPE 35
#define SB_TYPE 99
#define U_TYPE 55
#define UJ_TYPE 111


using namespace std;

typedef struct control_signals {
    unsigned int line;
    unsigned int funct7 : 7;
    unsigned int rs2 : 5;
    unsigned int rs1 : 5;
    unsigned int funct3 : 3;
    unsigned int rd : 5;
    unsigned int opcode : 7;
    unsigned int immed : 21;
    unsigned int squash : 1;

    control_signals(unsigned int line, unsigned int funct7, unsigned int rs2, 
                    unsigned int rs1, unsigned int funct3, unsigned int rd, 
                    unsigned int opcode, unsigned int immed, unsigned int squash);
    control_signals();
}control_signals;

typedef struct latch_if {
    unsigned int line;
    int stall;
}latch_if;

typedef struct latch_id {
    control_signals control;
    int stall;
}latch_id;

typedef struct latch_ex {
    control_signals control;
}latch_ex ;

typedef struct latch_m {
    control_signals control;
}latch_m;

typedef struct latch_wb {
    control_signals control;
}latch_wb;

class Processor {
public:
    Processor(string filename, int cycle_count);
    void run();
    void print();

protected:
    string filename;
    int* stack;
    int registers[32];

    vector<string> bin_instruc;
    vector<string> pretty_instruc;
    vector<int> clock_end;

    int id_stall;
    int if_stall;
    int clock;
    int branch;

    int cycle_count;
    int tot_lines;

    latch_if latch_if_l, latch_if_r;
    latch_id latch_id_l, latch_id_r;
    latch_ex latch_ex_l, latch_ex_r;
    latch_m latch_m_l, latch_m_r;
    latch_wb latch_wb_l, latch_wb_r;

    void latch_set();
    void run_if();
    void run_id();
    void run_ex();
    void run_m();
    void run_wb();
    virtual int stall_detector();
};

#endif // PROCESSOR_H
