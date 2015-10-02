
#include "mcp23008.h"

mcp23008::mcp23008() : m_address(0)
{
}
    
int mcp23008::begin(uint8_t address, uint8_t sda, uint8_t scl)
{
	Wire.begin(sda,scl);  
	m_address = MCP23008_ADDRESS | address;
}

int mcp23008::detect()
{
	Wire.beginTransmission(m_address);
	return Wire.endTransmission();
}

int mcp23008::write(uint8_t reg, uint8_t value)
{
	Wire.beginTransmission(m_address);
	Wire.write(reg);
	Wire.write(value);
	return Wire.endTransmission();
}

int mcp23008::writeIODIR(uint8_t value)
{
	return write(MCP23008_IODIR, value);
}

int mcp23008::writeGPIO(uint8_t value)
{
	return write(MCP23008_GPIO, value);
}

