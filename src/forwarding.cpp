#include "processor.h"

class Processor_Forwarding : public Processor {
public:
    Processor_Forwarding(string filename, int cycle_count) : Processor(filename, cycle_count) {
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

int main (int argc, char * argv[]) {
    if (argc != 3) {
        cout << "Usage: ./executable [filename] [cycle_count]" << endl;
        return 0;
    }

    string filename = argv[1];
    string cycle_count_str = argv[2];
    int cycle_count = stoi(cycle_count_str, nullptr, 10);


    Processor_Forwarding processor(filename, cycle_count);
    processor.run();
    processor.print();   
}