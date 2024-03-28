#pragma once
#include "cpu_6502.h"
#include "ppu_2C02.h"
#include "ram.h"
#include "cart.h"


class Bus
{
public:
	Bus();
	~Bus();

	CPU_6502 cpu;
	PPU_2C02 ppu;
	RAM ram;
	Cart cart;
	
};
