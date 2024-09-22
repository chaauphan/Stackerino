#include <avr/io.h>
#include <util/delay.h>
#include "spi.h"
#include "timer.h"

#define LCD_CS PORTB2
#define LCD_A0 PORTD7
#define LCD_RESET PORTD6
/*****************************************************************************/
// Don't worry about the struct definitions---just look at the get and set functions

struct B {
    bool operator()(int pad) { return PINB & (1 << pad); } 
    void operator()(int pad, bool value) { 
        PORTB = (value) ? PORTB | (1 << pad) : PORTB & ~(1 << pad);
    }
};

struct C {
    bool operator()(int pad) { return PINC & (1 << pad); } 
    void operator()(int pad, bool value) {
        PORTC = (value) ? PORTC | (1 << pad) : PORTC & ~(1 << pad);
    }
};

struct D {
    bool operator()(int pad) { return PIND & (1 << pad); } 
    void operator()(int pad, bool value) { 
        PORTD = (value) ? PORTD | (1 << pad) : PORTD & ~(1 << pad);
    }
};

// get function: fetches values from pin registers
// Sample Usage:
// get<B>(7) -- gets the value at PINB7 (returns 1 or 0)
// get<D>(5) -- gets the value at PIND5 (returns 1 or 0)
template <typename Group>
bool get(int pad) {
    return Group()(pad);
}

// set function: sets value at port regsiter
// Note: This function DOES NOT disturb the other bits in the register
// Sample Usage:
// set<C>(5, 0) -- sets PORTC5 to 0
// set<B>(6, 1) -- sets PORTC6 to 1
// set<D>(7, true) -- sets PORTD7 to 1
template <typename Group>
void set(int pad, bool value) {
    Group()(pad, value);
}
/*****************************************************************************/

void cmd_st7735(unsigned char cmd) {
  // TODO: set A0 pin LOW
  set<D>(LCD_A0, 0);
  // TODO: set CS pin LOW
  set<B>(LCD_CS, 0);
  SPI_SEND(cmd);
  // TODO: set CS pin HIGH
  set<B>(LCD_CS, 1);
}

void dat_st7735(unsigned char dat) {
  // TODO: set A0 pin HIGH
  set<D>(LCD_A0, 1);
  // TODO: set CS pin LOW
  set<B>(LCD_CS, 0);
  SPI_SEND(dat);
  // TODO: set CS pin HIGH
  set<B>(LCD_CS, 1);
}


void HardwareReset(){
  // TODO: set RESET pin LOW
  set<D>(LCD_RESET, 0);
  _delay_ms(200);
  // TODO: set RESET pin HIGH
  set<D>(LCD_RESET, 1);
  _delay_ms(200);
}

void st7735_init(){
  HardwareReset();
  cmd_st7735(0x01); // SWRESET
  _delay_ms(150);
  cmd_st7735(0x11); // SLPOUT
  _delay_ms(200);
  cmd_st7735(0x3A); // COLMOD
  dat_st7735(0x05);
  _delay_ms(10);
  cmd_st7735(0x29); // DISPON
  _delay_ms(200);
}

void erase(int xStart, int xEnd, int yStart, int yEnd) {
    unsigned int sizeX = (xEnd - xStart) + 1;
    unsigned int sizeY = (yEnd - yStart) + 1;

    cmd_st7735(0x2A); 
    dat_st7735(xStart >> 8);
    dat_st7735(xStart & 0xFF);
    dat_st7735(xEnd >> 8);
    dat_st7735(xEnd & 0xFF); 
    
    cmd_st7735(0x2B); 
    dat_st7735(yStart >> 8);
    dat_st7735(yStart & 0xFF);
    dat_st7735(yEnd >> 8);
    dat_st7735(yEnd & 0xFF); 

    cmd_st7735(0x2C); 

    for (unsigned int i = 0; i < sizeY; ++i) {
      for (unsigned int j = 0; j < sizeX; ++j) {
        dat_st7735(0xFF);
        dat_st7735(0xFF);
      }
    }
}

void clearScreen() {
    erase(0, 127, 0, 127);
}

void testLetter(int letterXS, int letterXE, int letterYS, int letterYE) {
  unsigned int sizeX = (letterXE - letterXS) + 1;
  unsigned int sizeY = (letterYE - letterYS) + 1;

  unsigned int charS[30][10] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //5
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 2, 2, 2, 2, 2, 2, 0}, //10
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 0},
    {0, 2, 2, 1, 1, 1, 1, 1, 1, 0},
    {0, 1, 1, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 2, 2, 2, 2, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 1, 0, 0, 0}, //15
    {0, 0, 0, 1, 1, 1, 1, 2, 2, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 0},
    {0, 2, 2, 2, 2, 2, 2, 1, 1, 0},
    {0, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 0, 0, 0}, //20
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //25
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //30
  };

  cmd_st7735(0x2A);
  dat_st7735(letterXS >> 8);
  dat_st7735(letterXS & 0xFF);
  dat_st7735((letterXE) >> 8);
  dat_st7735((letterXE) & 0xFF);

  // RASET
  // 1. Send RASET command -- look at the datasheet
  cmd_st7735(0x2B);
  dat_st7735(letterYS >> 8);
  dat_st7735(letterYS & 0xFF);
  dat_st7735((letterYE) >> 8);
  dat_st7735((letterYE) & 0xFF);

  // RAMWR
  // 1. Send RAMWR command -- look at the datasheet
  cmd_st7735(0x2C);
  // 2. enter a loop:
  for (unsigned int i = 0; i < sizeY; ++i) {
    for (unsigned int j = 0; j < sizeX; ++j) {
      if (charS[i][j] == 1) {
        dat_st7735(0x00); //black
        dat_st7735(0x00);
      }
      else {
        dat_st7735(0xFF); //white
        dat_st7735(0xFF);
      }
    }
    
  }
}

void testLetterT(int letterXS, int letterXE, int letterYS, int letterYE) {
  unsigned int sizeX = (letterXE - letterXS) + 1;
  unsigned int sizeY = (letterYE - letterYS) + 1;

  unsigned int charS[30][10] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //5
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //10    20
    {0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 0, 0, 0, 0}, //15    15
    {0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 0}, //20    10
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //25    5
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //30    1
  };

  cmd_st7735(0x2A);
  dat_st7735(letterXS >> 8);
  dat_st7735(letterXS & 0xFF);
  dat_st7735((letterXE) >> 8);
  dat_st7735((letterXE) & 0xFF);

  // RASET
  // 1. Send RASET command -- look at the datasheet
  cmd_st7735(0x2B);
  dat_st7735(letterYS >> 8);
  dat_st7735(letterYS & 0xFF);
  dat_st7735((letterYE) >> 8);
  dat_st7735((letterYE) & 0xFF);

  // RAMWR
  // 1. Send RAMWR command -- look at the datasheet
  cmd_st7735(0x2C);
  // 2. enter a loop:
  for (unsigned int i = 0; i < sizeY; ++i) {
    for (unsigned int j = 0; j < sizeX; ++j) {
      if (charS[i][j] == 1) {
        dat_st7735(0x00); //black
        dat_st7735(0x00);
      }
      else {
        dat_st7735(0xFF); //white
        dat_st7735(0xFF);
      }
    }
    
  }
}

