#include "bus.h"


Bus::Bus()
	: cpu{ *this }
	, ppu{ *this }
	, ram{}
	, cart{}
{

}

Bus::~Bus()
{

}
