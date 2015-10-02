
#include "mcp23008.h"

class lcd
{
  public:
    int begin(uint8_t address, uint8_t sda, uint8_t scl);
    void print(const char* line1, const char* line2);

  private:
    void write_4bits(uint8_t bits, bool mode);
    void write_byte(uint8_t bits, bool mode, int postdelay = 0, int middelay = 0);
    void write_string(uint8_t line,const char* message);

  private:
    mcp23008 m_gpio;
    
    static const int LCD_RW = 0x1;
    static const int LCD_RS = 0x2;
    static const int LCD_E  = 0x4;

    static const int LCD_LINE_1 = 0x80; 
    static const int LCD_LINE_2 = 0xC0;
    static const int LCD_WIDTH  = 16 ;
};
