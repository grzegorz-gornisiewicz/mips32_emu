#pragma once
#include <cstdint>

class IBus
{
public:
	virtual uint32_t Size() = 0;
	virtual uint32_t Read(uint32_t addr) = 0;
	virtual void Write(uint32_t addr, uint8_t data) = 0;
};
