#pragma once
#include <vector>

#include "slave.h"


class RAM : public Slave
{
public:
	RAM(size_t size = 2048);
	~RAM();

	virtual uint8_t read(uint16_t addr, Master* master) override;
	virtual void write(uint16_t addr, uint8_t data, Master* master) override;

private:
	std::vector<uint8_t> ram;
};
