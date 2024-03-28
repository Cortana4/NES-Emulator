#include "cpu_6502.h"
#include "bus.h"


CPU_6502::CPU_6502(Bus& bus)
	: Master{ bus }
	, ACC{ 0x00 }, X{ 0x00 }, Y{ 0x00 }
	, SP{ 0x00 }, SR{ 0x00 }, PC{ 0x0000 }
	, opcode{ 0x00 }
	, cycles{ 0x00 }
	, operand{ 0x00 }
	, addr{ 0x0000 }
	, temp{ 0x0000 }
{

}

CPU_6502::~CPU_6502()
{

}

void CPU_6502::reset()
{
	uint16_t PC_low = read(0xFFFC);
	uint16_t PC_high = read(0xFFFD);

	ACC = 0x00;
	X = 0x00;
	Y = 0x00;
	SP = 0xFD;
	SR = 0x20;
	PC = (PC_high << 8) | PC_low;

	operand = 0x00;
	addr = 0x0000;
	temp = 0x0000;

	cycles = 8;
}

void CPU_6502::clock()
{
	if (cycles == 0)
	{
		opcode = read(PC);
		PC++;
		executeInst(opcode);
	}
	cycles--;
}

void CPU_6502::IRQ()
{
	if (!getFlag(I))
	{
		// push PC high
		write(0x0100 + SP, (PC >> 8) & 0x00FF);
		SP--;
		// push PC low
		write(0x0100 + SP, PC & 0x00FF);
		SP--;
		// push SR
		setFlag(B, false);
		setFlag(I, true);
		write(0x0100 + SP, SR);
		SP--;
		// read PC
		uint16_t PC_low = read(0xFFFE);
		uint16_t PC_high = read(0xFFFF);
		PC = (PC_high << 8) | PC_low;

		cycles = 7;
	}
}

void CPU_6502::NMI()
{
	// push PC high
	write(0x0100 + SP, (PC >> 8) & 0x00FF);
	SP--;
	// push PC low
	write(0x0100 + SP, PC & 0x00FF);
	SP--;
	// push SR
	setFlag(B, false);
	setFlag(I, true);
	write(0x0100 + SP, SR);
	SP--;
	// read PC
	uint16_t PC_low = read(0xFFFA);
	uint16_t PC_high = read(0xFFFB);
	PC = (PC_high << 8) | PC_low;

	cycles = 8;
}

// addressing modes
void CPU_6502::IMP()	// implied addressing
{
	addr = 0x0000;
}

void CPU_6502::IMM()	// operand address
{
	addr = PC;
	PC++;
}

void CPU_6502::ZP0()	// zero page addressing
{
	addr = static_cast<uint16_t>(read(PC));
	PC++;
}

void CPU_6502::ZPX()	// indexed zero page addressing (X)
{
	addr = static_cast<uint16_t>(read(PC) + X) & 0x00FF;
	PC++;
}

void CPU_6502::ZPY()	// indexed zero page addressing (Y)
{
	addr = static_cast<uint16_t>(read(PC) + Y) & 0x00FF;
	PC++;
}

void CPU_6502::REL()	// relative addressing
{
	uint16_t rel_addr = static_cast<uint16_t>(read(PC));
	PC++;
	// sign extend if negative
	if (rel_addr & 0x0080)
		rel_addr |= 0xFF00;

	addr = PC + rel_addr;
}

void CPU_6502::ABS()	// absolute addressing
{
	uint16_t addrL = static_cast<uint16_t>(read(PC));
	PC++;
	uint16_t addrH = static_cast<uint16_t>(read(PC));
	PC++;

	addr = (addrH << 8) | addrL;
}

void CPU_6502::ABX()	// indexed absolute addressing (X)
{
	uint16_t addrL = static_cast<uint16_t>(read(PC));
	PC++;
	uint16_t addrH = static_cast<uint16_t>(read(PC));
	PC++;

	addr = (addrH << 8) | addrL;
	addr += X;

	if ((addr & 0xFF00) != (addrH << 8))
		cycles++;
}

void CPU_6502::ABY()	// indexed absolute addressing (Y)
{
	uint16_t addrL = static_cast<uint16_t>(read(PC));
	PC++;
	uint16_t addrH = static_cast<uint16_t>(read(PC));
	PC++;

	addr = (addrH << 8) | addrL;
	addr += Y;

	if ((addr & 0xFF00) != (addrH << 8))
		cycles++;
}

