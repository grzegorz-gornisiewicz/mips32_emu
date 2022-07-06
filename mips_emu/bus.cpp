#include "bus.h"

uint32_t Bus::Size()
{
	return sizeof(_memory);
}

uint32_t Bus::Read(uint32_t addr) {
	if (addr < sizeof(_memory)) {
		return _memory[addr];
	}

	return 0;
}

void Bus::Write(uint32_t addr, uint32_t data)
{
	_memory[addr] = data;
}

/*
* @param size memory size in KBs
*/
Bus::Bus(uint32_t size)
{
	_memory = new uint8_t[size * 1024];
}
