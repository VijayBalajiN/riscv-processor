#include "processor.h"

string sanitize_string(const string& input) {
    string result;
    for (unsigned char c : input) {
        if (c < 128) { // Only keep ASCII characters
            result.push_back(c);
        } else {
            result.push_back(' '); // Replace non-ASCII with space
        }
    }
    return result;
}

int max_length(vector<string> v){
    int length = 0;
    for (int i = 0; i < v.size(); i++) {
        length =((count_char(v[i], ';')+1) > length ? (count_char(v[i], ';')+1) : length);
    }
    return length;
}

control_signals::control_signals(unsigned int line, unsigned int funct7, unsigned int rs2, 
                    unsigned int rs1, unsigned int funct3, unsigned int rd, 
                    unsigned int opcode, unsigned int immed, unsigned int squash) 
        : line(line), funct7(funct7), rs2(rs2), rs1(rs1), funct3(funct3), rd(rd), 
          opcode(opcode), immed(immed), squash(squash) {}

control_signals::control_signals() 
        : line(-1), funct7(127), rs2(31), rs1(31), funct3(7), rd(31), 
          opcode(127), immed(1048575), squash(1) {}


Processor::Processor(string filename, int cycle_count) {

    stack = new int[2010000];   //  Processor::~Processor() {
                                //     delete[] stack;
                                //  }
    this->filename = filename;
    this->cycle_count = cycle_count;

    tuple<vector<string>, vector<string>> instruc_tuple = get_code(filename);

    bin_instruc = get<0>(instruc_tuple);
    pretty_instruc = get<1>(instruc_tuple);

    this->tot_lines = pretty_instruc.size();

    latch_if_l.line = 0;
    latch_if_l.stall = 0;

    clock = 1;
    id_stall = 0;
    if_stall = 0;
    branch = 0;

    clock_end.assign(bin_instruc.size(), 0);

    for (int i = 0; i < 32; i++) registers[i] = 0;
}

void Processor::run() {
    for (; clock <= cycle_count; clock++) {
        branch = 0;
        latch_set();

        run_wb1(); //First half of cycle wb writes into registers before reading is performed
        run_id();
        run_if();
        
        run_m();
        run_wb();
        run_ex();
        
    }
    print_csv(pretty_instruc);
    print_registers();
}