void CPU_6502::IND()	// absolute indirect addressing
{
	uint16_t ptrL = static_cast<uint16_t>(read(PC));
	PC++;
	uint16_t ptrH = static_cast<uint16_t>(read(PC));
	PC++;

	uint16_t ptr = (ptrH << 8) | ptrL;

	uint16_t addrL = static_cast<uint16_t>(read(ptr));
	uint16_t addrH;

	// page boundary cross hardware bug
	if (ptrL == 0x00FF)
		addrH = static_cast<uint16_t>(read(ptr & 0xFF00));
	// normal behaviour
	else
		addrH = static_cast<uint16_t>(read(ptr + 1));

	addr = (addrH << 8) | addrL;
}

void CPU_6502::IZX()	// indexed indirect addressing (X)
{
	uint8_t offset = read(PC);
	PC++;

	uint16_t addrL = static_cast<uint16_t>(read(static_cast<uint16_t>(X + offset) & 0x00FF));
	uint16_t addrH = static_cast<uint16_t>(read(static_cast<uint16_t>(X + offset + 1) & 0x00FF));

	addr = (addrH << 8) | addrL;
}

void CPU_6502::IZY()	// indirect indexed addressing (Y)
{
	uint16_t ptr = static_cast<uint16_t>(read(PC));
	PC++;

	uint16_t addrL = static_cast<uint16_t>(ptr & 0x00FF);
	uint16_t addrH = static_cast<uint16_t>((ptr + 1) & 0x00FF);

	addr = (addrH << 8) | addrL;
	addr += Y;

	if ((addr & 0xFF00) != (addrH << 8))
		cycles++;
}

// instructions
void CPU_6502::ADC()
{
	operand = read(addr);
	temp = static_cast<uint16_t>(ACC) + static_cast<uint16_t>(operand) + static_cast<uint16_t>(getFlag(C));
	setFlag(C, temp > 0x00FF);
	setFlag(Z, (temp & 0x00FF) == 0);
	setFlag(V, ~(static_cast<uint16_t>(ACC) ^ static_cast<uint16_t>(operand)) & (static_cast<uint16_t>(ACC) ^ static_cast<uint16_t>(temp)) & 0x0080);
	setFlag(N, temp & 0x80);
	ACC = static_cast<uint8_t>(temp & 0x00FF);
	cycles++;
}

void CPU_6502::AND()
{
	operand = read(addr);
	ACC = ACC & operand;
	setFlag(Z, ACC == 0x00);
	setFlag(N, ACC & 0x80);
	cycles++;
}

void CPU_6502::ASL()
{
	operand = read(addr);
	temp = static_cast<uint16_t>(operand) << 1;
	setFlag(C, (temp & 0xFF00) > 0);
	setFlag(Z, (temp & 0x00FF) == 0x00);
	setFlag(N, temp & 0x0080);
	if (opcode == 0x0A)
		ACC = temp & 0x00FF;
	else
		write(addr, temp & 0x00FF);
}

void CPU_6502::BCC()
{
	if (!getFlag(C))
	{
		if ((addr & 0xFF00) != (PC & 0xFF00))
			cycles++;
		cycles++;
		PC = addr;
	}
}

void CPU_6502::BCS()
{
	if (getFlag(C))
	{
		if ((addr & 0xFF00) != (PC & 0xFF00))
			cycles++;
		cycles++;
		PC = addr;
	}
}

void CPU_6502::BEQ()
{
	if (getFlag(Z))
	{
		if ((addr & 0xFF00) != (PC & 0xFF00))
			cycles++;
		cycles++;
		PC = addr;
	}
}

void CPU_6502::BIT()
{
	operand = read(addr);
	temp = ACC & operand;
	setFlag(Z, (temp & 0x00FF) == 0x00);
	setFlag(N, operand & (1 << 7));
	setFlag(V, operand & (1 << 6));
}

void CPU_6502::BMI()
{
	if (getFlag(N))
	{
		if ((addr & 0xFF00) != (PC & 0xFF00))
			cycles++;
		cycles++;
		PC = addr;
	}
}

void CPU_6502::BNE()
{
	if (!getFlag(Z))
	{
		if ((addr & 0xFF00) != (PC & 0xFF00))
			cycles++;
		cycles++;
		PC = addr;
	}
}

void CPU_6502::BPL()
{
	if (!getFlag(N))
	{
		if ((addr & 0xFF00) != (PC & 0xFF00))
			cycles++;
		cycles++;
		PC = addr;
	}
}

