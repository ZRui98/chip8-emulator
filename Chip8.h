#ifndef _CHIP_8_INCLUDED_
#define _CHIP_8_INCLUDED_
#include <stdint.h>
class Chip8 {
	public :
		uint8_t memory[4096];
		uint8_t V[16];
		uint16_t pc;
		uint16_t I;
		uint8_t sp;
		uint16_t stack[16];
		uint8_t display[64 * 32];
		unsigned int dt;
		unsigned int st;
		void init();
		void loadRom(const char *path);
		void emulateCycle();
		bool keys[0xF];
		bool reDraw;
};
#endif
