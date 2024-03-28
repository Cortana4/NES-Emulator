#pragma once
#include "master.h"


class CPU_6502 : public Master
{
public:
	CPU_6502(Bus& bus);
	~CPU_6502();

	void reset();
	void clock();
	void IRQ();
	void NMI();

protected:
	virtual uint8_t read(uint16_t addr) override;
	virtual void write(uint16_t addr, uint8_t data) override;
	
private:
	// registers
	uint8_t ACC;
	uint8_t X;
	uint8_t Y;
	uint8_t SP;
	uint8_t SR;
	uint16_t PC;

	// addressing modes
	void IMP();	void IMM();
	void ZP0();	void ZPX();
	void ZPY();	void REL();
	void ABS();	void ABX();
	void ABY();	void IND();
	void IZX();	void IZY();

	// instructions
	void ADC();	void AND();	void ASL();	void BCC();
	void BCS();	void BEQ();	void BIT();	void BMI();
	void BNE();	void BPL();	void BRK();	void BVC();
	void BVS();	void CLC();	void CLD();	void CLI();
	void CLV();	void CMP();	void CPX();	void CPY();
	void DEC();	void DEX();	void DEY();	void EOR();
	void INC();	void INX();	void INY();	void JMP();
	void JSR();	void LDA();	void LDX();	void LDY();
	void LSR();	void NOP();	void ORA();	void PHA();
	void PHP();	void PLA();	void PLP();	void ROL();
	void ROR();	void RTI();	void RTS();	void SBC();
	void SEC();	void SED();	void SEI();	void STA();
	void STX();	void STY();	void TAX();	void TAY();
	void TSX();	void TXA();	void TXS();	void TYA();

	// helper functions and variables
	enum Flag
	{
		C = (1 << 0),	// carry
		Z = (1 << 1),	// zero
		I = (1 << 2),	// interrupt disable
		D = (1 << 3),	// decimal mode
		B = (1 << 4),	// break
		V = (1 << 6),	// Overflow
		N = (1 << 7),	// Negative
	};

	uint8_t opcode;
	uint8_t cycles;
	uint8_t operand;
	uint16_t addr;
	uint16_t temp;

	void executeInst(uint8_t opcode);

	bool getFlag(Flag flag);
	void setFlag(Flag flag, bool value);
};
