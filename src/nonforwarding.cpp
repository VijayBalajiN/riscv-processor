#include "processor.h"

class Processor_NonForwarding : public Processor {
public:
    Processor_NonForwarding(string filename) : Processor(filename) {
    }


    int stall_detector() override {
        unsigned int opcode = latch_id_r.control.opcode;

        switch (opcode) {
            case 51: {
                unsigned int rs1 = latch_id_r.control.rs1;
                unsigned int rs2 = latch_id_r.control.rs1;

                if ( (latch_ex_r.control.squash == 0 && latch_ex_l.control.rd != 0 && 
                            (latch_ex_l.control.rd == rs1 || latch_ex_l.control.rd == rs2)) || 
                    (latch_m_r.control.squash == 0 && latch_m_l.control.rd != 0 && 
                            (latch_m_l.control.rd == rs1 || latch_m_l.control.rd == rs2)) ) return 1;
                break;
            }
            case 19: case 3: {
                unsigned int rs1 = latch_id_r.control.rs1;

                if ( (latch_ex_r.control.squash == 0 && latch_ex_l.control.rd != 0 && 
                            (latch_ex_l.control.rd == rs1)) || 
                    (latch_m_r.control.squash == 0 && latch_m_l.control.rd != 0 && 
                            (latch_m_l.control.rd == rs1)) ) return 1;
            }
            default:
                printf("Unknown Opcode\n");
        }
        return 0;
        
    }
};

int main () {
    Processor_NonForwarding processor("../inputfiles/strlen.txt");
    processor.run();
    processor.print();   
}