void CPU_6502::BRK()
{
	PC++;
	setFlag(I, true);
	write(0x0100 + static_cast<uint16_t>(SP), static_cast<uint8_t>((PC >> 8) & 0x00FF));
	SP--;
	write(0x0100 + static_cast<uint16_t>(SP), static_cast<uint8_t>(PC & 0x00FF));
	SP--;
	setFlag(B, true);
	write(0x0100 + static_cast<uint16_t>(SP), SR);
	SP--;
	setFlag(B, false);
	PC = static_cast<uint16_t>(read(0xFFFF) << 8) | static_cast<uint16_t>(read(0xFFFE));
}

void CPU_6502::BVC()
{
	if (!getFlag(V))
	{
		if ((addr & 0xFF00) != (PC & 0xFF00))
			cycles++;
		cycles++;
		PC = addr;
	}
}

void CPU_6502::BVS()
{
	if (getFlag(V))
	{
		if ((addr & 0xFF00) != (PC & 0xFF00))
			cycles++;
		cycles++;
		PC = addr;
	}
}

void CPU_6502::CLC()
{
	setFlag(C, false);
}

void CPU_6502::CLD()
{
	setFlag(D, false);
}

void CPU_6502::CLI()
{
	setFlag(I, false);
}

void CPU_6502::CLV()
{
	setFlag(V, false);
}

void CPU_6502::CMP()
{
	operand = read(addr);
	temp = static_cast<uint16_t>(ACC) - static_cast<uint16_t>(operand);
	setFlag(C, ACC >= operand);
	setFlag(Z, (temp & 0x00FF) == 0x0000);
	setFlag(N, temp & 0x0080);
	cycles++;
}

void CPU_6502::CPX()
{
	operand = read(addr);
	temp = static_cast<uint16_t>(X) - static_cast<uint16_t>(operand);
	setFlag(C, X >= operand);
	setFlag(Z, (temp & 0x00FF) == 0x0000);
	setFlag(N, temp & 0x0080);
}

void CPU_6502::CPY()
{
	operand = read(addr);
	temp = static_cast<uint16_t>(Y) - static_cast<uint16_t>(operand);
	setFlag(C, Y >= operand);
	setFlag(Z, (temp & 0x00FF) == 0x0000);
	setFlag(N, temp & 0x0080);
}

void CPU_6502::DEC()
{
	operand = read(addr);
	temp = static_cast<uint16_t>(operand) - 1;
	write(addr, static_cast<uint8_t>(temp & 0x00FF));
	setFlag(Z, (temp & 0x00FF) == 0x0000);
	setFlag(N, temp & 0x0080);
}

void CPU_6502::DEX()
{
	X--;
	setFlag(Z, X == 0x00);
	setFlag(N, X & 0x80);
}

void CPU_6502::DEY()
{
	Y--;
	setFlag(Z, Y == 0x00);
	setFlag(N, Y & 0x80);
}

void CPU_6502::EOR()
{
	operand = read(addr);
	ACC = ACC ^ operand;
	setFlag(Z, ACC == 0x00);
	setFlag(N, ACC & 0x80);
	cycles++;
}

void CPU_6502::INC()
{
	operand = read(addr);
	temp = static_cast<uint16_t>(operand) + 1;
	write(addr, temp & 0x00FF);
	setFlag(Z, (temp & 0x00FF) == 0x0000);
	setFlag(N, temp & 0x0080);
}

void CPU_6502::INX()
{
	X++;
	setFlag(Z, X == 0x00);
	setFlag(N, X & 0x80);
}

void CPU_6502::INY()
{
	Y++;
	setFlag(Z, Y == 0x00);
	setFlag(N, Y & 0x80);
}

void CPU_6502::JMP()
{
	PC = addr;
}

void CPU_6502::JSR()
{
	PC--;
	write(0x0100 + static_cast<uint16_t>(SP), static_cast<uint8_t>((PC >> 8) & 0x00FF));
	SP--;
	write(0x0100 + static_cast<uint16_t>(SP), static_cast<uint8_t>(PC & 0x00FF));
	SP--;
	PC = addr;
}

void CPU_6502::LDA()
{
	ACC = read(addr);
	setFlag(Z, ACC == 0x00);
	setFlag(N, ACC & 0x80);
	cycles++;
}

