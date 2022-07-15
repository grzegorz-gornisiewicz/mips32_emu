#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "ibus.h"

using namespace std;

#ifndef _MIPS32
#define _MIPS32
#define J_SHIFT 28
#define OPCODE_SHIFT 26
#define SHAMT_SHIFT 6
#define RS_SHIFT 21
#define RT_SHIFT 16
#define RD_SHIFT 11
#define OPCODE_MASK (uint32_t)(0b111111 << OPCODE_SHIFT)
#define RS_MASK (uint32_t)(0b11111 << RS_SHIFT)
#define RT_MASK (uint32_t)(0b11111 << RT_SHIFT)
#define RD_MASK (uint32_t)(0b11111 << RD_SHIFT)
#define IMM_MASK (uint32_t)0x0000ffff
#define FUNCT_MASK (uint32_t)0b0111111
#define SHAMT_MASK (uint32_t)(0b11111 << SHAMT_SHIFT)
#define J_MASK (uint32_t)(0b1111 << J_SHIFT)
#define RS(data) (uint32_t)((data & RS_MASK) >> RS_SHIFT)
#define RT(data) (uint32_t)((data & RT_MASK) >> RT_SHIFT)
#define RD(data) (uint32_t)((data & RD_MASK) >> RD_SHIFT)
#define SHAMT(data) (uint32_t)((data & SHAMT_MASK) >> SHAMT_SHIFT)
#define IMM(data) (uint32_t)(data & IMM_MASK)
#define J_TARGET(data) (uint32_t)(data & ~OPCODE_MASK)
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
} Reg;

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
		uint32_t inst = 0x0;//nop
		void (MIPS32::* func)() = nullptr;
	};

	map<Reg, string> _reg_names = {
		{ R0, "$zero"},
		{ R1, "$at"},
		{ R2, "$v0"}, { R3, "$v1"},
		{ R4, "$a0"}, { R5, "$a1"},	{ R6, "$a2"}, { R7, "$a3"},
		
		{ R8, "$t0"}, { R9, "$t1"},	{ R10, "$t2"}, { R11, "$t3"},
		{ R12, "$t4"}, { R13, "$t5"}, { R14, "$t6"}, { R15, "$t7"},
		
		{ R16, "$s0"}, { R17, "$s1"}, { R18, "$s2"}, { R19, "$s3"},
		{ R20, "$s4"}, { R21, "$s5"}, { R22, "$s6"}, { R23, "$s7"},
		
		{ R24, "$t8"}, { R25, "$t9"},
		
		{ R26, "$k0"}, { R27, "$k1"},
		
		{ R28, "$gp"},
		{ R29, "$sp"},
		{ R30, "$fp"},
		{ R31, "$ra"},
	};

	bool _logEnabled = false;

	IBus* _bus;
	uint32_t _registers[32];
	uint32_t _pc = 0;//program counter
	uint32_t _hi = 0;
	uint32_t _lo = 0;
	bool _break;//TODO: implement CPU state

	uint32_t _clock = 0;
	uint32_t _fetched = 0;//current instruction, fetched from andress stored in PC
	Opcode _opcode;

	vector<Opcode> _ropcodes;
	vector<Opcode> _jopcodes;
	vector<Opcode> _iopcodes;

public:
	MIPS32(IBus* bus);

	void SetPC(uint32_t addr);
	uint32_t GetPC();
	void EnableLog(bool enable = true);
	
	bool Tick();

private:
	void InitOpcodes();
	void Fetch();
	void Execute();
	void Decode();

	void LogImm(bool printRT);
	void LogReg(bool printRD);
	void LogShift(bool printRT);
	void LogJr(bool printRA);
	void LogJ();

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

	//No operation
	void NOP();
};
