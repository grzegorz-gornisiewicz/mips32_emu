#pragma once

#include "ibus.h"

class Bus : public IBus
{
private:
	uint8_t* _memory = NULL;

public:
	Bus(uint32_t size);

	uint32_t Size();
	uint32_t Read(uint32_t addr);
	void Write(uint32_t addr, uint32_t data);
};