void CPU_6502::LDX()
{
	X = read(addr);
	setFlag(Z, X == 0x00);
	setFlag(N, X & 0x80);
	cycles++;
}

void CPU_6502::LDY()
{
	Y = read(addr);
	setFlag(Z, Y == 0x00);
	setFlag(N, Y & 0x80);
	cycles++;
}

void CPU_6502::LSR()
{
	operand = read(addr);
	setFlag(C, operand & 0x0001);
	temp = operand >> 1;
	setFlag(Z, (temp & 0x00FF) == 0x0000);
	setFlag(N, temp & 0x0080);
	if (opcode = 0x4A)
		ACC = static_cast<uint8_t>(temp & 0x00FF);
	else
		write(addr, static_cast<uint8_t>(temp & 0x00FF));
}

void CPU_6502::NOP()
{

}

void CPU_6502::ORA()
{
	operand = read(addr);
	ACC = ACC | operand;
	setFlag(Z, ACC == 0x00);
	setFlag(N, ACC & 0x80);
	cycles++;
}

void CPU_6502::PHA()
{
	write(0x0100 + static_cast<uint16_t>(SP), ACC);
	SP--;
}

void CPU_6502::PHP()
{
	write(0x0100 + static_cast<uint16_t>(SP), SR | B);
	setFlag(B, false);
	SP--;
}

void CPU_6502::PLA()
{
	SP++;
	ACC = read(0x0100 + static_cast<uint16_t>(SP));
	setFlag(Z, ACC == 0x00);
	setFlag(N, ACC & 0x80);
}

void CPU_6502::PLP()
{
	SP++;
	SR = read(0x0100 + static_cast<uint16_t>(SP));
}

void CPU_6502::ROL()
{
	operand = read(addr);
	temp = (static_cast<uint16_t>(operand) << 1) | static_cast<uint16_t>(getFlag(C));
	setFlag(C, temp & 0xFF00);
	setFlag(Z, (temp & 0x00FF) == 0x0000);
	setFlag(N, temp & 0x0080);
	if (opcode == 0x2A)
		ACC = static_cast<uint8_t>(temp & 0x00FF);
	else
		write(addr, static_cast<uint8_t>(temp & 0x00FF));
}

void CPU_6502::ROR()
{
	operand = read(addr);
	temp = (static_cast<uint16_t>(getFlag(C)) << 7) | (static_cast<uint16_t>(operand) >> 1);
	setFlag(C, operand & 0x01);
	setFlag(Z, (temp & 0x00FF) == 0x00);
	setFlag(N, temp & 0x0080);
	if (opcode == 0x6A)
		ACC = static_cast<uint8_t>(temp & 0x00FF);
	else
		write(addr, static_cast<uint8_t>(temp & 0x00FF));
}

void CPU_6502::RTI()
{
	SP++;
	SR = read(0x0100 + static_cast<uint16_t>(SP));
	SR &= ~B;
	SP++;
	PC = static_cast<uint16_t>(read(0x0100 + static_cast<uint16_t>(SP)));
	SP++;
	PC |= static_cast<uint16_t>(read(0x0100 + static_cast<uint16_t>(SP))) << 8;
}

void CPU_6502::RTS()
{
	SP++;
	PC = static_cast<uint16_t>(read(0x0100 + static_cast<uint16_t>(SP)));
	SP++;
	PC |= static_cast<uint16_t>(read(0x0100 + static_cast<uint16_t>(SP))) << 8;
	PC++;
}

void CPU_6502::SBC()
{
	operand = read(addr);
	uint16_t neg_operand = static_cast<uint16_t>(operand) ^ 0x00FF;
	temp = static_cast<uint16_t>(ACC) + neg_operand + static_cast<uint16_t>(getFlag(C));
	setFlag(C, temp > 0x00FF);
	setFlag(Z, (temp & 0x00FF) == 0);
	setFlag(V, (temp ^ static_cast<uint16_t>(ACC)) & (temp ^ neg_operand) & 0x0080);
	setFlag(N, temp & 0x80);
	ACC = static_cast<uint8_t>(temp & 0x00FF);
	cycles++;
}

void CPU_6502::SEC()
{
	setFlag(C, true);
}

void CPU_6502::SED()
{
	setFlag(D, true);
}

void CPU_6502::SEI()
{
	setFlag(I, true);
}

void CPU_6502::STA()
{
	write(addr, ACC);
}

void CPU_6502::STX()
{
	write(addr, X);
}

