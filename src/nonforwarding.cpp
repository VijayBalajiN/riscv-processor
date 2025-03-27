#include "processor.h"

class Processor_NonForwarding : public Processor {
public:
    Processor_NonForwarding(string filename, int cycle_count) : Processor(filename, cycle_count) {
    }


    int stall_detector() override {
        unsigned int opcode = latch_id_r.control.opcode;

        switch (opcode) {
            case R_TYPE: case SB_TYPE: case S_TYPE: case I_TYPE3: {
                unsigned int rs1 = latch_id_r.control.rs1;
                unsigned int rs2 = latch_id_r.control.rs2;

                if ( (latch_ex_r.control.squash == 0 && latch_ex_r.control.rd != 0 && 
                            (latch_ex_r.control.rd == rs1 || latch_ex_r.control.rd == rs2)) || 
                    (latch_m_r.control.squash == 0 && latch_m_r.control.rd != 0 && 
                            (latch_m_r.control.rd == rs1 || latch_m_r.control.rd == rs2)) ) return 1;
                break;
            }
            case I_TYPE2: case I_TYPE1: case UJ_TYPE: {
                unsigned int rs1 = latch_id_r.control.rs1;

                if ( (latch_ex_r.control.squash == 0 && latch_ex_r.control.rd != 0 && 
                            (latch_ex_r.control.rd == rs1)) || 
                    (latch_m_r.control.squash == 0 && latch_m_r.control.rd != 0 && 
                            (latch_m_r.control.rd == rs1)) ) return 1;
            }
            default:
                printf("Unknown Opcode\n");
        }
        return 0;
        
    }
};

int main (int argc, char * argv[]) {

    string filename;
    int cycle_count = 20;
    
    if (argc != 3) {
        cout << "Usage: ./executable [filename] [cycle_count]" << endl;

        filename = "../inputfiles/testcases/test1.txt";
    } else {
        filename = argv[1];
        string cycle_count_str = argv[2];
        cycle_count = stoi(cycle_count_str, nullptr, 10);
    }

    Processor_NonForwarding processor(filename, cycle_count);
    processor.run();
    processor.print();   
}