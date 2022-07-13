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

/*
* example programs: https://www.csfieldguide.org.nz/en/interactives/mips-assembler/
*/
vector<uint32_t> program = {
    0x2408001e,
    0x24090014,
    0x01092020,
    0x24020001,
    0x0000000c,
    0x240400c0,
    0x00042400,
    0x24840000,
    0x24020004,
    0x0000000c,
    0x24020000,
    0x03e00008,
};

vector<uint32_t> data1 = {
    0x0a48656c,
    0x6c6f2057,
    0x6f726c64,
    0x210a0000
};

vector<uint32_t> complex = {
    0x24080005,
    0x24090000,
    0x1109000a,
    0x00082020,
    0x24020001,
    0x0000000c,
    0x24020004,
    0x240400c0,
    0x00042400,
    0x24840008,
    0x0000000c,
    0x2108ffff,
    0x08400008,
    0x24020004,
    0x240400c0,
    0x00042400,
    0x24840000,
    0x0000000c,
    0x24020000,
    0x03e00008
};

vector<uint32_t> data2 = {
    0x474f2121,
    0x2121210a,
    0x00000000,
    0x0a000000
};

void CopyToMemory(uint32_t addr, vector<uint32_t>* source, IBus* bus);

int main()
{
    cout << "MIPS32 EMU\n";
    cout << "Version " << VER_MAJOR << "." << VER_MINOR << " (" << VER_BUILD << ")" << endl;

    //initialize 16MB of RAM (in KBs)
    IBus *bus = new Bus(16 * 1024);
    //feed with test program and data
    uint32_t addr = (uint32_t)0x00400000;
    CopyToMemory(addr, &program, bus);
    addr = (uint32_t)0x00C00000;
    CopyToMemory(addr, &data1, bus);

    MIPS32 *core = new MIPS32(bus);
    core->SetPC(0x00400000);
    
    while (!core->Tick()) {}

    cout << "end of execution" << endl;
}

void CopyToMemory(uint32_t addr, vector<uint32_t>* source, IBus *bus)
{
    for (vector<uint32_t>::iterator i = source->begin(); i != source->end(); i++)
    {
        Word word;
        word.word32 = (uint32_t)*i;

        //it's for x86 platforms, convertion from LITTLE ENDIAN to BIG ENDIAN
        bus->Write(addr + 0, word.bytes[3]);
        bus->Write(addr + 1, word.bytes[2]);
        bus->Write(addr + 2, word.bytes[1]);
        bus->Write(addr + 3, word.bytes[0]);

        addr += 4;
    }

}