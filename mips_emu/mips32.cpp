#include "mips32.h"

#include <algorithm>
#include <iostream>
#include <bitset>

using namespace std;

bool MIPS32::Tick()
{
	Fetch();
	Execute();
	
	_clock++;

	return _break;
}

void MIPS32::Fetch()
{
	if (_bus != nullptr)
	{
		_fetched = _bus->Read(_pc);
		_break = (_fetched == (uint32_t)0x0);
		_pc += 4;
		if (_pc > _bus->Size())
		{
			cout << "PC overturned" << endl;
			_pc = 0;//overturn _pc when we reach end of memory
		}
	}
}

void MIPS32::Execute()
{
	Decode();
	(this->*_opcode.func)();//call the function
}

void MIPS32::Decode()
{
	uint32_t opcode = _fetched & OPCODE_MASK;
	uint32_t funct = _fetched & FUNCT_MASK;

	//check if it's R-Type
	//R-Type commands op = 0 always
	if (opcode == 0) {
		for (vector<Opcode>::iterator i = _ropcodes.begin(); i != _ropcodes.end(); i++)
		{
			if (opcode == 0 && funct == ((Opcode)*i).inst) {
				_opcode = *i;
				return;
			}
		}
	}

	//check if it's J-Type
	for (vector<Opcode>::iterator i = _jopcodes.begin(); i != _jopcodes.end(); i++)
	{
		if (((Opcode)*i).inst << OPCODE_SHIFT == opcode)
		{
			_opcode = *i;
			return;
		}
	}

	//if it's not R-Type nor J-Type
	//then it should be I-Type
	for (vector<Opcode>::iterator i = _iopcodes.begin(); i != _iopcodes.end(); i++)
	{
		if (((Opcode) *i).inst == opcode)
		{
			_opcode = *i;
			return;
		}
	}

	//unsupported opcode
	printf("usupported opcode %x\n", _fetched);
	cout << bitset<32>{ _fetched } << endl;
	_break = true;
}

