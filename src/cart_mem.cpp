#include "cart_mem.h"


Cart_MEM::Cart_MEM()
	: progRomBanks{ 0x00 }
	, progRamBanks{ 0x00 }
	, charRomBanks{ 0x00 }
	, charRamBanks{ 0x00 }
{
	
}

Cart_MEM::~Cart_MEM()
{

}

void Cart_MEM::reset()
{
	progRomBanks = 0x00;
	progRamBanks = 0x00;
	charRomBanks = 0x00;
	charRamBanks = 0x00;

	progROM.clear();
	progRAM.clear();
	charROM.clear();
	charRAM.clear();
	trainer.clear();
}