void testLetterA(int letterXS, int letterXE, int letterYS, int letterYE) {
  unsigned int sizeX = (letterXE - letterXS) + 1;
  unsigned int sizeY = (letterYE - letterYS) + 1;

  unsigned int charS[30][10] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //5
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //10    20
    {0, 0, 1, 1, 0, 0, 0, 1, 1, 0},
    {0, 0, 1, 1, 0, 0, 0, 1, 1, 0},
    {0, 0, 1, 1, 0, 0, 0, 1, 1, 0},
    {0, 0, 1, 1, 0, 0, 0, 1, 1, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 0}, //15    15
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 0, 1, 1, 0, 0, 0, 1, 1, 0},
    {0, 0, 1, 1, 0, 0, 0, 1, 1, 0},
    {0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 1, 0, 0, 0}, //20    10
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //25    5
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //30    1
  };


  cmd_st7735(0x2A);
  dat_st7735(letterXS >> 8);
  dat_st7735(letterXS & 0xFF);
  dat_st7735((letterXE) >> 8);
  dat_st7735((letterXE) & 0xFF);

  // RASET
  // 1. Send RASET command -- look at the datasheet
  cmd_st7735(0x2B);
  dat_st7735(letterYS >> 8);
  dat_st7735(letterYS & 0xFF);
  dat_st7735((letterYE) >> 8);
  dat_st7735((letterYE) & 0xFF);

  // RAMWR
  // 1. Send RAMWR command -- look at the datasheet
  cmd_st7735(0x2C);
  // 2. enter a loop:
  for (unsigned int i = 0; i < sizeY; ++i) {
    for (unsigned int j = 0; j < sizeX; ++j) {
      if (charS[i][j] == 1) {
        dat_st7735(0x00); //black
        dat_st7735(0x00);
      }
      else {
        dat_st7735(0xFF); //white
        dat_st7735(0xFF);
      }
    }
    
  }
}

void testLetterC(int letterXS, int letterXE, int letterYS, int letterYE) {
  unsigned int sizeX = (letterXE - letterXS) + 1;
  unsigned int sizeY = (letterYE - letterYS) + 1;

  unsigned int charS[30][10] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //5
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //10    20
    {0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
    {0, 1, 1, 0, 0, 0, 0, 1, 1, 0},
    {0, 1, 1, 0, 0, 0, 0, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 0}, //15    15
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 0},
    {0, 1, 1, 0, 0, 0, 0, 1, 1, 0},
    {0, 1, 1, 0, 0, 0, 0, 1, 1, 0},
    {0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 1, 0, 0, 0}, //20    10
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //25    5
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //30    1
  };

  cmd_st7735(0x2A);
  dat_st7735(letterXS >> 8);
  dat_st7735(letterXS & 0xFF);
  dat_st7735((letterXE) >> 8);
  dat_st7735((letterXE) & 0xFF);

  // RASET
  // 1. Send RASET command -- look at the datasheet
  cmd_st7735(0x2B);
  dat_st7735(letterYS >> 8);
  dat_st7735(letterYS & 0xFF);
  dat_st7735((letterYE) >> 8);
  dat_st7735((letterYE) & 0xFF);

  // RAMWR
  // 1. Send RAMWR command -- look at the datasheet
  cmd_st7735(0x2C);
  // 2. enter a loop:
  for (unsigned int i = 0; i < sizeY; ++i) {
    for (unsigned int j = 0; j < sizeX; ++j) {
      if (charS[i][j] == 1) {
        dat_st7735(0x00); //black
        dat_st7735(0x00);
      }
      else {
        dat_st7735(0xFF); //white
        dat_st7735(0xFF);
      }
    }
    
  }
}

void testLetterK(int letterXS, int letterXE, int letterYS, int letterYE) {
  unsigned int sizeX = (letterXE - letterXS) + 1;
  unsigned int sizeY = (letterYE - letterYS) + 1;

  unsigned int charS[30][10] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //5
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //10    20
    {0, 1, 1, 0, 0, 0, 0, 1, 1, 0},
    {0, 1, 1, 0, 0, 0, 0, 1, 1, 0},
    {0, 0, 0, 1, 1, 0, 0, 1, 1, 0},
    {0, 0, 0, 1, 1, 0, 0, 1, 1, 0},
    {0, 0, 0, 0, 0, 1, 1, 1, 1, 0}, //15    15
    {0, 0, 0, 0, 0, 1, 1, 1, 1, 0},
    {0, 0, 0, 1, 1, 0, 0, 1, 1, 0},
    {0, 0, 0, 1, 1, 0, 0, 1, 1, 0},
    {0, 1, 1, 0, 0, 0, 0, 1, 1, 0},
    {0, 1, 1, 0, 0, 0, 0, 1, 1, 0}, //20    10
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //25    5
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //30    1
  };

  cmd_st7735(0x2A);
  dat_st7735(letterXS >> 8);
  dat_st7735(letterXS & 0xFF);
  dat_st7735((letterXE) >> 8);
  dat_st7735((letterXE) & 0xFF);

  // RASET
  // 1. Send RASET command -- look at the datasheet
  cmd_st7735(0x2B);
  dat_st7735(letterYS >> 8);
  dat_st7735(letterYS & 0xFF);
  dat_st7735((letterYE) >> 8);
  dat_st7735((letterYE) & 0xFF);

  // RAMWR
  // 1. Send RAMWR command -- look at the datasheet
  cmd_st7735(0x2C);
  // 2. enter a loop:
  for (unsigned int i = 0; i < sizeY; ++i) {
    for (unsigned int j = 0; j < sizeX; ++j) {
      if (charS[i][j] == 1) {
        dat_st7735(0x00); //black
        dat_st7735(0x00);
      }
      else {
        dat_st7735(0xFF); //white
        dat_st7735(0xFF);
      }
    }
    
  }
}

