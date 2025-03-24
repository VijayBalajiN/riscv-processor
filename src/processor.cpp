#include "processor.h"

control_signals::control_signals(unsigned int line, unsigned int funct7, unsigned int rs2, 
                    unsigned int rs1, unsigned int funct3, unsigned int rd, 
                    unsigned int opcode, unsigned int immed, unsigned int squash) 
        : line(line), funct7(funct7), rs2(rs2), rs1(rs1), funct3(funct3), rd(rd), 
          opcode(opcode), immed(immed), squash(squash) {}

control_signals::control_signals() 
        : line(-1), funct7(127), rs2(31), rs1(31), funct3(7), rd(31), 
          opcode(127), immed(1048575), squash(1) {}


Processor::Processor(string filename) {

    stack = new int[2010000];
    this->filename = filename;

    tuple<vector<string>, vector<string>> instruc_tuple = get_code(filename);

    bin_instruc = get<0>(instruc_tuple);
    pretty_instruc = get<1>(instruc_tuple);

    latch_if_l.line = 0;
    latch_if_l.stall = 0;

    clock = 1;
    id_stall = 0;
    if_stall = 0;
}

void Processor::run() {
    for (; clock <= 8; clock++) {
        latch_set();
        
        run_id();
        run_if();
        run_ex();
        run_m();
        run_wb();
    }
    print_csv(pretty_instruc);
}

void Processor::print () {
    for (auto &a : pretty_instruc) {
        cout << a << "\n";
    }
}


void Processor::latch_set () {
    latch_if_r = latch_if_l;
    latch_id_r = latch_id_l;
    latch_ex_r = latch_ex_l;
    latch_m_r = latch_m_l;
    latch_wb_r = latch_wb_l;
}

void Processor::run_if() {
    
    unsigned int line = latch_if_r.line;

    string instruc_hex = bin_instruc[line/4];


    string start_instruc = "";

    if (if_stall == 0) {
        for (int i = 1; i < clock; i++) {
            start_instruc += " ;";
        }
    } 

    pretty_instruc[line/4].append(";"+start_instruc+"IF");

    if (id_stall == 1) {
        if_stall = 1;
        return;
    } else {
        if_stall = 0;
    }


    unsigned int instruc_dec = stoul(instruc_hex, nullptr, 16);


    bitset<32> instruc_bin(instruc_dec);

    string instruc = instruc_bin.to_string();
    string opcode_str = instruc.substr(25, 7);  // opcode [0:6]
    string rd_str = instruc.substr(20, 5);      // rd [7:11]
    string funct3_str = instruc.substr(17, 3);  // funct3 [12:14]
    string rs1_str = instruc.substr(12, 5);     // rs1 [15:19]
    string rs2_str = instruc.substr(7, 5);      // rs2 [20:24]
    string funct7_str = instruc.substr(0, 7); 


    unsigned int opcode = stoi(opcode_str, nullptr, 2);
    unsigned int rd = stoi(rd_str, nullptr, 2);
    unsigned int funct3 = stoi(funct3_str, nullptr, 2);
    unsigned int rs1 = stoi(rs1_str, nullptr, 2);
    unsigned int rs2 = stoi(rs2_str, nullptr, 2);
    unsigned int funct7 = stoi(funct7_str, nullptr, 2);

    


    switch (opcode) {
        case 51: {
            control_signals control_signal = {
                line, funct7, rs2, rs1, funct3, rd, opcode, 0, 0
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
                line, rs2, rs1, funct3, 0, opcode, immed, 0, 0
            };

            latch_id_l = {
                control_signal
            };
            break;
        }
        case 99: {
            control_signals control_signal = {
                line, 0, rs2, rs1, funct3, 0, opcode, 0, 0
            };

            latch_id_l = {
                control_signal
            };
            break;
            
        }
        case 55: {
            control_signals control_signal = {
                line, 0, 0, 0, 0, rd, opcode, 0, 0
            };
            latch_id_l = {
                control_signal
            };
            break;
        } 
        case 111: {
            string immed_str_20 = instruc.substr(0, 1);
            string immed_str_10_1 = instruc.substr(1, 10);
            string immed_str_11 = instruc.substr(12, 1);
            string immed_str_19_12 = instruc.substr(13, 8);

            unsigned int immed_20 = stoi(immed_str_20, nullptr, 2);
            unsigned int immed_10_1 = stoi(immed_str_10_1, nullptr, 2);
            unsigned int immed_11 = stoi(immed_str_11, nullptr, 2);
            unsigned int immed_19_12 = stoi(immed_str_19_12, nullptr, 2);

            unsigned int immed = (immed_10_1 << 1) | (immed_11 << 11) | 
                        (immed_19_12 << 12) | (immed_20 << 20);

            control_signals control_signal = {
                line, 0, 0, 0, 0, rd, opcode, immed, 0
            };

            latch_id_l = {
                control_signal
            };
            break;
        }
        default:
            printf("Incorrect Opcode %d %d\n", opcode, line);
            
            assert(0);
    }

    

    latch_if_l.line += 4;

}

void Processor::run_id () {
    unsigned int line = latch_id_r.control.line;
    unsigned int opcode = latch_id_r.control.opcode;
    unsigned int squash = latch_id_r.control.squash;

    int stall_check = stall_detector();

    latch_ex_l = {
        latch_id_r.control
    };

    if (stall_check) {
        id_stall = 1;
        latch_ex_l.control.squash = 1;
    } else id_stall = 0;

    if (squash) {
        return;
    }

    pretty_instruc[line/4].append(";ID");
}

void Processor::run_ex () {
    unsigned int line = latch_ex_r.control.line;
    unsigned int squash = latch_ex_r.control.squash;

    latch_m_l = {
        latch_ex_r.control
    };

    if (squash) {
        return;
    }

    pretty_instruc[line/4].append(";EX");
}

void Processor::run_m () {
    unsigned int line = latch_m_r.control.line;
    unsigned int squash = latch_m_r.control.squash;

    latch_wb_l = {
        latch_m_r.control
    };

    if (squash) {
        return;
    }

    pretty_instruc[line/4].append(";MEM");
}

void Processor::run_wb () {
    unsigned int line = latch_wb_r.control.line;
    unsigned int squash = latch_wb_r.control.squash;


    if (squash) {
        return;
    }

    pretty_instruc[line/4].append(";WB");
}


int Processor::stall_detector() {
    // Default implementation
    cout << "Stall detection in base Processor class.\n";
    return 0;
}

