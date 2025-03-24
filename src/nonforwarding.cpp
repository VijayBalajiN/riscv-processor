#include "processor.h"

class Processor_NonForwarding : public Processor {
public:
    int stall_detector() override {

        if (latch_id_r.control.rd != 0 && 
            (latch_ex_r.control.rs1 == latch_id_r.control.rd || 
             latch_ex_r.control.rs2 == latch_id_r.control.rd)) {
            return 1; // Stall required
        }
        return 0; // No stall required
    }
};

int main () {
    Processor processor("../inputfiles/strlen.txt");
    processor.run();
    processor.print();   
}