void testLetterE(int letterXS, int letterXE, int letterYS, int letterYE) {
  unsigned int sizeX = (letterXE - letterXS) + 1;
  unsigned int sizeY = (letterYE - letterYS) + 1;

  unsigned int charS[30][10] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //5
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //10    20
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 0},
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 0}, //15    15
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 0}, //20    10
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //25    5
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  };

  cmd_st7735(0x2A);
  dat_st7735(letterXS >> 8);
  dat_st7735(letterXS & 0xFF);
  dat_st7735((letterXE) >> 8);
  dat_st7735((letterXE) & 0xFF);

  // RASET
  // 1. Send RASET command -- look at the datasheet
  cmd_st7735(0x2B);
  dat_st7735(letterYS >> 8);
  dat_st7735(letterYS & 0xFF);
  dat_st7735((letterYE) >> 8);
  dat_st7735((letterYE) & 0xFF);

  // RAMWR
  // 1. Send RAMWR command -- look at the datasheet
  cmd_st7735(0x2C);
  // 2. enter a loop:
  for (unsigned int i = 0; i < sizeY; ++i) {
    for (unsigned int j = 0; j < sizeX; ++j) {
      if (charS[i][j] == 1) {
        dat_st7735(0x00); //black
        dat_st7735(0x00);
      }
      else {
        dat_st7735(0xFF); //white
        dat_st7735(0xFF);
      }
    }
    
  }
}

void testLetterR(int letterXS, int letterXE, int letterYS, int letterYE) {
  unsigned int sizeX = (letterXE - letterXS) + 1;
  unsigned int sizeY = (letterYE - letterYS) + 1;

  unsigned int charS[30][10] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //5
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //10    20
    {0, 1, 1, 0, 0, 0, 0, 1, 1, 0},
    {0, 1, 1, 0, 0, 0, 0, 1, 1, 0},
    {0, 0, 0, 1, 1, 0, 0, 1, 1, 0},
    {0, 0, 0, 1, 1, 0, 0, 1, 1, 0},
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 0}, //15    15
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 0},
    {0, 1, 1, 0, 0, 0, 0, 1, 1, 0},
    {0, 1, 1, 0, 0, 0, 0, 1, 1, 0},
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 0},
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 0}, //20    10
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //25    5
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //30    1
  };

  cmd_st7735(0x2A);
  dat_st7735(letterXS >> 8);
  dat_st7735(letterXS & 0xFF);
  dat_st7735((letterXE) >> 8);
  dat_st7735((letterXE) & 0xFF);

  // RASET
  // 1. Send RASET command -- look at the datasheet
  cmd_st7735(0x2B);
  dat_st7735(letterYS >> 8);
  dat_st7735(letterYS & 0xFF);
  dat_st7735((letterYE) >> 8);
  dat_st7735((letterYE) & 0xFF);

  // RAMWR
  // 1. Send RAMWR command -- look at the datasheet
  cmd_st7735(0x2C);
  // 2. enter a loop:
  for (unsigned int i = 0; i < sizeY; ++i) {
    for (unsigned int j = 0; j < sizeX; ++j) {
      if (charS[i][j] == 1) {
        dat_st7735(0x00); //black
        dat_st7735(0x00);
      }
      else {
        dat_st7735(0xFF); //white
        dat_st7735(0xFF);
      }
    }
    
  }
}

void testLetterI(int letterXS, int letterXE, int letterYS, int letterYE) {
  unsigned int sizeX = (letterXE - letterXS) + 1;
  unsigned int sizeY = (letterYE - letterYS) + 1;

  unsigned int charS[30][10] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //5
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //10    20
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 0, 0, 0, 0}, //15    15
    {0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 0}, //20    10
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //25    5
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  };

  cmd_st7735(0x2A);
  dat_st7735(letterXS >> 8);
  dat_st7735(letterXS & 0xFF);
  dat_st7735((letterXE) >> 8);
  dat_st7735((letterXE) & 0xFF);

  // RASET
  // 1. Send RASET command -- look at the datasheet
  cmd_st7735(0x2B);
  dat_st7735(letterYS >> 8);
  dat_st7735(letterYS & 0xFF);
  dat_st7735((letterYE) >> 8);
  dat_st7735((letterYE) & 0xFF);

  // RAMWR
  // 1. Send RAMWR command -- look at the datasheet
  cmd_st7735(0x2C);
  // 2. enter a loop:
  for (unsigned int i = 0; i < sizeY; ++i) {
    for (unsigned int j = 0; j < sizeX; ++j) {
      if (charS[i][j] == 1) {
        dat_st7735(0x00); //black
        dat_st7735(0x00);
      }
      else {
        dat_st7735(0xFF); //white
        dat_st7735(0xFF);
      }
    }
    
  }
}

void testLetterN(int letterXS, int letterXE, int letterYS, int letterYE) {
  unsigned int sizeX = (letterXE - letterXS) + 1;
  unsigned int sizeY = (letterYE - letterYS) + 1;

  unsigned int charS[30][10] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //5
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //10    20
    {0, 1, 1, 0, 0, 0, 0, 1, 1, 0},
    {0, 1, 1, 0, 0, 0, 0, 1, 1, 0},
    {0, 1, 1, 0, 0, 0, 0, 1, 1, 0},
    {0, 1, 1, 0, 0, 0, 0, 1, 1, 0},
    {0, 1, 1, 1, 1, 0, 0, 1, 1, 0}, //15    15
    {0, 1, 1, 1, 1, 0, 0, 1, 1, 0},
    {0, 1, 1, 0, 0, 1, 1, 1, 1, 0},
    {0, 1, 1, 0, 0, 1, 1, 1, 1, 0},
    {0, 1, 1, 0, 0, 0, 0, 1, 1, 0},
    {0, 1, 1, 0, 0, 0, 0, 1, 1, 0}, //20    10
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //25    5
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  };

  cmd_st7735(0x2A);
  dat_st7735(letterXS >> 8);
  dat_st7735(letterXS & 0xFF);
  dat_st7735((letterXE) >> 8);
  dat_st7735((letterXE) & 0xFF);

  // RASET
  // 1. Send RASET command -- look at the datasheet
  cmd_st7735(0x2B);
  dat_st7735(letterYS >> 8);
  dat_st7735(letterYS & 0xFF);
  dat_st7735((letterYE) >> 8);
  dat_st7735((letterYE) & 0xFF);

  // RAMWR
  // 1. Send RAMWR command -- look at the datasheet
  cmd_st7735(0x2C);
  // 2. enter a loop:
  for (unsigned int i = 0; i < sizeY; ++i) {
    for (unsigned int j = 0; j < sizeX; ++j) {
      if (charS[i][j] == 1) {
        dat_st7735(0x00); //black
        dat_st7735(0x00);
      }
      else {
        dat_st7735(0xFF); //white
        dat_st7735(0xFF);
      }
    }
    
  }
}