//based on:
//https://s3-eu-west-1.amazonaws.com/downloads-mips/documents/MD00086-2B-MIPS32BIS-AFP-6.06.pdf
void MIPS32::InitOpcodes() {
	_ropcodes = {
		//R-Format
		{"sll",    (uint32_t)0b000000, &MIPS32::SLL},//0
		{"srl",    (uint32_t)0b000010, &MIPS32::SRL},//2, unsigned right shift
		{"sra",    (uint32_t)0b000011, &MIPS32::SRA},//3, signed right shift
		{"sllv",   (uint32_t)0b000100, &MIPS32::SLLV},//4
		{"srlv",   (uint32_t)0b000110, &MIPS32::SRLV},//6, unsigned right shift
		{"srav",   (uint32_t)0b000111, &MIPS32::SRAV},//7, signed right shift
		{"jr",	   (uint32_t)0b001000, &MIPS32::JR},//8, R[$rs] must be a multiple of 4
		
		//can have special of jalr $rs form when $rd == 31
		{"jalr",   (uint32_t)0b001001, &MIPS32::JALR},//9, R[$rs] must be a multiple of 4
		
		{"syscall",(uint32_t)0b001100, &MIPS32::SYSCALL},//12
		{"mfhi",   (uint32_t)0b010000, &MIPS32::MFHI},//16
		{"mthi",   (uint32_t)0b010001, &MIPS32::MTHI},//17
		{"mflo",   (uint32_t)0b010010, &MIPS32::MFLO},//18
		{"mtlo",   (uint32_t)0b010011, &MIPS32::MTLO},//19
		{"mult",   (uint32_t)0b011000, &MIPS32::MULT},//24, signed multiplication
		{"multu",  (uint32_t)0b011001, &MIPS32::MULTU},//25, unsigned multiplication
		{"div",    (uint32_t)0b011010, &MIPS32::DIV},//26, signed division
		{"divu",   (uint32_t)0b011011, &MIPS32::DIVU},//27, unsigned division
		{"add",    (uint32_t)0b100000, &MIPS32::ADD},//32, exception on signed overflow
		{"addu",   (uint32_t)0b100001, &MIPS32::ADDU},//33
		{"sub",    (uint32_t)0b100010, &MIPS32::SUB},//34, exception on signed overflow
		{"subu",   (uint32_t)0b100011, &MIPS32::SUBU},//35
		{"and",    (uint32_t)0b100100, &MIPS32::AND},//36
		{"or",     (uint32_t)0b100101, &MIPS32::OR},//37
		{"xor",    (uint32_t)0b100110, &MIPS32::XOR},//38
		{"nor",    (uint32_t)0b100111, &MIPS32::NOR},//39
		{"slt",	   (uint32_t)0b101010, &MIPS32::SLT},//42, signed comparison
		{"sltu",   (uint32_t)0b101011, &MIPS32::SLTU},//43, unsigned comparison
	};

	_jopcodes = {
		//J-Format
		//Jump instructions use pseudo-absolute addressing, in which the upper 4 bits 
		//of the computed address are taken relatively from the program counter.
		{"j",	(uint32_t)0b000010 << 26, &MIPS32::J},//2,
		{"jal", (uint32_t)0b000011 << 26, &MIPS32::JAL},//3,
	};

	_iopcodes = {
		//I-Format
		//These instructions are identified and differentiated by their opcode numbers
		//(any number greater than 3). All of these instructions feature a 16-bit immediate,
		//which is sign-extended to a 32-bit value in every instruction
		//(except for the and, or, and xor instructions which zero-extend
		//and the lui instruction in which it does not matter).
		//Branch instructions also effectively multiply the immediate by 4, to get a byte offset.
		{"beq",	  (uint32_t)0b000100 << 26, &MIPS32::BEQ},//4, branch on equal
		{"bne",   (uint32_t)0b000101 << 26, &MIPS32::BNE},//5, branch on not equal
		{"blez",  (uint32_t)0b000110 << 26, &MIPS32::BLEZ},//6, signed comparison, <= 0
		{"bgtz",  (uint32_t)0b000111 << 26, &MIPS32::BGTZ},//7, signed comparison, > 0
		{"addi",  (uint32_t)0b001000 << 26, &MIPS32::ADDI},//8, exception on signed overflow
		{"addiu", (uint32_t)0b001001 << 26, &MIPS32::ADDIU},//9
		{"slti",  (uint32_t)0b001010 << 26, &MIPS32::SLTI},//10, signed comparison
		{"sltiu", (uint32_t)0b001011 << 26, &MIPS32::SLTIU},//11, unsigned comparison
		{"andi",  (uint32_t)0b001100 << 26, &MIPS32::ANDI},//12
		{"ori",   (uint32_t)0b001101 << 26, &MIPS32::ORI},//13
		{"xori",  (uint32_t)0b001110 << 26, &MIPS32::XORI},//14
		{"lui",   (uint32_t)0b001111 << 26, &MIPS32::LHI},//15

		{"lb",    (uint32_t)0b100000 << 26, &MIPS32::LB},//32		
		{"lh",    (uint32_t)0b100001 << 26, &MIPS32::LH},//33, computed address must be a multiple of 2
		{"lw",    (uint32_t)0b100011 << 26, &MIPS32::LW},//34, computed address must be a multiple of 4
		{"lbu",   (uint32_t)0b100100 << 26, &MIPS32::LBU},//36
		{"lhu",   (uint32_t)0b100101 << 26, &MIPS32::LHU},//37, computed address must be a multiple of 2
		{"sb",    (uint32_t)0b101000 << 26, &MIPS32::SB},//40
		{"sh",    (uint32_t)0b101001 << 26, &MIPS32::SH},//41, computed address must be a multiple of 2
		{"sw",    (uint32_t)0b101011 << 26, &MIPS32::SW},//43, computed address must be a multiple of 4

		/*
		* those 2 below found here:
		* http://alumni.cs.ucr.edu/~vladimir/cs161/mips.html
		* https://student.cs.uwaterloo.ca/~isg/res/mips/opcodes
		* https://uweb.engr.arizona.edu/~ece369/Resources/spim/MIPSReference.pdf
		*/
		{"llo",   (uint32_t)0b011000 << 26, &MIPS32::LLO},//24
		{"lhi",   (uint32_t)0b011001 << 26, &MIPS32::LHI},//25

		//Exception and interrupt instructions
		//https://student.cs.uwaterloo.ca/~isg/res/mips/traps
		{"trap", (uint32_t)0b011010 << 26, &MIPS32::TRAP},
	};

	cout << "Opcodes initialized" << endl;
}

