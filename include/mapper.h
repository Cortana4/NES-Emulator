#pragma once
#include <vector>

#include "slave.h"
#include "cart_mem.h"


class Mapper : public Slave
{
public:
	Mapper(Cart_MEM& mem);
	~Mapper();

protected:
	Cart_MEM& mem;
};