void testLetterO(int letterXS, int letterXE, int letterYS, int letterYE) {
  unsigned int sizeX = (letterXE - letterXS) + 1;
  unsigned int sizeY = (letterYE - letterYS) + 1;

  unsigned int charS[30][10] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //5
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //10    20
    {0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
    {0, 1, 1, 0, 0, 0, 0, 1, 1, 0},
    {0, 1, 1, 0, 0, 0, 0, 1, 1, 0},
    {0, 1, 1, 0, 0, 0, 0, 1, 1, 0}, //15    15
    {0, 1, 1, 0, 0, 0, 0, 1, 1, 0},
    {0, 1, 1, 0, 0, 0, 0, 1, 1, 0},
    {0, 1, 1, 0, 0, 0, 0, 1, 1, 0},
    {0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 1, 0, 0, 0}, //20    10
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //25    5
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  };

  cmd_st7735(0x2A);
  dat_st7735(letterXS >> 8);
  dat_st7735(letterXS & 0xFF);
  dat_st7735((letterXE) >> 8);
  dat_st7735((letterXE) & 0xFF);

  // RASET
  // 1. Send RASET command -- look at the datasheet
  cmd_st7735(0x2B);
  dat_st7735(letterYS >> 8);
  dat_st7735(letterYS & 0xFF);
  dat_st7735((letterYE) >> 8);
  dat_st7735((letterYE) & 0xFF);

  // RAMWR
  // 1. Send RAMWR command -- look at the datasheet
  cmd_st7735(0x2C);
  // 2. enter a loop:
  for (unsigned int i = 0; i < sizeY; ++i) {
    for (unsigned int j = 0; j < sizeX; ++j) {
      if (charS[i][j] == 1) {
        dat_st7735(0x00); //black
        dat_st7735(0x00);
      }
      else {
        dat_st7735(0xFF); //white
        dat_st7735(0xFF);
      }
    }
    
  }
}

void testLetterG(int letterXS, int letterXE, int letterYS, int letterYE) {
  unsigned int sizeX = (letterXE - letterXS) + 1;
  unsigned int sizeY = (letterYE - letterYS) + 1;

  unsigned int charS[30][10] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //5
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //10    20
    {0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
    {0, 1, 1, 0, 0, 0, 0, 1, 1, 0},
    {0, 1, 1, 0, 0, 0, 0, 1, 1, 0},
    {0, 1, 1, 1, 1, 0, 0, 1, 1, 0}, //15    15
    {0, 1, 1, 1, 1, 0, 0, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 0},
    {0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 1, 0, 0, 0}, //20    10
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //25    5
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  };

  cmd_st7735(0x2A);
  dat_st7735(letterXS >> 8);
  dat_st7735(letterXS & 0xFF);
  dat_st7735((letterXE) >> 8);
  dat_st7735((letterXE) & 0xFF);

  // RASET
  // 1. Send RASET command -- look at the datasheet
  cmd_st7735(0x2B);
  dat_st7735(letterYS >> 8);
  dat_st7735(letterYS & 0xFF);
  dat_st7735((letterYE) >> 8);
  dat_st7735((letterYE) & 0xFF);

  // RAMWR
  // 1. Send RAMWR command -- look at the datasheet
  cmd_st7735(0x2C);
  // 2. enter a loop:
  for (unsigned int i = 0; i < sizeY; ++i) {
    for (unsigned int j = 0; j < sizeX; ++j) {
      if (charS[i][j] == 1) {
        dat_st7735(0x00); //black
        dat_st7735(0x00);
      }
      else {
        dat_st7735(0xFF); //white
        dat_st7735(0xFF);
      }
    }
    
  }
}

void testLetterM(int letterXS, int letterXE, int letterYS, int letterYE) {
  unsigned int sizeX = (letterXE - letterXS) + 1;
  unsigned int sizeY = (letterYE - letterYS) + 1;

  unsigned int charS[30][10] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //5
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //10    20
    {1, 1, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 1, 0, 0, 1, 1, 0, 0, 1, 1}, //15    15
    {1, 1, 0, 0, 1, 1, 0, 0, 1, 1},
    {1, 1, 1, 1, 0, 0, 1, 1, 1, 1},
    {1, 1, 1, 1, 0, 0, 1, 1, 1, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 1, 1}, //20    10
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //25    5
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  };

  cmd_st7735(0x2A);
  dat_st7735(letterXS >> 8);
  dat_st7735(letterXS & 0xFF);
  dat_st7735((letterXE) >> 8);
  dat_st7735((letterXE) & 0xFF);

  // RASET
  // 1. Send RASET command -- look at the datasheet
  cmd_st7735(0x2B);
  dat_st7735(letterYS >> 8);
  dat_st7735(letterYS & 0xFF);
  dat_st7735((letterYE) >> 8);
  dat_st7735((letterYE) & 0xFF);

  // RAMWR
  // 1. Send RAMWR command -- look at the datasheet
  cmd_st7735(0x2C);
  // 2. enter a loop:
  for (unsigned int i = 0; i < sizeY; ++i) {
    for (unsigned int j = 0; j < sizeX; ++j) {
      if (charS[i][j] == 1) {
        dat_st7735(0x00); //black
        dat_st7735(0x00);
      }
      else {
        dat_st7735(0xFF); //white
        dat_st7735(0xFF);
      }
    }
    
  }
}
void testLetterV(int letterXS, int letterXE, int letterYS, int letterYE) {
  unsigned int sizeX = (letterXE - letterXS) + 1;
  unsigned int sizeY = (letterYE - letterYS) + 1;

  unsigned int charS[30][10] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //5
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //10    20
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
    {0, 0, 1, 1, 0, 0, 1, 1, 0, 0},
    {0, 0, 1, 1, 0, 0, 1, 1, 0, 0}, //15    15
    {0, 0, 1, 1, 0, 0, 1, 1, 0, 0},
    {1, 1, 0, 0, 0, 0, 0, 0, 1, 1,},
    {1, 1, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 1, 1}, //20    10
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //25    5
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  };

  cmd_st7735(0x2A);
  dat_st7735(letterXS >> 8);
  dat_st7735(letterXS & 0xFF);
  dat_st7735((letterXE) >> 8);
  dat_st7735((letterXE) & 0xFF);

  // RASET
  // 1. Send RASET command -- look at the datasheet
  cmd_st7735(0x2B);
  dat_st7735(letterYS >> 8);
  dat_st7735(letterYS & 0xFF);
  dat_st7735((letterYE) >> 8);
  dat_st7735((letterYE) & 0xFF);

  // RAMWR
  // 1. Send RAMWR command -- look at the datasheet
  cmd_st7735(0x2C);
  // 2. enter a loop:
  for (unsigned int i = 0; i < sizeY; ++i) {
    for (unsigned int j = 0; j < sizeX; ++j) {
      if (charS[i][j] == 1) {
        dat_st7735(0x00); //black
        dat_st7735(0x00);
      }
      else {
        dat_st7735(0xFF); //white
        dat_st7735(0xFF);
      }
    }
    
  }
}

