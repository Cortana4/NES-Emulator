#pragma once
#include <vector>

#include "master.h"
#include "slave.h"


class PPU_2C02 : public Master, public Slave
{
public:
	PPU_2C02(Bus& bus);
	~PPU_2C02();

	void reset();
	void clock();

	virtual uint8_t read(uint16_t addr, Master* master) override;
	virtual void write(uint16_t addr, uint8_t data, Master* master) override;

	struct Color
	{
		uint8_t R;
		uint8_t G;
		uint8_t B;
	};

	static const size_t screenWidth;
	static const size_t screenHeight;
	std::vector<Color> screenBuffer;

protected:
	virtual uint8_t read(uint16_t addr) override;
	virtual void write(uint16_t addr, uint8_t data) override;

private:
	// todo: use heap
	//uint8_t nameTable[2][1024];
	//uint8_t patternTable[2][4096];	// (charROM/charRAM)

	uint16_t scanline;
	uint16_t cycle;

	bool frameDone;

	static const std::vector<Color> colorPalette;

	//static const Color colorPalette[0x40];

	void setPixel(size_t x, size_t y, Color color);
};
