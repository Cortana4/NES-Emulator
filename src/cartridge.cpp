#include "cart.h"
#include "mapper_000.h"

#include <fstream>
#include <array>


Cart::Cart()
	: mapper{ nullptr }
	, mapperID{ 0xFF }
	, usesVerticalMirroring{ false }
	, usesHorizontalMirroring{ false }
	, usesFourScreenMode{ false }
	, hasNonVolatileRAM{ false }
	, hasProgRAM{ false }
	, hasTrainer{ false }
	, hasBusConflicts{ false }
	, isVsUnisystem{ false }
	, isPlayChoice10{ false }
	, isNES2{ false }
	, isNTSC{ false }
	, isPAL{ false }
	, isDualCompatible{ false }
{
	
}

Cart::~Cart()
{
	delete mapper;
}

void Cart::reset()
{
	mem.reset();
	delete mapper;

	mapperID = 0xFF;
	usesVerticalMirroring = false;
	usesHorizontalMirroring = false;
	usesFourScreenMode = false;
	hasNonVolatileRAM = false;
	hasProgRAM = false;
	hasTrainer = false;
	hasBusConflicts = false;
	isVsUnisystem = false;
	isPlayChoice10 = false;
	isNES2 = false;
	isNTSC = false;
	isPAL = false;
	isDualCompatible = false;
}

bool Cart::load(const std::filesystem::path& path)
{
	// open ROM file
	std::ifstream ROM_file;
	ROM_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	ROM_file.open(path, std::ios::binary);

	// read file header
	std::array<uint8_t, 16> fileHeader;
	ROM_file.read(reinterpret_cast<char*>(fileHeader.data()), fileHeader.size());

	if (fileHeader[0] != 'N' || fileHeader[1] != 'E' || fileHeader[2] != 'S' || fileHeader[3] != 0x1A)
		throw std::invalid_argument::exception("Invalid file format!");

	mapperID = (fileHeader[6] & 0xF0) >> 4;
	mapperID |= fileHeader[7] & 0xF0;

	mem.progRomBanks = fileHeader[4];
	mem.progRamBanks = fileHeader[8];
	mem.charRomBanks = fileHeader[5];

	// set flags
	usesVerticalMirroring = fileHeader[6] & 0x01;
	usesHorizontalMirroring = !usesVerticalMirroring;
	usesFourScreenMode = fileHeader[6] & 0x08;

	hasNonVolatileRAM = fileHeader[6] & 0x02;
	hasTrainer = fileHeader[6] & 0x04;
	hasProgRAM = fileHeader[10] & 0x10;
	hasBusConflicts = fileHeader[10] & 0x20;

	isVsUnisystem = fileHeader[7] & 0x01;
	isPlayChoice10 = fileHeader[7] & 0x02;
	isNES2 = (fileHeader[7] & 0x0C) == 0x08;
	isPAL = fileHeader[9] & 0x01;
	isNTSC = !isPAL;
	isDualCompatible = ((fileHeader[10] & 0x03) == 0x01) || ((fileHeader[10] & 0x03) == 0x03);

	// read data
	if (hasTrainer)
	{
		mem.trainer.resize(512);
		ROM_file.read(reinterpret_cast<char*>(mem.trainer.data()), mem.trainer.size());
	}

	mem.progRamBanks += hasProgRAM && (mem.progRamBanks == 0);
	mem.progRAM.resize(mem.progRamBanks * 8192);
	mem.progROM.resize(mem.progRomBanks * 16384);
	ROM_file.read(reinterpret_cast<char*>(mem.progROM.data()), mem.progROM.size());

	// 0 char ROM banks indicate, the cartridge uses 1 char RAM bank instead
	if (mem.charRomBanks == 0)
	{
		mem.charRamBanks = 1;
		mem.charRAM.resize(8192);
	}
	else
	{
		mem.charRamBanks = 0;
		mem.charROM.resize(mem.charRomBanks * 8192);
		ROM_file.read(reinterpret_cast<char*>(mem.charROM.data()), mem.charROM.size());
	}

	ROM_file.close();

	switch (mapperID)
	{
	case 0x00:
		mapper = new Mapper_000{ mem };
		break;
	default:
		mapper = nullptr;
		throw std::invalid_argument::exception("Mapper not implemented!");
	}
}

uint8_t Cart::read(uint16_t addr, Master* master)
{
	return mapper->read(addr, master);
}

void Cart::write(uint16_t addr, uint8_t data, Master* master)
{
	mapper->write(addr, data, master);
}