void testConfettiBlue(int letterXS, int letterXE, int letterYS, int letterYE) {
  clearScreen();
  unsigned int sizeX = (letterXE - letterXS) + 1;
  unsigned int sizeY = (letterYE - letterYS) + 1;

  unsigned int charS[30][10] = {
    
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //5
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //10    20
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 0, 0, 0, 0, 0, 0}, //15    15
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 1, 0}, //20    10
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 0, 0, 0, 0, 0, 0}, //25    5
    {0, 1, 1, 1, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  };

  cmd_st7735(0x2A);
  dat_st7735(letterXS >> 8);
  dat_st7735(letterXS & 0xFF);
  dat_st7735((letterXE) >> 8);
  dat_st7735((letterXE) & 0xFF);

  // RASET
  // 1. Send RASET command -- look at the datasheet
  cmd_st7735(0x2B);
  dat_st7735(letterYS >> 8);
  dat_st7735(letterYS & 0xFF);
  dat_st7735((letterYE) >> 8);
  dat_st7735((letterYE) & 0xFF);

  // RAMWR
  // 1. Send RAMWR command -- look at the datasheet
  cmd_st7735(0x2C);
  // 2. enter a loop:
  for (unsigned int i = 0; i < sizeY; ++i) {
    for (unsigned int j = 0; j < sizeX; ++j) {
      if (charS[i][j] == 1) {
        dat_st7735(0xB9); //black
        dat_st7735(0xA4);
      }
      else {
        dat_st7735(0xFF); //white
        dat_st7735(0xFF);
      }
    }
    
  }
}

void testConfettiRed(int letterXS, int letterXE, int letterYS, int letterYE) {
  clearScreen();
  unsigned int sizeX = (letterXE - letterXS) + 1;
  unsigned int sizeY = (letterYE - letterYS) + 1;

  unsigned int charS[30][10] = {
    
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //5
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //10    20
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 0, 0, 0, 0, 0, 0}, //15    15
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 1, 0}, //20    10
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 0, 0, 0, 0, 0, 0}, //25    5
    {0, 1, 1, 1, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  };

  cmd_st7735(0x2A);
  dat_st7735(letterXS >> 8);
  dat_st7735(letterXS & 0xFF);
  dat_st7735((letterXE) >> 8);
  dat_st7735((letterXE) & 0xFF);

  // RASET
  // 1. Send RASET command -- look at the datasheet
  cmd_st7735(0x2B);
  dat_st7735(letterYS >> 8);
  dat_st7735(letterYS & 0xFF);
  dat_st7735((letterYE) >> 8);
  dat_st7735((letterYE) & 0xFF);

  // RAMWR
  // 1. Send RAMWR command -- look at the datasheet
  cmd_st7735(0x2C);
  // 2. enter a loop:
  for (unsigned int i = 0; i < sizeY; ++i) {
    for (unsigned int j = 0; j < sizeX; ++j) {
      if (charS[i][j] == 1) {
        dat_st7735(0x83);
        dat_st7735(0xFF);
      }
      else {
        dat_st7735(0xFF); //white
        dat_st7735(0xFF);
      }
    }
    
  }
}

void testConfettiGreen(int letterXS, int letterXE, int letterYS, int letterYE) {
  clearScreen();
  unsigned int sizeX = (letterXE - letterXS) + 1;
  unsigned int sizeY = (letterYE - letterYS) + 1;

  unsigned int charS[30][10] = {
    
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //5
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //10    20
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 0, 0, 0, 0, 0, 0}, //15    15
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 1, 0}, //20    10
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 0, 0, 0, 0, 0, 0}, //25    5
    {0, 1, 1, 1, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  };

  

  cmd_st7735(0x2A);
  dat_st7735(letterXS >> 8);
  dat_st7735(letterXS & 0xFF);
  dat_st7735((letterXE) >> 8);
  dat_st7735((letterXE) & 0xFF);

  // RASET
  // 1. Send RASET command -- look at the datasheet
  cmd_st7735(0x2B);
  dat_st7735(letterYS >> 8);
  dat_st7735(letterYS & 0xFF);
  dat_st7735((letterYE) >> 8);
  dat_st7735((letterYE) & 0xFF);

  // RAMWR
  // 1. Send RAMWR command -- look at the datasheet
  cmd_st7735(0x2C);
  // 2. enter a loop:
  for (unsigned int i = 0; i < sizeY; ++i) {
    for (unsigned int j = 0; j < sizeX; ++j) {
      if (charS[i][j] == 1) {
        dat_st7735(0x4F);
        dat_st7735(0xA3);
      }
      else {
        dat_st7735(0xFF); //white
        dat_st7735(0xFF);
      }
    }
    
  }
}

void testConfettiYellow(int letterXS, int letterXE, int letterYS, int letterYE) {
  clearScreen();
  unsigned int sizeX = (letterXE - letterXS) + 1;
  unsigned int sizeY = (letterYE - letterYS) + 1;

  unsigned int charS[30][10] = {
    
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //5
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, //10    20
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 0, 0, 0, 0, 0, 0}, //15    15
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 1, 1, 1, 0}, //20    10
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 0, 0, 0, 0, 0, 0}, //25    5
    {0, 1, 1, 1, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  };

  

  cmd_st7735(0x2A);
  dat_st7735(letterXS >> 8);
  dat_st7735(letterXS & 0xFF);
  dat_st7735((letterXE) >> 8);
  dat_st7735((letterXE) & 0xFF);

  // RASET
  // 1. Send RASET command -- look at the datasheet
  cmd_st7735(0x2B);
  dat_st7735(letterYS >> 8);
  dat_st7735(letterYS & 0xFF);
  dat_st7735((letterYE) >> 8);
  dat_st7735((letterYE) & 0xFF);

  // RAMWR
  // 1. Send RAMWR command -- look at the datasheet
  cmd_st7735(0x2C);
  // 2. enter a loop:
  for (unsigned int i = 0; i < sizeY; ++i) {
    for (unsigned int j = 0; j < sizeX; ++j) {
      if (charS[i][j] == 1) {
        dat_st7735(0x4F);
        dat_st7735(0xF9);
      }
      else {
        dat_st7735(0xFF); //white
        dat_st7735(0xFF);
      }
    }
    
  }
}

