#pragma once
#include <vector>


class Cart_MEM
{
public:
	Cart_MEM();
	~Cart_MEM();

	void reset();

	uint8_t progRomBanks;
	uint8_t progRamBanks;
	uint8_t charRomBanks;
	uint8_t charRamBanks;

	std::vector<uint8_t> progROM;
	std::vector<uint8_t> progRAM;
	std::vector<uint8_t> charROM;
	std::vector<uint8_t> charRAM;
	std::vector<uint8_t> trainer;
};
