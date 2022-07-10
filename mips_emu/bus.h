#include <vector>

#include "ibus.h"

using namespace std;

class Bus : public IBus
{
private:
	vector<uint8_t> _mem;

public:
	Bus(uint32_t size);
	~Bus();

	uint32_t Size();
	uint32_t Read(uint32_t addr);
	void Write(uint32_t addr, uint8_t data);
};
