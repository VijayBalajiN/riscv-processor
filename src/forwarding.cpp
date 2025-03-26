#include "processor.h"

class Processor_Forwarding : public Processor {
public:
    Processor_Forwarding(string filename) : Processor(filename) {
    }


    int stall_detector() override {
        unsigned int opcode = latch_id_r.control.opcode;

        switch (opcode) {
            case SB_TYPE:  {
                unsigned int rs1 = latch_id_r.control.rs1;
                unsigned int rs2 = latch_id_r.control.rs1;
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
            case I_TYPE2: case S_TYPE: {
                unsigned int rs1 = latch_id_r.control.rs1;
                // alu or sw preceded by mem operation
                if ( latch_ex_r.control.squash == 0 && latch_ex_r.control.rd != 0 && latch_ex_r.control.opcode == I_TYPE1 && 
                    (latch_ex_r.control.rd == rs1) ) return 1;
                return 0;
                break;
            }
            case R_TYPE:{
                unsigned int rs1 = latch_id_r.control.rs1;
                unsigned int rs2 = latch_id_r.control.rs1;
                // alu preceded by mem operation
                if ( latch_ex_r.control.squash == 0 && latch_ex_r.control.rd != 0 && latch_ex_r.control.opcode == I_TYPE1 && 
                    (latch_ex_r.control.rd == rs1 || latch_ex_r.control.rd == rs2) ) return 1;
                return 0;
                break;
            }
            case I_TYPE1: case U_TYPE: case UJ_TYPE: {return 0; break;}

            
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