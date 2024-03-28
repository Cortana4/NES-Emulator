#pragma once
#include "mapper.h"


class Mapper_000 : public Mapper
{
public:
	Mapper_000(Cart_MEM& mem);
	~Mapper_000();

	virtual uint8_t read(uint16_t addr, Master* master) override;
	virtual void write(uint16_t addr, uint8_t data, Master* master) override;
};
