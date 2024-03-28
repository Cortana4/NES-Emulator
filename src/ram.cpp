#include "ram.h"
#include "cpu_6502.h"


RAM::RAM(size_t size)
{
	ram.resize(size);
}

RAM::~RAM()
{

}

uint8_t RAM::read(uint16_t addr, Master* master)
{
	if (dynamic_cast<CPU_6502*>(master) != nullptr)
		return ram.at(addr & 0x07FF);

	return 0x00;
}

void RAM::write(uint16_t addr, uint8_t data, Master* master)
{
	if (dynamic_cast<CPU_6502*>(master) != nullptr)
		ram.at(addr & 0x07FF) = data;
}