void Processor::print () {
    printf("\n");
    int l = max_length(pretty_instruc);
    int first = 1;
    for (int i = 0; i <= l; i++){
        if (first){printf("%s", "Instruction          ");first=0;}
        else{
            string s_i = to_string(i);
            printf("C");
            printf("%d", i);
            for (int i = s_i.length(); i < 7; i++){
                cout << " ";
            }
            // printf("C%3d", i);
        }

    }
    cout << endl;
    for (string a: pretty_instruc){
        a = sanitize_string(a);
        
        int first = 1;
        int count=0;
        for (int i = 0; i<a.size(); i++){
            if (a[i] == ';'){
                if (first){
                    while (count < 20){
                        cout << " "; count++;    
                    }
                    first = 0;
                }
                else {
                    while (count < 7){
                        cout << " "; count++;
                    }
                }
                cout << ';'; count = 0;
            }
            else{
                cout << a[i]; count++;
            }
        }
        cout << endl;
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

    if (line/4 >= tot_lines) {
        if (id_stall == 0) latch_id_l.control.squash = 1;
        return;
    }

    string instruc_hex = bin_instruc[line/4];


    string start_instruc = "";

    
    for (int i = 1; i < clock-clock_end[line/4]; i++) {
        start_instruc += " ;";
    }
   
    char check_stall = ' ';

    if (start_instruc == "") {
        int pos = pretty_instruc[line/4].size()-1;
        while (pretty_instruc[line/4][pos] == '-' || pretty_instruc[line/4][pos] == ';') {
            pos --;
        }
        check_stall = pretty_instruc[line/4][pos];
    }
    if (check_stall == 'F') pretty_instruc[line/4].append(";-");
    else pretty_instruc[line/4].append(";"+start_instruc+"IF");  // why append ";" at the start explain me once you read this
    clock_end[line/4] = clock;

    if (id_stall == 1 || branch == 1) {
        return;
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

    
    // 51 -> R-type
    // 3, 19, 103 -> I-type
    // 35 -> S-type
    // 99 -> SB-type
    // 55 -> U-type
    // 111 -> UJ-type

    switch (opcode) {
        case R_TYPE: {                                                     
            control_signals control_signal = {
                line, funct7, rs2, rs1, funct3, rd, opcode, 0, 0
            };
            latch_id_l = {
                control_signal
            };
            break;
        }
        case I_TYPE1: case I_TYPE2: case I_TYPE3: {     // should jalr be handled separetly? also slli, srai, srli
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
        case S_TYPE: {
            string immed_str_1 = instruc.substr(0, 7);
            string immed_str_2 = instruc.substr(20, 5);

            int immed_1 = stoi(immed_str_1, nullptr, 2);
            int immed_2 = stoi(immed_str_2, nullptr, 2);

            unsigned int immed = (immed_1 << 5) | immed_2;

            control_signals control_signal = {
                line, 0, rs2, rs1, funct3, 0, opcode, immed, 0
            };

            latch_id_l = {
                control_signal
            };
            break;
        }
        case SB_TYPE: {
            string immed_str_12 = instruc.substr(0, 1);
            string immed_str_10_5 = instruc.substr(1, 6);
            string immed_str_4_1 = instruc.substr(20, 4);
            string immed_str_11 = instruc.substr(24, 1);

            unsigned int immed_12 = stoi(immed_str_12, nullptr, 2);
            unsigned int immed_10_5 = stoi(immed_str_10_5, nullptr, 2);
            unsigned int immed_4_1 = stoi(immed_str_4_1, nullptr, 2);
            unsigned int immed_11 = stoi(immed_str_11, nullptr, 2);

            unsigned int immed = (immed_12 << 12) | (immed_11 << 11) | 
                        (immed_10_5 << 5) | (immed_4_1 << 1);

            control_signals control_signal = {
                line, 0, rs2, rs1, funct3, 0, opcode, immed, 0   // add immed values (copy paste from prev case?)
            };

            latch_id_l = {
                control_signal
            };
            break;
            
        }
        case U_TYPE: {
            string immed_str = instruc.substr(0, 20);

            unsigned int immed = stoi(immed_str, nullptr, 2);

            control_signals control_signal = {
                line, 0, 0, 0, 0, rd, opcode, immed, 0          // add immed values 31-12
            };
            latch_id_l = {
                control_signal
            };
            break;
        } 
        case UJ_TYPE: {
            string immed_str_20 = instruc.substr(0, 1);
            string immed_str_10_1 = instruc.substr(1, 10);
            string immed_str_11 = instruc.substr(11, 1);
            string immed_str_19_12 = instruc.substr(12, 8);

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
    unsigned int immed = latch_id_r.control.immed;
    unsigned int funct3 = latch_id_r.control.funct3;

    latch_id_r.operand1 = registers[latch_id_r.control.rs1];
    latch_id_r.operand2 = registers[latch_id_r.control.rs2];

    


    latch_ex_l = {
        latch_id_r.control
    };

    if (squash) {
        return;
    }

    int stall_check = stall_detector();

    

    if (stall_check) {
        id_stall = 1;
        latch_ex_l.control.squash = 1;
    } else {
        id_stall = 0;

        forwarding_id();

        int operand1 = latch_id_r.operand1;
        int operand2 = latch_id_r.operand2;
        int line_displacement;
        int jump;

        switch (opcode) {
            case R_TYPE:
                latch_ex_l.operand1 = operand1;
                latch_ex_l.operand2 = operand2;
                latch_ex_l.write_back = 1;
                latch_ex_l.use_alu = 1;
                break;

            case I_TYPE2:
                latch_ex_l.operand1 = operand1;
                latch_ex_l.operand2 = immed;
                latch_ex_l.write_back = 1;
                latch_ex_l.use_alu = 1;
                break;

            case I_TYPE1:

                latch_ex_l.operand1 = operand1;
                latch_ex_l.operand2 = immed;
                latch_ex_l.use_alu = 1;
                latch_ex_l.mem_read = 1;
                latch_ex_l.write_back = 1;
                break;

            case S_TYPE:
                latch_ex_l.mem_write_val = registers[latch_id_r.control.rs2];
                latch_ex_l.mem_write = 1;
                latch_ex_l.operand1 = operand1;
                latch_ex_l.operand2 = immed;
                latch_ex_l.use_alu = 1;
                break;
                
            case UJ_TYPE:
                
                
                if ((1<<20) & immed) { //immed is negative
                    line_displacement = (int)immed - 2 * (1 << 20);
                } else {
                    line_displacement = (int)immed;
                }

                latch_if_l.line = (unsigned int)((int)line + line_displacement);
                branch = 1;
                latch_id_l.control.squash = 1;
                
                break;
            case I_TYPE3: 
                

                if ((1<<11) & immed) {
                    line_displacement = (int)immed - 2 * (1<<11);
                } else {
                    line_displacement = (int)immed;
                }
                latch_ex_l.use_alu = 1;

                latch_if_l.line = (unsigned int)((int)line + line_displacement);
                branch = 1;
                latch_id_l.control.squash = 1;
            case SB_TYPE:
                

                if ((1<<12) & immed) {
                    line_displacement = (int)immed - 2 * (1<<12);
                } else {
                    line_displacement = (int)immed;
                }

                jump = 0;

                switch (funct3) {
                    case 0:
                        if (operand1 == operand2) jump = 1;
                        break;
                    case 1:
                        if (operand1 != operand2) jump = 1;
                        break;
                    case 4: case 6:
                        if (operand1 < operand2) jump = 1;
                        break;
                    case 5: case 7:
                        if (operand1 >= operand2) jump = 1;
                        break;
                    default:
                        cout << "Incorrect Opcode " << pretty_instruc[line/4] << endl;
                        assert(0);
                }

                if (jump == 1) {
                    latch_if_l.line = (unsigned int)((int)line + line_displacement);
                    branch = 1;
                    latch_id_l.control.squash = 1;
                }
            default:
                break;
        }
    }

    char check_stall = ' ';
    int pos = pretty_instruc[line/4].size()-1;
    while (pretty_instruc[line/4][pos] == '-' || pretty_instruc[line/4][pos] == ';') {
        pos --;
    }
    check_stall = pretty_instruc[line/4][pos];

    if (check_stall == 'D') pretty_instruc[line/4].append(";-");
    else pretty_instruc[line/4].append(";ID");
    clock_end[line/4] = clock;
}

void Processor::run_ex () {
    unsigned int line = latch_ex_r.control.line;
    unsigned int squash = latch_ex_r.control.squash;
    unsigned int opcode = latch_ex_r.control.opcode;
    unsigned int immed = latch_ex_r.control.immed;
    
    int operand1 = latch_ex_r.operand1;
    int operand2 = latch_ex_r.operand2;

    

    latch_m_l = {
        latch_ex_r.control, 
        latch_ex_r.mem_write_val,
        latch_ex_r.mem_read,
        latch_ex_r.mem_write,
        latch_ex_r.write_back
    };

    if (squash) {
        return;
    }

    forwarding_ex();



    int alu_output;

    if (latch_ex_r.use_alu == 1) {
        unsigned int funct3 = latch_ex_r.control.funct3;
        unsigned int funct7 = latch_ex_r.control.funct7;

        switch(funct3) {
            case 0:
                if (funct7 == 0) alu_output = operand1 + operand2;
                else if (funct7 == 32) alu_output = operand1 - operand2;
                else if (funct7 == 1) alu_output = operand1 * operand2;
                else {
                    cout << pretty_instruc[line/4] << endl;
                    printf("Incorrect Code\n");
                    assert(0);
                }
                break;
            case 1:
                alu_output = operand1 << (operand2 & 0x1F);
                break;
            case 4:
                if (funct7 == 0 && operand2 == 0) printf("Division By Zero Error\n");
                if (funct7 == 1) alu_output = (operand2 == 0 ? 0 : operand1 / operand2);
                else alu_output = operand1 ^ operand2;
                break;
            case 5:
                alu_output = operand1 >> (operand2 & 0x1F);
                break;
            case 6:
                alu_output = operand1 | operand2;
                break;
            case 7:
                alu_output = operand1 & operand2;
                break;
            case 2:
                alu_output = operand1 + operand2;
                break;
            default:
                cout << pretty_instruc[line/4] << endl;
                printf("Incorrect Code\n");
                assert(0);
        }
    }

    latch_m_l.alu_output = alu_output;

    if (clock_end[line/4] == clock) pretty_instruc[line/4].append("/EX");
    else pretty_instruc[line/4].append(";EX");
    clock_end[line/4] = clock;
}

void Processor::run_m () {
    unsigned int line = latch_m_r.control.line;
    unsigned int squash = latch_m_r.control.squash;

    latch_wb_l = {
        latch_m_r.control, 
        latch_m_r.write_back,
        latch_m_r.alu_output
    };

    if (squash) {
        return;
    }

    forwarding_m();

    int address = latch_m_r.alu_output;

    if (latch_m_r.mem_write == 1) {
        if (address > 0 && address/4 < 2010000)
        stack[address] = latch_m_r.mem_write_val;
    } else if (latch_m_r.mem_read == 1) {
        latch_wb_l.mem_output = stack[address];
    }


    if (clock_end[line/4] == clock) pretty_instruc[line/4].append("/MEM");
    else pretty_instruc[line/4].append(";MEM");
    clock_end[line/4] = clock;
}

void Processor::run_wb () {
    unsigned int line = latch_wb_r.control.line;
    unsigned int squash = latch_wb_r.control.squash;


    if (squash) {
        return;
    }

    if (clock_end[line/4] == clock) pretty_instruc[line/4].append("/WB");
    else pretty_instruc[line/4].append(";WB");
    clock_end[line/4] = clock;
}

void Processor::run_wb1 () {

    if (latch_wb_r.control.squash == 0 && latch_wb_r.write_back == 1 && latch_wb_r.control.rd != 0) {
        registers[latch_wb_r.control.rd] = (unsigned int)latch_wb_r.mem_output;
    }
}


int Processor::stall_detector() {
    // Default implementation
    cout << "Stall detection in base Processor class.\n";
    return 0;
}

void Processor::print_registers() {
    cout << "\nFormatted Register Values:\n";
    int count = 0;
    for (int i = 0; i < 32; ++i) {
        if (count == 8){
            cout << endl;
            count = 0;
        }
        // cout << "  (R" << i << ") " << registers[i] << "  | ";
        printf("  (R%02d) %5d  | ", i, registers[i]);
        count++;
    }
    cout << endl;
}

void Processor::forwarding_ex(){}
void Processor::forwarding_m(){}
void Processor::forwarding_id(){}