#include<iostream>
#include<sstream>
#include<fstream>
#include<iomanip>
#include<vector>
#include<stdint.h>
#include <bitset>
using namespace std;
static vector<uint32_t> instruction(1);
static int total_ins_size = 0;
static int cc = 0;

//IF
static int current_pc = 0;
static int added_pc = 0;
static int cur_inst = 0;
static bool if_read = 1;
static bool pc_mux = 0;

//ID
static int ReadData1r = 0;
static int ReadData2r = 0;
static int sign_ext = 0;
static int rs = 0;
static int rt = 0;
static int rd = 0;
static int control1 = 0;
static int control1_mux = 0;
static int added_pc2 = 0;
static int current_pc2 = 0;
static bool id_read = 1;

//EX
static int rs2 = 0;
static int rt2 = 0;
static int ALUout3 = 0;
static int WriteData = 0;
static int rt_rd = 0;
static int control2 = 0;

//MEM
static int ReadDatam = 0;
static int ALUout4 = 0;
static int control3 = 0;
static int rt_rd4 = 0;

//WB
static int rt_rd5 = 0;
static int ALUout5 = 0;

static int regs[32];
static char mems[40];

int to_machine_code(string&);
void add_instruction(int);
void init_mips_reg_and_mems();
void mips_execute(ostream&);
void mips_close();
void output_message(ostream&);

void fetch_instruction();
void instruction_decode();
void execute_calculation();
void memory_access();
void write_back();
void hazard_detection_unit();

int getControl(int instruction_id);

int main(int argc, char* argv[]){
    ifstream fin;
    ofstream fout;
    string str_inst;
    int start_inst = 0;
    instruction.resize(0);
    fin.open("General.txt");
    if(!fin){
        cerr << "Cannot Open File!" << endl;
        return 1;
    }
    while(!fin.eof()){
        fin >> str_inst;
        add_instruction(to_machine_code(str_inst));
    }
    fin.close();
    init_mips_reg_and_mems();
    fout.open("genResult.txt");
    if(!fout){
        cerr << "Cannot Open File!" << endl;
        return 1;
    }
    mips_execute(fout);
    fout.close();

    fin.open("Datahazard.txt");
    if(!fin){
        cerr << "Cannot Open File!" << endl;
        return 1;
    }
    while(!fin.eof()){
        fin >> str_inst;
        add_instruction(to_machine_code(str_inst));
    }
    fin.close();
    init_mips_reg_and_mems();
    fout.open("dataResult.txt");
    if(!fout){
        cerr << "Cannot Open File!" << endl;
        return 1;
    }
    mips_execute(fout);
    fout.close();

    fin.open("Lwhazard.txt");
    if(!fin){
        cerr << "Cannot Open File!" << endl;
        return 1;
    }
    while(!fin.eof()){
        fin >> str_inst;
        add_instruction(to_machine_code(str_inst));
    }
    fin.close();
    init_mips_reg_and_mems();
    fout.open("loadResult.txt");
    if(!fout){
        cerr << "Cannot Open File!" << endl;
        return 1;
    }
    mips_execute(fout);
    fout.close();

    fin.open("Branchhazard.txt");
    if(!fin){
        cerr << "Cannot Open File!" << endl;
        return 1;
    }
    while(!fin.eof()){
        fin >> str_inst;
        add_instruction(to_machine_code(str_inst));
    }
    fin.close();
    init_mips_reg_and_mems();
    fout.open("branchResult.txt");
    if(!fout){
        cerr << "Cannot Open File!" << endl;
        return 1;
    }
    mips_execute(fout);
    fout.close();
    return 0;
}

int to_machine_code(string &str){
    int result = 0;
    for(int i=0;i<str.length();i++){
        result <<= 1;
        result += (str[i] & 1);
    }
    return result;
}

void add_instruction(int code){
    instruction.push_back(code);
    total_ins_size = total_ins_size + 4;
}

void init_mips_reg_and_mems(){
    for(int i=0;i<4;i++){
        instruction.push_back(0);
    }

    regs[0] = 0; regs[1] = 9; regs[2] = 8; regs[3] = 7;
    regs[4] = 1; regs[5] = 2; regs[6] = 3; regs[7] = 4;
    regs[8] = 5; regs[9] = 6;
    mems[0] = 5; mems[4] = 9; mems[8] = 4; mems[12] = 8; mems[16] = 7;
}

