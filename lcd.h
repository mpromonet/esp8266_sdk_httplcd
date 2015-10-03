/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** lcd.h
** 
** Interface to an LCD 16x2 though a MCP23008 GPIO expander 
**
** -------------------------------------------------------------------------*/


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
        
        static const int LCD_RS = (1 << 4);
        static const int LCD_E  = (1 << 5);
      
        static const int LCD_LINE_1 = 0x80; 
        static const int LCD_LINE_2 = 0xC0;
        static const int LCD_WIDTH  = 16 ;
};
