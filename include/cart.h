#pragma once
#include <filesystem>

#include "slave.h"
#include "cart_mem.h"
#include "mapper.h"


class Cart : public Slave
{
public:
	Cart();
	~Cart();

	bool load(const std::filesystem::path& path);
	void reset();

	virtual uint8_t read(uint16_t addr, Master* master) override;
	virtual void write(uint16_t addr, uint8_t data, Master* master) override;

private:
	Cart_MEM mem;
	Mapper* mapper;
	uint8_t mapperID;

	bool usesVerticalMirroring;
	bool usesHorizontalMirroring;
	bool usesFourScreenMode;

	bool hasNonVolatileRAM;
	bool hasProgRAM;
	bool hasTrainer;
	bool hasBusConflicts;
	
	bool isVsUnisystem;
	bool isPlayChoice10;
	bool isNES2;
	bool isNTSC;
	bool isPAL;
	bool isDualCompatible;
};
