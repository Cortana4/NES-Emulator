#pragma once
#include <vector>

#include "bus.h"


class NES : protected Bus
{
public:
	NES();
	~NES();

	void clock();

	size_t getScreenWidth();
	size_t getScreenHeight();
	uint8_t* getScreenBuffer();
};

