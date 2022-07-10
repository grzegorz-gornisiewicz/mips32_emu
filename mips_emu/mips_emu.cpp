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
.data
str:  .asciiz "\nHello World!\n"
# You can change what is between the quotes if you like

.text
.globl main

main:
# Do the addition
# For this, we first need to put the values
# to add into registers ($t0 and $t1)
# You can change the 30 below to another value
li $t0, 30
# You can change the 20 below to another value
li $t1, 20

# Now we can add the values in $t0
# and $t1, putting the result in special register $a0
add $a0, $t0, $t1

# Set up for printing the value in $a0.
# A 1 in $v0 means we want to print an integer
li $v0, 1

# The system call looks at what is in $v0
# and $a0, and knows to print what is in $a0
syscall

# Now we want to print Hello World
# So we load the (address of the) string into $a0.
# The address of the string is too big to be stored
# by one instruction, so we first load the upper half,
# shift it across, then load the lower half
la $a0, str

# And put a 4 in $v0 to mean print a string
li $v0, 4

# And just like before syscall looks at
# $v0 and $a0 and knows to print the string
syscall

# Nicely end the program
li $v0, 0
jr $ra
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
        0x0a48656c,
        0x6c6f2057,
        0x6f726c64,
        0x210a0000,
};

int main()
{
    cout << "MIPS32 EMU\n";
    cout << "Version " << VER_MAJOR << "." << VER_MINOR << " (" << VER_BUILD << ")" << endl;

    //initialize RAM
    IBus *bus = new Bus(1024);
    
    //feed with test program and data
    uint32_t addr = (uint32_t)0x0;
    
    for (vector<uint32_t>::iterator i = program.begin(); i != program.end(); i++)
    {
        Word word;
        word.word32 = (uint32_t)*i;

        //TODO: consider to add Write/Read uint32_t functions
        //it's for x86 platforms, convertion from LITTLE ENDIAN to BIG ENDIAN
        bus->Write(addr + 0, word.bytes[3]);
        bus->Write(addr + 1, word.bytes[2]);
        bus->Write(addr + 2, word.bytes[1]);
        bus->Write(addr + 3, word.bytes[0]);

        addr += 4;
    }

    MIPS32 *core = new MIPS32(bus);

    while (!core->Tick()) {
    }

    cout << "end of execution" << endl;
    //cout << i->mnemonic << " ";
    //cout << bitset<6>{(* (uint32_t*)&i->inst) >> 26} << ":";
    //cout << bitset<6>{masked >> 26} << endl;
}