MIPS32::MIPS32(IBus *bus)
{
	InitOpcodes();
	_clock = 0;
	_bus = bus;
	memset(_registers, 0, sizeof(_registers));
}

void MIPS32::LogImm(bool printRT = false) {
	cout << _opcode.mnemonic;
	cout << " " << _reg_names[(Reg)RT(_fetched)];
	cout << ", " << _reg_names[(Reg)RS(_fetched)];
	cout << ", " << hex << IMM(_fetched);
	if (printRT) {
		cout << " => " << _reg_names[(Reg)RT(_fetched)] << " = " << hex << _registers[RT(_fetched)];
	}
	cout << endl;
}

//Arithmeticand logical instructions
void MIPS32::ADD()
{
	cout << "MIPS32::ADD" << endl;
}

void MIPS32::ADDU()
{
}

void MIPS32::ADDI()
{
	
}

void MIPS32::ADDIU()
{
	_registers[RT(_fetched)] = _registers[RS(_fetched)] + IMM(_fetched);
#ifdef _DEBUG
	LogImm(true);
#endif // DEBUG

}

void MIPS32::AND()
{
}

void MIPS32::ANDI()
{
}

void MIPS32::DIV()
{
}

void MIPS32::DIVU()
{
}

void MIPS32::MULT()
{
}

void MIPS32::MULTU()
{
}

void MIPS32::NOR()
{
}

void MIPS32::OR()
{
}

void MIPS32::ORI()
{
}

void MIPS32::SLL()
{
	cout << "MIPS32::SLL" << endl;
}

void MIPS32::SLLV()
{
}

void MIPS32::SRA()
{
}

void MIPS32::SRAV()
{
}

void MIPS32::SRL()
{
}

void MIPS32::SRLV()
{
}

void MIPS32::SUB()
{
}

void MIPS32::SUBU()
{
}

void MIPS32::XOR()
{
}

void MIPS32::XORI()
{
}

//Constant - manipulating instructions
void MIPS32::LHI()
{
}

void MIPS32::LLO()
{
}

//Comparison instructions
void MIPS32::SLT()
{
}

void MIPS32::SLTU()
{
}

void MIPS32::SLTI()
{
}

void MIPS32::SLTIU()
{
}

//Branch instructions
void MIPS32::BEQ()
{
}

void MIPS32::BGTZ()
{
}

void MIPS32::BLEZ()
{
}

void MIPS32::BNE()
{
}

void MIPS32::BGEZ()
{
}

void MIPS32::BGEZAL()
{
}

void MIPS32::BLTZ()
{
}

void MIPS32::BLTZAL()
{
}

//Breakpoint epc=pc; pc=0x3c	000000|code|001101
void MIPS32::BREAK()
{
}

//Jump instructions
void MIPS32::J()
{
}

void MIPS32::JAL()
{
}

void MIPS32::JALR()
{
}

void MIPS32::JR()
{
	cout << "MIPS32::JR" << endl;
}

//Load instructions
void MIPS32::LB()
{
}

void MIPS32::LBU()
{
}

void MIPS32::LH()
{
}

void MIPS32::LHU()
{
}

void MIPS32::LW()
{
}

//Store instructions
void MIPS32::SB()
{
}

void MIPS32::SH()
{
}

void MIPS32::SW()
{
}

//Data movement instructions
void MIPS32::MFHI()
{
}

void MIPS32::MFLO()
{
}

void MIPS32::MTHI()
{
}

void MIPS32::MTLO()
{
}

//Exception and interrupt instructions
void MIPS32::TRAP()
{
}

//Syscall
//System Call	epc=pc; pc=0x3c	000000|00000000000000000000|001100
void MIPS32::SYSCALL()
{
	cout << "MIPS32::SYSCALL" << endl;
}

//Coprocessor
//Move From Coprocessor	rt=CPR[0,rd]	010000|00000|rt|rd|00000000000
void MIPS32::MFC0()
{
}

//Move To Coprocessor	CPR[0,rd]=rt	010000|00100|rt|rd|00000000000
void MIPS32::MTC0()
{
}

