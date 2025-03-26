#include "processor.h"

class Processor_Forwarding : public Processor {
public:
    Processor_Forwarding(string filename) : Processor(filename) {
    }


    int stall_detector() override {
        unsigned int opcode = latch_id_r.control.opcode;

        switch (opcode) {
            case 99:  {
                unsigned int rs1 = latch_id_r.control.rs1;
                unsigned int rs2 = latch_id_r.control.rs1;

                if ( (latch_ex_r.control.squash == 0 && latch_ex_r.control.rd != 0 && latch_ex_r.control.opcode == 3 && 
                            (latch_ex_r.control.rd == rs1 || latch_ex_r.control.rd == rs2)) || 
                    (latch_m_r.control.squash == 0 && latch_m_r.control.rd != 0 && latch_ex_r.control.opcode == 3 && 
                            (latch_m_r.control.rd == rs1 || latch_m_r.control.rd == rs2)) ) return 1;
                break;
            }
            
            default:
                printf("Unknown Opcode\n");
        }
        return 0;
        
    }
};

int main () {
    Processor_Forwarding processor("../inputfiles/strlen.txt");
    processor.run();
    processor.print();   
}