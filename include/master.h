#pragma once
#include <cstdint>


class Bus;

class Master
{
public:
	Master(Bus& bus);
	~Master();

protected:
	virtual uint8_t read(uint16_t addr) = 0;
	virtual void write(uint16_t addr, uint8_t data) = 0;

	Bus& bus;
};