void mips_execute(ostream& output){
    int terminate_cc = (total_ins_size/4) + 3;
    cc = cc + 1;
    fetch_instruction();
    output_message(output);
    while(cc<=terminate_cc){
        cc = cc + 1;
        write_back();
        memory_access();
        execute_calculation();
        instruction_decode();
        hazard_detection_unit();
        fetch_instruction();

        output_message(output);
        if(if_read == 0 || id_read == 0){
            terminate_cc++;
        }
        if(pc_mux){
            terminate_cc -= sign_ext;
            terminate_cc++;
        }
    }
    mips_close();
}

void mips_close(){
    instruction.resize(0);

    total_ins_size = 0;
    cc = 0;

    //IF
    current_pc = 0;
    added_pc = 0;
    cur_inst = 0;
    if_read = 1;
    pc_mux = 0;

    //ID
    ReadData1r = 0;
    ReadData2r = 0;
    sign_ext = 0;
    rs = 0;
    rt = 0;
    rd = 0;
    control1 = 0;
    control1_mux = 0;
    id_read = 1;

    //EX
    rs2 = 0;
    rt2 = 0;
    ALUout3 = 0;
    WriteData = 0;
    rt_rd = 0;
    control2 = 0;

    //MEM
    ReadDatam = 0;
    ALUout4 = 0;
    control3 = 0;
    rt_rd4 = 0;

    //WB
    rt_rd5 = 0;
    ALUout5 = 0;
}

void fetch_instruction(){
    if(if_read){
        current_pc = added_pc;
        void* ins_ptr = (void*)&instruction[0];
        ins_ptr = ins_ptr + current_pc;
        cur_inst = *(int *)ins_ptr;
        added_pc = added_pc + 4;
    }
}

void instruction_decode(){
    short temp;
    if(id_read){        //not lw hazard
        if(pc_mux){               //branch taken
            added_pc = added_pc2;
            cur_inst = 0;
            pc_mux=0;
        }
        current_pc2 = added_pc;
        control1 = getControl((cur_inst>>26)&0x3f);     //getControl(instruction(31~26))
        rs = (cur_inst>>21)&0x1f;       //instruction(25~21)
        rt = (cur_inst>>16)&0x1f;       //instruction(20~16)
        rd = (cur_inst>>11)&0x1f;       //instruction(15~11)
        temp = cur_inst & 0xffff;   //instruction(15~0)
        sign_ext = (int)temp;
        added_pc2 = current_pc2 + (sign_ext<<2);
        ReadData1r = regs[rs];
        ReadData2r = regs[rt];
        if(((cur_inst>>26)&0x3f) == 0b000100){  //beq
            if(!(ReadData1r ^ ReadData2r)){
                pc_mux = 1;
            }
        }
        if(((cur_inst>>26)&0x3f) == 0b000101){  //bne
            if((ReadData1r ^ ReadData2r)){
                pc_mux = 1;
            }
        }
    }
}

void execute_calculation(){
    int temp1, temp2;
    rt_rd = ((control1_mux>>8)&1)? rd : rt; //RegDst
    rs2 = rs;
    rt2 = rt;
    WriteData = ReadData2r;
    if(rs == rt_rd4 && (control2&3) != 0)            //is data hazard
        temp1 = ALUout4;
    else if(rs == rt_rd5)
        temp1 = ALUout5;
    else
        temp1 = ReadData1r;

    temp2 = ((control1_mux>>5)&1)? sign_ext : ReadData2r;    //ALUsrc
    if(temp2 == ReadData2r){
        if(rt == rt_rd4 && (control2&3) != 0){  //RegWrite & Mem to Reg and is_data_hazard
            temp2 = ALUout4;
            WriteData = ALUout4;
        }
        else if(rt == rt_rd5){
            temp2 = ALUout5;
            WriteData = ALUout5;
        }
    }

    switch((control1_mux>>6)&3){
    case 0:         //ALUop = 00
        ALUout3 = temp1 + temp2;
        break;
    case 1:         //ALUop = 01
        ALUout3 = temp1 - temp2;
        break;
    case 2:
        switch(sign_ext & 0x3f){
        case 0b100000:      //add
            ALUout3 = temp1 + temp2;
            break;
        case 0b100010:      //subtract
            ALUout3 = temp1 - temp2;
            break;
        case 0b100100:      //and
            ALUout3 = temp1 & temp2;
            break;
        case 0b100101:      //or
            ALUout3 = temp1 | temp2;
            break;
        case 0b101010:      //set_less_than
            ALUout3 = (temp1 < temp2);
            break;
        }
        break;
    case 3:         //ALUop = 0b11
        ALUout3 = temp1 & temp2;
        break;
    }
    control2 = (control1_mux & 0x1f);
}