/* GLOBAL VARIABLES */
bool gamePlaying = 0;
bool gameOver = 0;
unsigned char xS = 92; // moving block coords
unsigned char xE = 127;
unsigned char yS = 0;
unsigned char yE = 12;
unsigned char blockNum = 3;
unsigned char level = 1;
unsigned char prevBlockXS = 48;
unsigned char prevBlockXE = 83;
unsigned char speed = 7;


void drawFirstBlock(int blockXS, int blockXE, int blockYS, int blockYE) {
  erase(0, 128, yS, yE);
  unsigned int sizeX = (blockXE - blockXS) + 1;
  unsigned int sizeY = (blockYE - blockYS) + 1;

  unsigned int block[12][36] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
  };

  cmd_st7735(0x2A);
  dat_st7735(blockXS >> 8);
  dat_st7735(blockXS & 0xFF);
  dat_st7735((blockXE) >> 8);
  dat_st7735((blockXE) & 0xFF);

  // RASET
  // 1. Send RASET command -- look at the datasheet
  cmd_st7735(0x2B);
  dat_st7735(blockYS >> 8);
  dat_st7735(blockYS & 0xFF);
  dat_st7735((blockYE) >> 8);
  dat_st7735((blockYE) & 0xFF);

  // RAMWR
  // 1. Send RAMWR command -- look at the datasheet
  cmd_st7735(0x2C);
  
  for (unsigned int i = 0; i < sizeY; ++i) {
    for (unsigned int j = 0; j < sizeX; ++j) {
      if (block[i][j] == 1) {
        dat_st7735(0x83);
        dat_st7735(0xFF);
      }
      else {
        dat_st7735(0xB4); 
        dat_st7735(0xFF);
      }
    }
  }
}

void draw2Block(int blockXS, int blockXE, int blockYS, int blockYE) {
  erase(0, 128, yS, yE);
  unsigned int sizeX = (blockXE - blockXS) + 1;
  unsigned int sizeY = (blockYE - blockYS) + 1;

  unsigned int block[12][28] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
  };

  cmd_st7735(0x2A);
  dat_st7735(blockXS >> 8);
  dat_st7735(blockXS & 0xFF);
  dat_st7735((blockXE) >> 8);
  dat_st7735((blockXE) & 0xFF);

  // RASET
  // 1. Send RASET command -- look at the datasheet
  cmd_st7735(0x2B);
  dat_st7735(blockYS >> 8);
  dat_st7735(blockYS & 0xFF);
  dat_st7735((blockYE) >> 8);
  dat_st7735((blockYE) & 0xFF);

  // RAMWR
  // 1. Send RAMWR command -- look at the datasheet
  cmd_st7735(0x2C);
  
  for (unsigned int i = 0; i < sizeY; ++i) {
    for (unsigned int j = 0; j < sizeX; ++j) {
      if (block[i][j] == 1) {
        dat_st7735(0x83);
        dat_st7735(0xFF);
      }
      else {
        dat_st7735(0xB4); 
        dat_st7735(0xFF);
      }
    }
  }
}

void drawBlock(int blockXS, int blockXE, int blockYS, int blockYE) {
  unsigned int sizeX = (blockXE - blockXS) + 1;
  unsigned int sizeY = (blockYE - blockYS) + 1;

  unsigned int block[12][36] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
  };

  cmd_st7735(0x2A);
  dat_st7735(blockXS >> 8);
  dat_st7735(blockXS & 0xFF);
  dat_st7735((blockXE) >> 8);
  dat_st7735((blockXE) & 0xFF);

  // RASET
  // 1. Send RASET command -- look at the datasheet
  cmd_st7735(0x2B);
  dat_st7735(blockYS >> 8);
  dat_st7735(blockYS & 0xFF);
  dat_st7735((blockYE) >> 8);
  dat_st7735((blockYE) & 0xFF);

  // RAMWR
  // 1. Send RAMWR command -- look at the datasheet
  cmd_st7735(0x2C);
  
  for (unsigned int i = 0; i < sizeY; ++i) {
    for (unsigned int j = 0; j < sizeX; ++j) {
      if (block[i][j] == 1) {
        dat_st7735(0x83);
        dat_st7735(0xFF);
      }
      else {
        dat_st7735(0xB4); 
        dat_st7735(0xFF);
      }
    }
  }
}

enum moveStates {init, moveRight, moveLeft, waitMoveLeft, waitMoveRight};
int tickFctMove(int state);

unsigned char timer;
int tickFctMove(int state) {
  
  switch(state) {
    case init:
      timer = 0;
      yS = yS + 13;
      yE = yE + 13;
      xE = 128;
      if ((gamePlaying == true)) {
        state = moveRight;
      }
      else {
        state = init;
      }
      break;

    case moveRight:
      if (xS == 0) {
        timer = 0;
        state = moveLeft;
      }
      if (xS > 0) {
        timer = 0;
        state = waitMoveRight;
      }
      if ((gamePlaying == false)) {
        clearScreen();
        state = init;
      }
      break;

    case moveLeft:
      if (xE == 127) {
        timer = 0;
        state = moveRight;
      }
      if (xE < 127) {
        timer = 0;
        state = waitMoveLeft;
      }
      if ((gamePlaying == false)) {
        clearScreen();
        state = init;
      }
      break;

    case waitMoveLeft:
      if (timer < speed) {
        timer++;
        state = waitMoveLeft;
      }
      if (timer >= speed) {
        timer = 0;
        state = moveLeft;
      }
      
      break;

    case waitMoveRight:
      if (timer < speed) {
        timer++;
        state = waitMoveRight;
      }
      if (timer >= speed) {
        timer = 0;
        state = moveRight;
      }
      
      break;

    default:
      state = init;
      break;

  }

  switch(state) {
    case init:
      xE = 127;
      break;

    case moveRight:
      erase((xE + 1), (xE + 1), yS, yE);
      drawBlock(xS, xE, yS, yE);
      xS = xS - 1;
      xE = xE - 1;
      timer = 0;
      break;

    case moveLeft:
      erase((xS - 1), (xS - 1), yS, yE);
      drawBlock(xS, xE, yS, yE);
      xS = xS + 1;
      xE = xE + 1;
      timer = 0;
      break;

    case waitMoveLeft:
      break;

    case waitMoveRight:
      break;

    default:
      break;
  }
  return state;
}

enum checkPress {waitPress, buttonPressed, checkAlign};
int tickFctCheckPress(int state);

