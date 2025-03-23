#include <string>
#include <vector>
#include "parse.h"

using namespace std;

typedef struct control_signals {
    unsigned int funct7 : 7;
    unsigned int rs2 : 5;
    unsigned int rs1 : 5;
    unsigned int funct3: 3;
    unsigned int rd : 5;
    unsigned int opcode : 7;
    unsigned int immed : 20;
    unsigned int squash : 1; 
} control_signals;

typedef struct latch_if {
    int line;
} latch_if;

typedef struct latch_id {
    control_signals control;
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
        filename = filename;

        (bin_instruc, pretty_instruc) = get_code(filename); 
        
               

    }

    vector <vector<string>> run() {
        for(int line = 0; line < bin_instruc.size(); line++) {
            string curr_instruction = 
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

    int clock = 0;



};

int main () {
    printf("%lu", sizeof(control_signals));
}