void memory_access(){
    rt_rd4 = rt_rd;
    ALUout4 = ALUout3;
    if((control2>>3)&1){
        ReadDatam = *(int *)&mems[ALUout3];
    }
    else{
        ReadDatam = 0;
    }
    if((control2>>2)&1){
        *(int *)&mems[ALUout3] = WriteData;
    }
    control3 = control2 & 3;
}

void write_back(){
    rt_rd5 = rt_rd4;
    ALUout5 = (control3&1)? ReadDatam : ALUout4;
    if((control3>>1)&1){
        regs[rt_rd4] = ALUout5;
    }
}

int getControl(int instruction_id){
    switch(instruction_id){
    case 0b000000:          //R-Type instruction
        if(cur_inst&0x3f){
            return 0b110000010;
        }else{
            return 0;
        }
    case 0b100011:          //lw instruction
        return 0b000101011;
    case 0b101011:          //sw instruction
        return 0b000100100;
    case 0b000100:          //beq instruction
    case 0b000101:          //bne instruction
        return 0b001010000;
    case 0b001000:          //addi instruction
        return 0b000100010;
    case 0b001100:          //andi instruction
        return 0b011100010;
    default:
        return 0;
    }
}

void hazard_detection_unit(){
    if(((control2>>3)&1) && (rt2 == rs || rt2 == rt)){
        control1_mux = 0;
        if_read = 0;
    }else{
        control1_mux = control1;
        if_read = 1;        //instruction fetch
    }
}

void output_message(ostream& output){
    output << "CC " << cc << ":" << endl;
    output << endl;
    output << "Registers:" << endl;
    output << "$0:" << regs[0] << endl;
    output << "$1:" << regs[1] << endl;
    output << "$2:" << regs[2] << endl;
    output << "$3:" << regs[3] << endl;
    output << "$4:" << regs[4] << endl;
    output << "$5:" << regs[5] << endl;
    output << "$6:" << regs[6] << endl;
    output << "$7:" << regs[7] << endl;
    output << "$8:" << regs[8] << endl;
    output << "$9:" << regs[9] << endl;
    output << endl;
    output << "Data memory:" << endl;
    output << uppercase;
    output << "0x" << setfill('0') << setw(2) << hex << 0 << ":" << dec << *(int *)&mems[0] << endl;
    output << "0x" << setfill('0') << setw(2) << hex << 4 << ":" << dec << *(int *)&mems[4] << endl;
    output << "0x" << setfill('0') << setw(2) << hex << 8 << ":" << dec << *(int *)&mems[8] << endl;
    output << "0x" << setfill('0') << setw(2) << hex << 12 << ":" << dec << *(int *)&mems[12] << endl;
    output << "0x" << setfill('0') << setw(2) << hex << 16 << ":" << dec << *(int *)&mems[16] << endl;
    output << endl;
    output << "IF/ID :" << endl;
    output << "PC\t\t" << added_pc << endl;
    output << "Instruction\t" << bitset<32>(cur_inst) << endl;
    output << endl;
    output << "ID/EX :" << endl;
    output << "ReadData1\t" << ReadData1r << endl;
    output << "ReadData2\t" << ReadData2r << endl;
    output << "sign_ext\t" << sign_ext << endl;
    output << "Rs\t\t" << rs << endl;
    output << "Rt\t\t" << rt << endl;
    output << "Rd\t\t" << rd << endl;
    output << "Control signals\t" << bitset<9>(control1_mux) << endl;
    output << endl;
    output << "EX/MEM :" << endl;
    output << "ALUout\t\t" << ALUout3 << endl;
    output << "WriteData\t" << WriteData << endl;
    output << "Rt/Rd\t\t" << rt_rd << endl;
    output << "Control signals " << bitset<5>(control2) << endl;
    output << endl;
    output << "MEM/WB :" << endl;
    output << "ReadData\t" << ReadDatam << endl;
    output << "ALUout\t\t" << ALUout4 << endl;
    output << "Rt/Rd\t\t" << rt_rd4 << endl;
    output << "Control signals " << bitset<2>(control3) << endl;
    output << "=================================================================" << endl;
}
