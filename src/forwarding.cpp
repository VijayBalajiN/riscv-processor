#include "processor.h"

class Processor_Forwarding : public Processor {
public:
    Processor_Forwarding(string filename, int cycle_count) : Processor(filename, cycle_count) {
    }


    int stall_detector() override {
        unsigned int opcode = latch_id_r.control.opcode;

        switch (opcode) {
            case SB_TYPE:  {
                unsigned int rs1 = latch_id_r.control.rs1;
                unsigned int rs2 = latch_id_r.control.rs2;
                // mem with or wihtout a gap of one followed by branch 
                if ( (latch_ex_r.control.squash == 0 && latch_ex_r.control.rd != 0 && latch_ex_r.control.opcode == I_TYPE1 && 
                            (latch_ex_r.control.rd == rs1 || latch_ex_r.control.rd == rs2)) || 
                    (latch_m_r.control.squash == 0 && latch_m_r.control.rd != 0 && latch_m_r.control.opcode == I_TYPE1 && 
                            (latch_m_r.control.rd == rs1 || latch_m_r.control.rd == rs2)) ) return 1;
                
                // alu (51->r_type 19 -> i_type2 for xori, addi, etc) followed by branch 
                if ( latch_ex_r.control.squash == 0 && latch_ex_r.control.rd != 0 && ( latch_ex_r.control.opcode == R_TYPE || latch_ex_r.control.opcode == I_TYPE2) && 
                    (latch_ex_r.control.rd == rs1 || latch_ex_r.control.rd == rs2) ) return 1;
                return 0;
                break;
            }
            case I_TYPE3:{
                unsigned int rs1 = latch_id_r.control.rs1;
                // jalr preceded by mem with or without a gap of one
                if ( (latch_ex_r.control.squash == 0 && latch_ex_r.control.rd != 0 && latch_ex_r.control.opcode == I_TYPE1 && 
                        (latch_ex_r.control.rd == rs1)) || 
                    (latch_m_r.control.squash == 0 && latch_m_r.control.rd != 0 && latch_m_r.control.opcode == I_TYPE1 && 
                        (latch_m_r.control.rd == rs1)) ) return 1;

                // jalr preceded by alu
                if ( (latch_ex_r.control.squash == 0 && latch_ex_r.control.rd != 0 && ( latch_ex_r.control.opcode == R_TYPE || latch_ex_r.control.opcode == I_TYPE2) && 
                    (latch_ex_r.control.rd == rs1)) ) return 1;
                return 0;
                break;
            }
            case I_TYPE1: case I_TYPE2: case S_TYPE: {
                unsigned int rs1 = latch_id_r.control.rs1;
                // alu or sw preceded by mem operation
                if ( latch_ex_r.control.squash == 0 && latch_ex_r.control.rd != 0 && latch_ex_r.control.opcode == I_TYPE1 && 
                    (latch_ex_r.control.rd == rs1) ) return 1;
                return 0;
                break;
            }
            case R_TYPE:{
                unsigned int rs1 = latch_id_r.control.rs1;
                unsigned int rs2 = latch_id_r.control.rs2;
                // alu preceded by mem operation
                if ( latch_ex_r.control.squash == 0 && latch_ex_r.control.rd != 0 && latch_ex_r.control.opcode == I_TYPE1 && 
                    (latch_ex_r.control.rd == rs1 || latch_ex_r.control.rd == rs2) ) return 1;
                return 0;
                break;
            }
            case U_TYPE: case UJ_TYPE: case 23: {return 0; break;}

            
            default:
                printf("Unknown Opcode\n");
        }
        return 0;
        
    }
};

int main (int argc, char * argv[]) {

    string filename;
    int cycle_count = 6;
    
    if (argc != 3) {
        cout << "Usage: ./executable [filename] [cycle_count]" << endl;

        filename = "../inputfiles/testcases/test1.txt";
    } else {
        filename = argv[1];
        string cycle_count_str = argv[2];
        cycle_count = stoi(cycle_count_str, nullptr, 10);
    }

    Processor_Forwarding processor(filename, cycle_count);
    processor.run();
    processor.print();   
}