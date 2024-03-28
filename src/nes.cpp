#include "nes.h"


NES::NES()
{

}

NES::~NES()
{

}

void NES::clock()
{
	ppu.clock();
}

size_t NES::getScreenWidth()
{
	return ppu.screenWidth;
}

size_t NES::getScreenHeight()
{
	return ppu.screenHeight;
}

uint8_t* NES::getScreenBuffer()
{
	return reinterpret_cast<uint8_t*>(ppu.screenBuffer.data());
}
