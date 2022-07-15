#include "bus.h"
#include <iostream>

using namespace std;

uint32_t Bus::Size()
{
	return _mem.size();
}

uint32_t Bus::Read(uint32_t addr) {
	if (addr < _mem.size()) {
		uint8_t b0 = _mem.at(addr + 0);
		uint8_t b1 = _mem.at(addr + 1);
		uint8_t b2 = _mem.at(addr + 2);
		uint8_t b3 = _mem.at(addr + 3);

		return b0 << 24 | b1 << 16 | b2 << 8 | b3;
	}

	return (uint32_t)0x0;
}

void Bus::Write(uint32_t addr, uint8_t data)
{
	if (addr < _mem.size())
	{
		_mem[addr] = data;
	}
}

/*
* @param size memory size in KBs
*/
Bus::Bus(uint32_t size)
{
	_mem.resize(size * 1024);
	memset(&_mem[0], 0, _mem.size() * sizeof (uint8_t));
}

Bus::~Bus()
{
	_mem.clear();
}