unsigned char pressed;
int tickFctCheckPress(int state) {
  switch(state) {
    case waitPress:
      if (get<C>(0) == 0) {
        state = waitPress;
      }
      if ((get<C>(0) == 1) && (gamePlaying == true)) {
        pressed = 0;
        state = buttonPressed;
      }
      break;

    case buttonPressed:
      if (get<C>(0) == 0) {
        state = buttonPressed;
      }
      if (get<C>(0) == 1) {
        pressed = 1;
        state = checkAlign;
      }
      break;
    
    case checkAlign:
      if (get<C>(0) == 0) {
        pressed = 0;
        
        if (blockNum == 3) {
          // 83   48    3 blocks
          // 83   60    2 block left side
          // 71   48    2 block right side
          // 59   48    1 block right side
          // 83   72    1 block left side
          // 71   60
          // check alignment and account if too far right or too far left (~3 pixels max)
          // perfect block alignment
          if (((xS >= 45) && (xE <= 86))) {
            drawFirstBlock(48, 83, (yS), (yE));
            yS = yS + 13;
            yE = yE + 13;
            prevBlockXS = 48;
            prevBlockXE = 83;
            blockNum = 3;
            ++level;
            if (level >= 4 && level <= 8) {
              speed -= 1;
            }
            
          }
          // too far right by 1 block
          else if (xS >= 34 && xS <= 44) {
            drawFirstBlock(48, 71, (yS), (yE));
            xS = xS + 12;
            yS = yS + 13;
            yE = yE + 13;
            prevBlockXS = 48;
            prevBlockXE = 71;
            blockNum = 2;
            ++level;
            if (level >= 4 && level <= 8) {
              speed -= 1;
            }
          }
          // too far left by 1 block
          else if (xE >= 87 && xE <= 98) {
            drawFirstBlock(60, 83, (yS), (yE));
            xS = xS + 12;
            yS = yS + 13;
            yE = yE + 13;
            prevBlockXS = 60;
            prevBlockXE = 83;
            blockNum = 2;
            ++level;
            if (level >= 4 && level <= 8) {
              speed -= 1;
            }
          }
          // too far right by 2 blocks
          else if (xS >= 20 && xS <= 33) {
            drawFirstBlock(48, 59, (yS), (yE));
            xS = xS + 24;
            yS = yS + 13;
            yE = yE + 13;
            prevBlockXS = 48;
            prevBlockXE = 59;
            blockNum = 1;
            ++level;
            if (level >= 4 && level <= 8) {
              speed -= 1;
            }
          }
          // too far left by 2 blocks
          else if (xE >= 99 && xE <= 109) {
            drawFirstBlock(72, 83, (yS), (yE));
            xS = xS + 24;
            yS = yS + 13;
            yE = yE + 13;
            prevBlockXS = 72;
            prevBlockXE = 83;
            blockNum = 1;
            ++level;
            if (level >= 4 && level <= 8) {
              speed -= 1;
            }
          }
          else {
              gameOver = true;
            }
        }

        else if (blockNum == 2) {
          // perfect alignment 
          if ((prevBlockXS == 48) && (prevBlockXE == 71)) {
            if (((xS >= 43) && (xE <= 75))) {
            drawFirstBlock(48, 71, (yS), (yE));
            yS = yS + 13;
            yE = yE + 13;
            prevBlockXS = 48;
            prevBlockXE = 71;
            blockNum = 2;
            ++level;
            if (level >= 4 && level <= 8) {
              speed -= 1;
            }
            }
            //far right
            else if (xS >= 34 && xS <= 42) {
            drawFirstBlock(48, 59, (yS), (yE));
            xS = xS + 12;
            yS = yS + 13;
            yE = yE + 13;
            prevBlockXS = 48;
            prevBlockXE = 59;
            blockNum = 1;
            ++level;
            if (level >= 4 && level <= 8) {
              speed -= 1;
            }
            }
            //far left (middle block)
            else if (xE >= 72 && xE <= 85) {
            drawFirstBlock(60, 71, (yS), (yE));
            xS = xS + 12;
            yS = yS + 13;
            yE = yE + 13;
            prevBlockXS = 60;
            prevBlockXE = 71;
            blockNum = 1;
            ++level;
            if (level >= 4 && level <= 8) {
              speed -= 1;
            }
            }
            else {
              gameOver = true;
            }
          }

          else if ((prevBlockXS == 60) && (prevBlockXE == 83)) {
            // perfect alignment 
            if (((xS >= 56) && (xE <= 87))) {
            drawFirstBlock(60, 83, (yS), (yE));
            yS = yS + 13;
            yE = yE + 13;
            prevBlockXS = 60;
            prevBlockXE = 83;
            blockNum = 2;
            ++level;
            if (level >= 4 && level <= 8) {
              speed -= 1;
            }
            }
            //far left
            else if ((xE >= 86) && (xE <= 98)) {
            drawFirstBlock(72, 83, (yS), (yE));
            xS = xS + 12;
            yS = yS + 13;
            yE = yE + 13;
            prevBlockXS = 72;
            prevBlockXE = 83;
            blockNum = 1;
            ++level;
            if (level >= 4 && level <= 8) {
              speed -= 1;
            }
            }
            //far right (middle) 
            else if ((xS >= 46) && (xS <= 59)) {
            drawFirstBlock(60, 71, (yS), (yE));
            xS = xS + 12;
            yS = yS + 13;
            yE = yE + 13;
            prevBlockXS = 60;
            prevBlockXE = 71;
            blockNum = 1;
            ++level;
            if (level >= 4 && level <= 8) {
              speed -= 1;
            }
            }
            else {
              gameOver = true;
            }
          }
        }

        else if (blockNum == 1) {
          if ((prevBlockXS == 60) && (prevBlockXE == 71)) {
            if (((xS >= 56) && (xE <= 74))) {
              drawFirstBlock(60, 71, (yS), (yE));
              yS = yS + 13;
              yE = yE + 13;
              prevBlockXS = 60;
              prevBlockXE = 71;
              blockNum = 1;
              ++level;
            if (level >= 4 && level <= 8) {
              speed -= 1;
            }
            }
            else {
              gameOver = true;
            }
          }
          else if ((prevBlockXS == 72) && (prevBlockXE == 83)) {
            if (((xS >= 68) && (xE <= 86))) {
              drawFirstBlock(72, 83, (yS), (yE));
              yS = yS + 13;
              yE = yE + 13;
              prevBlockXS = 72;
              prevBlockXE = 83;
              blockNum = 1;
              ++level;
            if (level >= 4 && level <= 8) {
              speed -= 1;
            }
            }
            else {
              gameOver = true;
            }
          }
          else if ((prevBlockXS == 48) && (prevBlockXE == 59)) {
            if (((xS >= 43) && (xE <= 63))) {
              drawFirstBlock(48, 59, (yS), (yE));
              yS = yS + 13;
              yE = yE + 13;
              prevBlockXS = 48;
              prevBlockXE = 59;
              blockNum = 1;
              ++level;
            if (level >= 4 && level <= 8) {
              speed -= 1;
            }
            }
            else {
              gameOver = true;
            }
          }
        }
        
      }
      if (pressed == 0) {
        state = waitPress;
      }
      break;

    default:
      state = waitPress;
      break;
  }
  switch(state) {
    case waitPress:
      break;

    case buttonPressed:
      break;

    case checkAlign:
      break;

    default:
      break;
  }
  return state;
}

