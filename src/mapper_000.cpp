#include "mapper_000.h"
#include "cpu_6502.h"
#include "ppu_2C02.h"


Mapper_000::Mapper_000(Cart_MEM& mem)
	: Mapper{ mem }
{

}

Mapper_000::~Mapper_000()
{

}

uint8_t Mapper_000::read(uint16_t addr, Master* master)
{
	if (dynamic_cast<CPU_6502*>(master) != nullptr)
	{
		// prog RAM
		if (addr >= 0x6000 && addr <= 0x7FFF)
		{
			if (mem.progRamBanks != 0)
				return mem.progRAM.at(addr & 0x1FFF);
		}
		// prog ROM
		else if (addr >= 0x8000 && addr <= 0xFFFF)
		{
			if (mem.progRomBanks > 1)
				return mem.progROM.at(addr & 0x7FFF);

			return mem.progROM.at(addr & 0x3FFF);
		}
	}
	else if (dynamic_cast<PPU_2C02*>(master) != nullptr)
	{
		if (addr >= 0x0000 && addr <= 0x1FFF)
		{
			if (mem.charRomBanks == 0)
				return mem.charRAM.at(addr);

			return mem.charROM.at(addr);
		}
	}

	return 0x00;
}

void Mapper_000::write(uint16_t addr, uint8_t data, Master* master)
{
	if (dynamic_cast<CPU_6502*>(master) != nullptr)
	{
		// prog RAM
		if (addr >= 0x6000 && addr <= 0x7FFF)
		{
			if (mem.progRamBanks != 0)
				mem.progRAM.at(addr & 0x1FFF) = data;
		}
	}
	else if (dynamic_cast<PPU_2C02*>(master) != nullptr)
	{
		if (addr >= 0x0000 && addr <= 0x1FFF)
		{
			if (mem.charRomBanks == 0)
				mem.charRAM.at(addr) = data;
		}
	}
}