void CPU_6502::STY()
{
	write(addr, Y);
}

void CPU_6502::TAX()
{
	X = ACC;
	setFlag(Z, X == 0x00);
	setFlag(N, X & 0x80);
}

void CPU_6502::TAY()
{
	Y = ACC;
	setFlag(Z, Y == 0x00);
	setFlag(N, Y & 0x80);
}


void CPU_6502::TSX()
{
	X = SP;
	setFlag(Z, X == 0x00);
	setFlag(N, X & 0x80);
}

void CPU_6502::TXA()
{
	ACC = X;
	setFlag(Z, ACC == 0x00);
	setFlag(N, ACC & 0x80);
}

void CPU_6502::TXS()
{
	SP = X;
}

void CPU_6502::TYA()
{
	ACC = Y;
	setFlag(Z, ACC == 0x00);
	setFlag(N, ACC & 0x80);
}

// helpers
void CPU_6502::executeInst(uint8_t opcode)
{
	switch (opcode)
	{
	case 0x00:	cycles = 7; IMM(); BRK(); break;
	case 0x01:	cycles = 6; IZX(); ORA(); break;
	case 0x05:	cycles = 3; ZP0(); ORA(); break;
	case 0x06:	cycles = 5; ZP0(); ASL(); break;
	case 0x08:	cycles = 3; IMP(); PHP(); break;
	case 0x09:	cycles = 2; IMM(); ORA(); break;
	case 0x0A:	cycles = 2; IMP(); ASL(); break;
	case 0x0D:	cycles = 4; ABS(); ORA(); break;
	case 0x0E:	cycles = 6; ABS(); ASL(); break;

	case 0x10:	cycles = 2; REL(); BPL(); break;
	case 0x11:	cycles = 5; IZY(); ORA(); break;
	case 0x15:	cycles = 4; ZPX(); ORA(); break;
	case 0x16:	cycles = 6; ZPX(); ASL(); break;
	case 0x18:	cycles = 2; IMP(); CLC(); break;
	case 0x19:	cycles = 4; ABY(); ORA(); break;
	case 0x1D:	cycles = 4; ABX(); ORA(); break;
	case 0x1E:	cycles = 7; ABX(); ASL(); break;

	case 0x20:	cycles = 6; ABS(); JSR(); break;
	case 0x21:	cycles = 6; IZX(); AND(); break;
	case 0x24:	cycles = 3; ZP0(); BIT(); break;
	case 0x25:	cycles = 3; ZP0(); AND(); break;
	case 0x26:	cycles = 5; ZP0(); ROL(); break;
	case 0x28:	cycles = 4; IMP(); PLP(); break;
	case 0x29:	cycles = 2; IMM(); AND(); break;
	case 0x2A:	cycles = 2; IMP(); ROL(); break;
	case 0x2C:	cycles = 4; ABS(); BIT(); break;
	case 0x2D:	cycles = 4; ABS(); AND(); break;
	case 0x2E:	cycles = 6; ABS(); ROL(); break;

	case 0x30:	cycles = 2; REL(); BMI(); break;
	case 0x31:	cycles = 5; IZY(); AND(); break;
	case 0x35:	cycles = 4; ZPX(); AND(); break;
	case 0x36:	cycles = 6; ZPX(); ROL(); break;
	case 0x38:	cycles = 2; IMP(); SEC(); break;
	case 0x39:	cycles = 4; ABY(); AND(); break;
	case 0x3D:	cycles = 4; ABX(); AND(); break;
	case 0x3E:	cycles = 7; ABX(); ROL(); break;

	case 0x40:	cycles = 6; IMP(); RTI(); break;
	case 0x41:	cycles = 3; IZX(); EOR(); break;
	case 0x45:	cycles = 3; ZP0(); EOR(); break;
	case 0x46:	cycles = 5; ZP0(); LSR(); break;
	case 0x48:	cycles = 3; IMP(); PHA(); break;
	case 0x49:	cycles = 2; IMM(); EOR(); break;
	case 0x4A:	cycles = 2; IMP(); LSR(); break;
	case 0x4C:	cycles = 3; ABS(); JMP(); break;
	case 0x4D:	cycles = 4; ABS(); EOR(); break;
	case 0x4E:	cycles = 6; ABS(); LSR(); break;

	case 0x50:	cycles = 2; REL(); BVC(); break;
	case 0x51:	cycles = 5; IZY(); EOR(); break;
	case 0x55:	cycles = 4; ZPX(); EOR(); break;
	case 0x56:	cycles = 6; ZPX(); LSR(); break;
	case 0x58:	cycles = 2; IMP(); CLI(); break;
	case 0x59:	cycles = 4; ABY(); EOR(); break;
	case 0x5D:	cycles = 4; ABX(); EOR(); break;
	case 0x5E:	cycles = 7; ABX(); LSR(); break;

	case 0x60:	cycles = 6; IMP(); RTS(); break;
	case 0x61:	cycles = 6; IZX(); ADC(); break;
	case 0x65:	cycles = 3; ZP0(); ADC(); break;
	case 0x66:	cycles = 5; ZP0(); ROR(); break;
	case 0x68:	cycles = 4; IMP(); PLA(); break;
	case 0x69:	cycles = 2; IMM(); ADC(); break;
	case 0x6A:	cycles = 2; IMP(); ROR(); break;
	case 0x6C:	cycles = 5; IND(); JMP(); break;
	case 0x6D:	cycles = 4; ABS(); ADC(); break;
	case 0x6E:	cycles = 6; ABS(); ROR(); break;

	case 0x70:	cycles = 2; REL(); BVS(); break;
	case 0x71:	cycles = 5; IZY(); ADC(); break;
	case 0x75:	cycles = 4; ZPX(); ADC(); break;
	case 0x76:	cycles = 6; ZPX(); ROR(); break;
	case 0x78:	cycles = 2; IMP(); SEI(); break;
	case 0x79:	cycles = 4; ABY(); ADC(); break;
	case 0x7D:	cycles = 4; ABX(); ADC(); break;
	case 0x7E:	cycles = 7; ABX(); ROR(); break;

	case 0x81:	cycles = 6; IZX(); STA(); break;
	case 0x84:	cycles = 3; ZP0(); STY(); break;
	case 0x85:	cycles = 3; ZP0(); STA(); break;
	case 0x86:	cycles = 3; ZP0(); STX(); break;
	case 0x88:	cycles = 2; IMP(); DEY(); break;
	case 0x8A:	cycles = 2; IMP(); TXA(); break;
	case 0x8C:	cycles = 4; ABS(); STY(); break;
	case 0x8D:	cycles = 4; ABS(); STA(); break;
	case 0x8E:	cycles = 4; ABS(); STX(); break;

	case 0x90:	cycles = 2; REL(); BCC(); break;
	case 0x91:	cycles = 6; IZY(); STA(); break;
	case 0x94:	cycles = 4; ZPX(); STY(); break;
	case 0x95:	cycles = 4; ZPX(); STA(); break;
	case 0x96:	cycles = 4; ZPY(); STX(); break;
	case 0x98:	cycles = 2; IMP(); TYA(); break;
	case 0x99:	cycles = 5; ABY(); STA(); break;
	case 0x9A:	cycles = 2; IMP(); TXS(); break;
	case 0x9D:	cycles = 5; ABX(); STA(); break;

	case 0xA0:	cycles = 2; IMM(); LDY(); break;
	case 0xA1:	cycles = 6; IZX(); LDA(); break;
	case 0xA2:	cycles = 2; IMM(); LDX(); break;
	case 0xA4:	cycles = 3; ZP0(); LDY(); break;
	case 0xA5:	cycles = 3; ZP0(); LDA(); break;
	case 0xA6:	cycles = 3; ZP0(); LDX(); break;
	case 0xA8:	cycles = 2; IMP(); TAY(); break;
	case 0xA9:	cycles = 2; IMM(); LDA(); break;
	case 0xAA:	cycles = 2; IMP(); TAX(); break;
	case 0xAC:	cycles = 4; ABS(); LDY(); break;
	case 0xAD:	cycles = 4; ABS(); LDA(); break;
	case 0xAE:	cycles = 4; ABS(); LDX(); break;

	case 0xB0:	cycles = 2; REL(); BCS(); break;
	case 0xB1:	cycles = 5; IZY(); LDA(); break;
	case 0xB4:	cycles = 4; ZPX(); LDY(); break;
	case 0xB5:	cycles = 4; ZPX(); LDA(); break;
	case 0xB6:	cycles = 4; ZPY(); LDX(); break;
	case 0xB8:	cycles = 2; IMP(); CLV(); break;
	case 0xB9:	cycles = 4; ABY(); LDA(); break;
	case 0xBA:	cycles = 2; IMP(); TSX(); break;
	case 0xBC:	cycles = 4; ABX(); LDY(); break;
	case 0xBD:	cycles = 4; ABX(); LDA(); break;
	case 0xBE:	cycles = 4; ABY(); LDX(); break;

	case 0xC0:	cycles = 2; IMM(); CPY(); break;
	case 0xC1:	cycles = 6; IZX(); CMP(); break;
	case 0xC4:	cycles = 3; ZP0(); CPY(); break;
	case 0xC5:	cycles = 3; ZP0(); CMP(); break;
	case 0xC6:	cycles = 5; ZP0(); DEC(); break;
	case 0xC8:	cycles = 2; IMP(); INY(); break;
	case 0xC9:	cycles = 2; IMM(); DEX(); break;
	case 0xCC:	cycles = 4; ABS(); CPY(); break;
	case 0xCD:	cycles = 4; ABS(); CMP(); break;
	case 0xCE:	cycles = 6; ABS(); DEC(); break;

	case 0xD0:	cycles = 2; REL(); BNE(); break;
	case 0xD1:	cycles = 5; IZY(); CMP(); break;
	case 0xD5:	cycles = 4; ZPX(); CMP(); break;
	case 0xD6:	cycles = 6; ZPX(); DEC(); break;
	case 0xD8:	cycles = 2; IMP(); CLD(); break;
	case 0xD9:	cycles = 4; ABY(); CMP(); break;
	case 0xDD:	cycles = 4; ABX(); CMP(); break;
	case 0xDE:	cycles = 7; ABX(); DEC(); break;

	case 0xE0:	cycles = 2; IMM(); CPX(); break;
	case 0xE1:	cycles = 6; IZX(); SBC(); break;
	case 0xE4:	cycles = 3; ZP0(); CPX(); break;
	case 0xE5:	cycles = 3; ZP0(); SBC(); break;
	case 0xE6:	cycles = 5; ZP0(); INC(); break;
	case 0xE8:	cycles = 2; IMP(); INX(); break;
	case 0xE9:	cycles = 2; IMM(); SBC(); break;
	case 0xEA:	cycles = 2; IMP(); NOP(); break;
	case 0xEC:	cycles = 4; ABS(); CPX(); break;
	case 0xED:	cycles = 4; ABS(); SBC(); break;
	case 0xEE:	cycles = 6; ABS(); INC(); break;

	case 0xF0:	cycles = 2; REL(); BEQ(); break;
	case 0xF1:	cycles = 5; IZY(); SBC(); break;
	case 0xF5:	cycles = 4; ZPX(); SBC(); break;
	case 0xF6:	cycles = 6; ZPX(); INC(); break;
	case 0xF8:	cycles = 2; IMP(); SED(); break;
	case 0xF9:	cycles = 4; ABY(); SBC(); break;
	case 0xFD:	cycles = 4; ABX(); SBC(); break;
	case 0xFE:	cycles = 7; ABX(); INC(); break;
	default:	cycles = 2; IMP(); NOP();
	}
}

