#pragma once
#include "master.h"


class Slave
{
public:
	virtual uint8_t read(uint16_t addr, Master* master) = 0;
	virtual void write(uint16_t addr, uint8_t data, Master* master) = 0;
};
