#include "ppu_2C02.h"
#include "cpu_6502.h"
#include "bus.h"


const size_t PPU_2C02::screenWidth = 256;
const size_t PPU_2C02::screenHeight = 240;
const std::vector<PPU_2C02::Color> PPU_2C02::colorPalette =
{
	{ 84,  84,  84},
	{  0,  30, 116},
	{  8,  16, 144},
	{ 48,   0, 136},
	{ 68,   0, 100},
	{ 92,   0,  48},
	{ 84,   4,   0},
	{ 60,  24,   0},
	{ 32,  42,   0},
	{  8,  58,   0},
	{  0,  64,   0},
	{  0,  60,   0},
	{  0,  50,  60},
	{  0,   0,   0},
	{  0,   0,   0},
	{  0,   0,   0},

	{152, 150, 152},
	{  8,  76, 196},
	{ 48,  50, 236},
	{ 92,  30, 228},
	{136,  20, 176},
	{160,  20, 100},
	{152,  34,  32},
	{120,  60,   0},
	{ 84,  90,   0},
	{ 40, 114,   0},
	{  8, 124,   0},
	{  0, 118,  40},
	{  0, 102, 120},
	{  0,   0,   0},
	{  0,   0,   0},
	{  0,   0,   0},

	{236, 238, 236},
	{ 76, 154, 236},
	{120, 124, 236},
	{176,  98, 236},
	{228,  84, 236},
	{236,  88, 180},
	{236, 106, 100},
	{212, 136,  32},
	{160, 170,   0},
	{116, 196,   0},
	{ 76, 208,  32},
	{ 56, 204, 108},
	{ 56, 180, 204},
	{ 60,  60,  60},
	{  0,   0,   0},
	{  0,   0,   0},

	{236, 238, 236},
	{168, 204, 236},
	{188, 188, 236},
	{212, 178, 236},
	{236, 174, 236},
	{236, 174, 212},
	{236, 180, 176},
	{228, 196, 144},
	{204, 210, 120},
	{180, 222, 120},
	{168, 226, 144},
	{152, 226, 180},
	{160, 214, 228},
	{160, 162, 160},
	{  0,   0,   0},
	{  0,   0,   0}
};

PPU_2C02::PPU_2C02(Bus& bus)
	: Master{ bus }
{
	size_t size = screenWidth * screenHeight;
	screenBuffer.resize(size);
	Color color{ 60, 255, 60 };

	for (size_t y = 0; y < screenHeight; y++)
	{
		for (size_t x = 0; x < screenWidth; x++)
			setPixel(x, y, color);
	}
}

PPU_2C02::~PPU_2C02()
{
}

void PPU_2C02::reset()
{

}

void PPU_2C02::clock()
{
	for (size_t y = 0; y < screenHeight; y++)
	{
		for (size_t x = 0; x < screenWidth; x++)
		{
			if (rand() % 2)
				setPixel(x, y, Color{ 0x00, 0x00, 0x00 });
			else
				setPixel(x, y, Color{ 0xFF, 0xFF, 0xFF });
		}
	}


	/*
	cycle++;
	if (cycle >= 341)
	{
		cycle = 0;
		scanline++;
		if (scanline >= 261)
		{
			scanline = -1;
			frameDone = true;
		}
	}*/
}

uint8_t PPU_2C02::read(uint16_t addr, Master* master)
{
	if (dynamic_cast<CPU_6502*>(master) != nullptr)
	{
		switch (addr & 0x0007)
		{
		case 0x0000: // control
			break;
		case 0x0001: // mask
			break;
		case 0x0002: // status
			break;
		case 0x0003: // OAM address
			break;
		case 0x0004: // OAM data
			break;
		case 0x0005: // scroll
			break;
		case 0x0006: // PPU address
			break;
		case 0x0007: // PPU data
			break;
		}
	}

	return 0x00;
}

void PPU_2C02::write(uint16_t addr, uint8_t data, Master* master)
{
	if (dynamic_cast<CPU_6502*>(master) != nullptr)
	{
		switch (addr & 0x0007)
		{
		case 0x0000: // control
			break;
		case 0x0001: // mask
			break;
		case 0x0002: // status
			break;
		case 0x0003: // OAM address
			break;
		case 0x0004: // OAM data
			break;
		case 0x0005: // scroll
			break;
		case 0x0006: // PPU address
			break;
		case 0x0007: // PPU data
			break;
		}
	}
}

uint8_t PPU_2C02::read(uint16_t addr)
{
	return uint8_t();
}

void PPU_2C02::write(uint16_t addr, uint8_t data)
{

}

void PPU_2C02::setPixel(size_t x, size_t y, Color color)
{
	y = screenHeight - y - 1;

	size_t i = y * screenWidth + x;

	screenBuffer.at(i) = color;
}
