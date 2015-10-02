#include <Arduino.h>
#include "lcd.h"

int lcd::begin(uint8_t address, uint8_t sda, uint8_t scl)
{      
	m_gpio.begin(address, sda, scl);

	int result = m_gpio.detect();
	if (result != 0)
	{
		Serial.println("\nMCP23008 not found");
	}      
	else
	{
		// make all GPIO pins as outputs
		m_gpio.writeIODIR(0x00);

		// setup 4bits mode
		write_byte(0x33,false,200,10000);
		write_byte(0x32,false,200);

		// initialization
		write_byte(0x28,false,200);
		write_byte(0x08,false,200); 
		write_byte(0x01,false,100000);
		write_byte(0x06,false,200);
		write_byte(0x0c,false,200);       
	}
	return result;
}
    
void lcd::write_4bits(uint8_t bits, bool mode)
{
	bits=bits & 0xf;
	bits=bits << 3;
	if (mode) 
	{
		bits=bits|LCD_RS;
	}
	m_gpio.writeGPIO(bits);
	m_gpio.writeGPIO(bits|LCD_E);
	delayMicroseconds(200);
	m_gpio.writeGPIO(bits);
}

void lcd::write_byte(uint8_t bits, bool mode, int postdelay, int middelay)
{
	write_4bits(bits>>4,mode);  
	delayMicroseconds(middelay);
	write_4bits(bits&0xf,mode);
	delayMicroseconds(postdelay);
}

void lcd::write_string(uint8_t line,const char* message)
{
	Serial.println(message);
	write_byte(line,false);
	int max = (strlen(message)<LCD_WIDTH) ? strlen(message) : LCD_WIDTH;      
	for (int i = 0; i<strlen(message); ++i)
	{
		write_byte(message[i],true);
	}
	for (int i = strlen(message); i<LCD_WIDTH; ++i)
	{
		write_byte(' ',true);
	}
}

void lcd::print(const char* line1, const char* line2)
{
    if (line1)
    {
	    write_string(LCD_LINE_1, line1);
    }
    if (line2)
    {
	    write_string(LCD_LINE_2, line2);
    }
}