enum mainMenu {menuIdle, startPressed, resetPressed, startGame, loseGame, winGame};
int tickFctMenu(int state);

int tickFctMenu(int state) {
  switch(state) {
    case menuIdle:
      if (get<C>(1) == 1) {
        state = startPressed;
      }
      break;

    case startPressed:
      if (get<C>(1) == 1) {
        state = startPressed;
      }
      if (get<C>(1) == 0) {
        gamePlaying = true;
        clearScreen();
        yS = 0;
        yE = 12;
        xS = 92;
        xE = 127;
        state = startGame;
      }
      break;

    case startGame:
      if (get<C>(1) == 0) {
        state = startGame;
      }
      if (get<C>(1) == 1) {
        state = resetPressed;
      }
      if (gameOver == true) {
        gamePlaying = false;
        state = loseGame;
      }
      if (level >= 10) {
        gamePlaying = false;
        state = winGame;
      }
      break;

    case resetPressed:
      if (get<C>(1) == 1 || get<C>(0) == 1) {
        state = resetPressed;
      }
      if (get<C>(1) == 0 || get<C>(0) == 0) {
        clearScreen();
        gamePlaying = 0;
        xS = 92; 
        xE = 127;
        yS = 0;
        yE = 12;
        blockNum = 3;
        level = 1;
        prevBlockXS = 48;
        speed = 7;
        gameOver = false;
        state = menuIdle;
      }
      
      break;
    
    case loseGame:
      if (get<C>(0) == 0) {
        state = loseGame;
      }
      if (get<C>(0) == 1) {
        state = resetPressed;
      }
      break;

    case winGame:
      if (get<C>(0) == 0) {
        state = winGame;
      }
      if (get<C>(0) == 1) {
        state = resetPressed;
      }
      break;

    default:
      state = menuIdle;
      break;
  }
  switch(state) {
    case menuIdle:
      gamePlaying = false;
        testLetter(109, 118, 96, 125);
        testLetterT(100, 109, 96, 125);
        testLetterA(90, 99, 96, 125);
        testLetterC(80, 89, 96, 125);
        testLetterK(70, 79, 96, 125);
        testLetterE(60, 69, 96, 125);
        testLetterR(50, 59, 96, 125);
        testLetterI(40, 49, 96, 125);
        testLetterN(30, 39, 96, 125);
        testLetterO(20, 29, 96, 125);
      break;
    case startPressed:
      break;
    case startGame:
      drawBlock(48, 83, 0, 12);
      break;
    case resetPressed:
      break;
    case loseGame:
      testLetterG(79, 88, 86, 115);
      testLetterA(69, 78, 86, 115);
      testLetterM(59, 68, 86, 115);
      testLetterE(49, 58, 86, 115);

      testLetterO(79, 88, 61, 90);
      testLetterV(69, 78, 61, 90);
      testLetterE(59, 68, 61, 90);
      testLetterR(49, 58, 61, 90);
      break;
    case winGame: // put into while/if loop
      testConfettiBlue(109, 118, 96, 125);
      testConfettiRed(100, 109, 96, 125);
      testConfettiGreen(90, 99, 96, 125);
      testConfettiYellow(80, 89, 96, 125);
      testConfettiBlue(70, 79, 96, 125);
      testConfettiRed(60, 69, 96, 125);
      testConfettiGreen(50, 59, 96, 125);
      testConfettiYellow(40, 49, 96, 125);

      testConfettiBlue(109, 118, 76, 105);
      testConfettiRed(100, 109, 76, 105);
      testConfettiGreen(90, 99, 76, 105);
      testConfettiYellow(80, 89, 76, 105);
      testConfettiBlue(70, 79, 76, 105);
      testConfettiRed(60, 69, 76, 105);
      testConfettiGreen(50, 59, 76, 105);
      testConfettiYellow(40, 49, 76, 105);

      testConfettiBlue(109, 118, 56, 85);
      testConfettiRed(100, 109, 56, 85);
      testConfettiGreen(90, 99, 56, 85);
      testConfettiYellow(80, 89, 56, 85);
      testConfettiBlue(70, 79, 56, 85);
      testConfettiRed(60, 69, 56, 85);
      testConfettiGreen(50, 59, 56, 85);
      testConfettiYellow(40, 49, 56, 85);
      
      break;
    default:
      break;
  }
  return state;
}

typedef struct _task{
  signed char state; //Task's current state
  unsigned long period; //Task period
  unsigned long elapsedTime; //Time elapsed since last task tick
  int (*TickFct)(int); //Task tick function
} task;

task tasks[3];

int main() {
  DDRD = 0xF0;
  DDRC = 0x00;

  unsigned long moveBlockElapsedTime = 1;
  unsigned long waitPressElapsedTime = 10;
  unsigned long menuElapsedTime = 100;
  const unsigned long timerPeriod = 1;

  unsigned char i = 0;
  tasks[i].state = menuIdle;
  tasks[i].period = menuElapsedTime;
  tasks[i].elapsedTime = tasks[i].period;
  tasks[i].TickFct = &tickFctMenu;
  ++i;

  tasks[i].state = init;
  tasks[i].period = moveBlockElapsedTime;
  tasks[i].elapsedTime = tasks[i].period;
  tasks[i].TickFct = &tickFctMove;
  ++i;

  tasks[i].state = waitPress;
  tasks[i].period = waitPressElapsedTime;
  tasks[i].elapsedTime = tasks[i].period;
  tasks[i].TickFct = &tickFctCheckPress;
  ++i;
  TimerSet(1);
  TimerOn();

  SPI_INIT(); // initialize internal SPI module
  st7735_init(); // initialize the ST7735 display
  clearScreen();
  
  while(true) { 
    for (unsigned int i = 0; i < 3; i++) {
      if (tasks[i].elapsedTime == tasks[i].period) {
          tasks[i].state = tasks[i].TickFct(tasks[i].state);
          tasks[i].elapsedTime = 0;
        }
        tasks[i].elapsedTime += timerPeriod;
      }
      while (!TimerFlag);
      TimerFlag = false;

      _delay_ms(5);
  }
  return 0;
}