#ifndef _MCP23008_H_
#define _MCP23008_H_

#include <Wire.h>

class mcp23008
{
  public:
    mcp23008();
  
    int begin(uint8_t address, uint8_t sda, uint8_t scl);    
    int detect();
    int write(uint8_t reg, uint8_t value);
    int writeIODIR(uint8_t value);    
    int writeGPIO(uint8_t value);

  private:
    uint8_t m_address;

    // base I2C address
    static const int MCP23008_ADDRESS = 0x20;

    // MCP23008 registers
    static const int MCP23008_IODIR   = 0x00;
    static const int MCP23008_GPIO    = 0x09;
};

#endif
