// mips_emu.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//

#include <iostream>

#ifndef MIPS_EMU_VERSION
#define MIPS_EMU_VERSION
#define VER_MAJOR 0
#define VER_MINOR 0
#define VER_BUILD 1
#endif

using namespace std;

#include "bus.h"
#include "mips32.h"
#include <bitset>

int main()
{
    cout << "MIPS32 EMU\n";
    cout << "Version " << VER_MAJOR << "." << VER_MINOR << " (" << VER_BUILD << ")" << endl;
    cout << "insruction size: " << sizeof(Inst) << endl;

    IBus *bus = new Bus(1024);

    MIPS32 *core = new MIPS32(bus);

    //Inst a;
    //a.i.op = 0;
    //a.i.rs = R31;
    //a.i.rt = R0;
    //a.i.arg = 0x00ff;
    //
    //Inst b = a;
    //b.i.arg = 0xffff;

    //cout << bitset<32>{*(uint32_t*)&a} << endl;
    //cout << bitset<32>{*(uint32_t*)&b} << endl;
}



