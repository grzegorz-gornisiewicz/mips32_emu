#include <iostream>
#include <string>
#include <vector>

#include "ibus.h"

using namespace std;

#ifndef _MIPS32
#define _MIPS32
#define OPCODE_SHIFT 26
#define OPCODE_MASK 0b111111 << OPCODE_SHIFT
#define FUNCT_MASK 0b111111
#endif // !MIPS32

/*
	Inspired by:
		https://www.youtube.com/watch?v=PlavjNH_RRU&list=PLylNWPMX1lPlmEeeMdbEFQo20eHAJL8hx
		and
		https://github.com/OneLoneCoder/olcNES
*/
typedef enum {
	R0 = 0,//this register always contains 0
	zero = 0,
	
	//reserved temp
	R1 = 1,
	at = 1,

	//returns values
	R2 = 2, R3,
	v0 = 2, v1,

	//procedure arguments
	R4 = 4, R5, R6, R7,
	a0 = 4, a1, a2, a3,

	//temporaries, may BE overwritten by called procedures, caller save,
	R8 = 8, R9, R10, R11, R12, R13, R14, R15,
	t0 = 8, t1, t2, t3, t4, t5, t6, t7,

	//temporaries, may NOT be overwritten by called procedures, callee save
	R16 = 16, R17, R18, R19, R20, R21, R22, R23,
	s0 = 16, s1, s2, s3, s4, s5, s6, s7,

	//temp, caller save
	R24 = 24, R25,
	t8 = 24, t9,

	//reserved for operationg sys
	R26 = 26, R27,
	k0 = 26, k1,

	//global pointer
	R28 = 28,
	gp = 28,

	//stack pointer, for MIPS grows down, calle save
	R29 = 29,
	sp = 29,

	//frame pointer, calle save
	R30 = 30,
	fp = 30,

	//return address, calle save
	R31 = 31,
	ra = 31,
} Regs;

typedef struct {
	uint32_t funct : 6;
	uint32_t shmt : 5;
	uint32_t rd : 5;
	uint32_t rt : 5;
	uint32_t rs : 5;
	uint32_t op : 6;
} Register;

typedef struct {
	uint16_t arg : 16;
	uint16_t rt : 5;
	uint16_t rs : 5;
	uint16_t op : 6;
} Immediate;

typedef struct {
	uint32_t target_address : 26;
	uint32_t op : 6;
} Jump;

typedef union {
	uint32_t word32;
	uint8_t bytes[4];
} Word;

class MIPS32
{
private:

	struct Opcode
	{
		string mnemonic;
		uint32_t inst;
		void (MIPS32::* func)(void) = nullptr;
		static bool Compare(Opcode a, Opcode b) {
			return a.inst < b.inst;
		}
	};

	IBus* _bus;
	uint32_t _registers[32];
	uint32_t _pc = 0;//program counter
	uint32_t _hi = 0;
	uint32_t _lo = 0;
	bool _break;//TODO: implement CPU state

	uint32_t _clock = 0;
	uint32_t _fetched = 0;//current instruction, fetched from andress stored in PC
	Register* _reg;
	Immediate* _imm;
	Jump* _jmp;
	
	vector<Opcode> _ropcodes;
	vector<Opcode> _jopcodes;
	vector<Opcode> _iopcodes;

public:
	MIPS32(IBus* bus);

	bool Tick();

private:
	void InitOpcodes();
	void Fetch();
	void Clear();
	void Execute();
	void Decode();

	//Arithmetic and logical instructions
	void ADD();
	void ADDU();
	void ADDI();
	void ADDIU();
	void AND();
	void ANDI();
	void DIV();
	void DIVU();
	void MULT();
	void MULTU();
	void NOR();
	void OR();
	void ORI();
	void SLL();
	void SLLV();
	void SRA();
	void SRAV();
	void SRL();
	void SRLV();
	void SUB();
	void SUBU();
	void XOR();
	void XORI();

	//Constant - manipulating instructions
	void LHI();
	void LLO();

	//Comparison instructions
	void SLT();
	void SLTU();
	void SLTI();
	void SLTIU();

	//Branch instructions
	void BEQ();
	void BGEZ();
	void BGEZAL();
	void BGTZ();
	void BLEZ();
	void BLTZ();
	void BLTZAL();
	void BNE();
	void BREAK();

	//Jump instructions
	void J();
	void JAL();
	void JALR();
	void JR();

	//Load instructions
	void LB();
	void LBU();
	void LH();
	void LHU();
	void LW();

	//Store instructions
	void SB();
	void SH();
	void SW();

	//Data movement instructions
	void MFHI();
	void MFLO();
	void MTHI();
	void MTLO();

	//Exception and interrupt instructions
	void TRAP();

	//System call
	void SYSCALL();

	//Coprocessor
	void MFC0();
	void MTC0();
};