bool CPU_6502::getFlag(Flag flag)
{
	return SR & flag;
}

void CPU_6502::setFlag(Flag flag, bool value)
{
	if (value)
		SR |= flag;
	else
		SR &= ~flag;
}

uint8_t CPU_6502::read(uint16_t addr)
{
	// RAM
	if (addr >= 0x0000 && addr <= 0x1FFFF)
		return bus.ram.read(addr, this);
	// PPU registers
	else if (addr >= 0x2000 && addr <= 0x3FFF)
		return bus.ppu.read(addr, this);
	// APU registers
	else if (addr >= 0x4000 && addr <= 0x401F)
		return 0x00;
	// cartridge
	else if (addr >= 0x4020 && addr <= 0xFFFF)
		return bus.cart.read(addr, this);

	return 0x00;
}

void CPU_6502::write(uint16_t addr, uint8_t data)
{
	// RAM
	if (addr >= 0x0000 && addr <= 0x1FFFF)
		bus.ram.write(addr, data, this);
	// PPU registers
	else if (addr >= 0x2000 && addr <= 0x3FFF)
		bus.ppu.write(addr, data, this);
	// APU registers
	else if (addr >= 0x4000 && addr <= 0x401F)
		return;
	// cartridge
	else if (addr >= 0x4020 && addr <= 0xFFFF)
		bus.cart.write(addr, data, this);
}
