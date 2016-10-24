#include "Chip8.h"
#include <fstream>
unsigned char memory_font[80] ={
	0xF0,0x90,0x90,0x90,0xF0,//0
	0x20,0x60,0x20,0x20,0x70,//1
	0xF0,0x10,0xF0,0x80,0xF0,//2
	0xF0,0x10,0xF0,0x10,0xF0,//3
	0x90,0x90,0xF0,0x10,0x10,//4
	0xF0,0x80,0xF0,0x10,0xF0,//5
	0xF0,0x80,0xF0,0x90,0xF0,//6
	0xF0,0x10,0x20,0x40,0x40,//7
	0xF0,0x90,0xF0,0x90,0xF0,//8
	0xF0,0x90,0xF0,0x10,0xF0,//9
	0xF0,0x90,0xF0,0x90,0x90,//A
	0xE0,0x90,0xE0,0x90,0xE0,//B
	0xF0,0x80,0x80,0x80,0xF0,//C
	0xE0,0x90,0x90,0x90,0xE0,//D
	0xF0,0x80,0xF0,0x80,0xF0,//E
	0xF0,0x80,0xF0,0x80,0x80,//F
};


void Chip8::init()
{
	for (int i = 0; i < 80; i++)
		memory[i] = memory_font[i];
	for (int i = 80; i < 4096; i++)
		memory[i] = 0;
	for (int i = 0; i < 16; i++)
		stack[i] = 0;
	for (int i = 0; i < 16; i++)
		V[i] = 0;
	for (int i = 0; i < 16; i++)
		keys[i] = 0;
	for (int i = 0; i < 64 * 32; i++)
		display[i] = 0;
	pc = 0x200;
	reDraw = false;
	I = 0;
	sp = 0;
	dt = 0;
	st = 0;
}

void Chip8::loadRom(const char *path)
{
	init();
	
	std::ifstream input(path, std::ios::binary);
	std::streampos fsize = 0;
	fsize = input.tellg();
	input.seekg(0, std::ios::end);
	fsize = input.tellg() - fsize;
	
	if (fsize<3584)//4096-0x200 
	{
		input.seekg(0, std::ios::beg);
		char * buffer = new char[(int)fsize];
		input.read(buffer, fsize);
		for (int i = 0; i < fsize; i++) {
			memory[i + 0x200] = (uint8_t)(buffer[i]);
		}
		free(buffer);
	}
	input.close();
}

void Chip8::emulateCycle() {
	unsigned short opcode = memory[pc] << 8 | memory[pc + 1];

	switch (opcode & 0xF000) {
		case(0x0000): {
			switch (opcode & 0xF) {
				case(0x0): {
					for (int i = 0; i < 64 * 32; i++)
						display[i] = 0;
					reDraw = true;
					pc += 2;
					break;
				}
				case(0xE): {
					pc = stack[--sp%16];
					pc += 2;
					break;
				}
				default:{
					printf("%04X\n", opcode);
					break;
				}
			}
			break;
		}
		case(0x1000): {
			pc = (opcode & 0x0FFF);
			break;
		}
		case(0x2000): {
			stack[sp++%16] = pc;
			pc = (opcode & 0x0FFF);
			break;
		}
		case(0x3000): {
			if (V[(opcode & 0xF00) >> 8] == (opcode & 0xFF))
				pc += 2;
			pc += 2;
			break;
		}
		case(0x4000): {
			if (V[(opcode & 0xF00) >> 8] != (opcode & 0x0FF))
				pc += 2;
			pc += 2;
			break;
		}
		case(0x5000): {
			if (V[(opcode & 0xF00) >> 8] == V[(opcode & 0xF0) >> 4])
				pc += 2;
			pc += 2;
			break;
		}
		case(0x6000):{
			V[(opcode & 0xF00) >> 8] = (opcode & 0x0FF);
			pc += 2;
			break;
		}
		case(0x7000): {
			V[(opcode & 0xF00) >> 8] += (opcode & 0x0FF);
			pc += 2;
			break;
		}
		case(0x8000): {
			switch (opcode & 0xF) {
				case(0x0): {//set x = y
					V[(opcode & 0xF00) >> 8] = V[(opcode & 0xF0) >> 4];
					pc += 2;
					break;
				}
				case(0x1): {//set x = x|y
					V[(opcode & 0xF00) >> 8] = V[(opcode & 0xF00) >> 8] | V[(opcode & 0xF0) >> 4];
					pc += 2;
					break;
				}
				case(0x2): {//set x = x&y
					V[(opcode & 0xF00) >> 8] = V[(opcode & 0xF00) >> 8] & V[(opcode & 0xF0) >> 4];
					pc += 2;
					break;
				}
				case(0x3): {//set x = x^y
					V[(opcode & 0xF00) >> 8] = V[(opcode & 0xF00) >> 8] ^ V[(opcode & 0xF0) >> 4];
					pc += 2;
					break;
				}
				case(0x4): {
					if (V[(opcode & 0xF00) >> 8]  > (0xFF-V[(opcode & 0xF0) >> 4])) {
						V[0xF] = 1;
					}
					else {
						V[0xF] = 0;
					}
					V[(opcode & 0xF00) >> 8] += V[(opcode & 0xF0) >> 4];
					pc += 2;
					break;
				}
				case(0x5): {
					if (V[(opcode & 0xF00) >> 8] > V[(opcode & 0xF0) >> 4])
						V[0xF] = 1;
					else
						V[0xF] = 0;
					V[(opcode & 0xF00) >> 8] -= V[(opcode & 0xF0) >> 4];
					pc += 2;
					break;
				}
				case(0x6): {
					V[0xF] = V[(opcode & 0xF00) >> 8] & 0x1;
					V[(opcode & 0xF00) >> 8] = V[(opcode & 0xF00) >> 8] >> 1;
					pc += 2;
					break;
				}
				case(0x7): {
					if (V[(opcode & 0xF0) >> 4] > V[(opcode & 0xF00) >> 8])
						V[0xF] = 1;
					else
						V[0xF] = 0;
					V[(opcode & 0xF00) >> 8] = V[(opcode & 0xF0) >> 4] - V[(opcode & 0xF00) >> 8];
					pc += 2;
					break;
				}
				case(0xE): {
					V[0xF] = V[(opcode & 0xF00) >> 8] >> 7;
					V[(opcode & 0xF00) >> 8] = V[(opcode & 0xF00) >> 8] << 1;
					pc += 2;
					break;
				}
				default: {
					printf("%04X\n", opcode);
					break;
				}
			}
			break;
		}
		case(0x9000): {
			if (V[(opcode & 0xF00) >> 8] != V[(opcode & 0xF0) >> 4])
				pc += 2;
			pc += 2;
			break;
		}
		case(0xA000): {
			I = (opcode & 0x0FFF);
			pc += 2;
			break;
		}
		case(0xB000): {
			pc = V[0] + (opcode & 0x0FFF);
			break;
		}
		case(0xC000): {
			V[(opcode & 0xF00) >> 8] = (rand() % 256) & (opcode & 0xFF);
			pc += 2;
			break;
		}
		case(0xD000): {
			unsigned char x = V[(opcode & 0xF00)>>8];
			unsigned char y = V[(opcode & 0xF0)>>4];
			unsigned char n = (opcode & 0xF);
			unsigned short pixelRow=0;
			V[0xF] = 0;
			for (int i = 0; i < n; i++) {
				pixelRow = memory[I + i];
				for (int j = 0; j < 8; j++) {
					if ((pixelRow & (0x80 >> j)) != 0) {
						if ((y + i) >= 0 && (y + i) < 32 && (x + j) >= 0 && (x + j) < 64) {
							if (display[((y + i) * 64) + x + j] == 1)
								V[0xF] = 1;
							display[((y + i) * 64) + x + j] ^= 1;
						}
					}
				}
			}
			reDraw = true;
			pc += 2;
			break;
		}
		case(0xE000): {
			switch (opcode & 0xFF) {
				case(0x9E): {
					if (keys[V[(opcode & 0xF00)>>8]] == 1) {
						pc += 2;
					}
					pc += 2;
					break;
				}
				case(0xA1): {
					if (keys[V[(opcode & 0xF00)>>8]] == 0) {
						pc += 2;
					}
					pc += 2;
					break;
				}		
				default:{
					printf("%04X\n", opcode);
					break;
				}
			}
			break;
		}
		case(0xF000): {
			switch (opcode & 0xFF) {
				case(0x07): {
					V[(opcode & 0xF00) >> 8] = dt;
					pc += 2;
					break;
				}
				case(0x0A): {
					int keyPressed = 0;
					for (int i = 0; i < 16; i++) {
						if (keys[i] != 0) {
							keyPressed = i;
							break;
						}
					}
					if (keyPressed == 0)
						return;
					V[(opcode & 0xF00) >> 8] = keyPressed;
					pc += 2;
					break;
				}
				case(0x15): {
					dt = V[(opcode & 0xF00) >> 8];
					pc += 2;
					break;
				}
				case(0x18): {
					st = V[(opcode & 0xF00) >> 8];
					pc += 2;
					break;
				}
				case(0x1E): {
					I += V[(opcode & 0xF00) >> 8];
					pc += 2;
					break;
				}
				case(0x29): {
					I = V[(opcode & 0xF00) >> 8] * 0x5;
					pc += 2;
					break;
				}
				case(0x33): {
					memory[I] = V[(opcode & 0xF00) >> 8] / 100;
					memory[I + 1] = (V[(opcode & 0xF00) >> 8] / 10) % 10;
					memory[I + 2] = (V[(opcode & 0xF00) >> 8]) % 10;
					pc += 2;
					break;
				}
				case(0x55): {
					for (int i = 0; i <= ((opcode & 0xF00) >> 8); i++)
						memory[I++] = V[i];
					pc += 2;
					break;
				}
				case(0x65): {
					for (int i = 0; i <= ((opcode & 0xF00) >> 8); i++)
						V[i] = memory[I++];
					pc += 2;
					break;
				}
				default: {
					printf("%04X\n", opcode);
					break;
				}
			}
			break;
		}
		default: {
			printf("%04X\n", opcode);
			break;
		}
	}

	if (dt > 0)
		dt--;
	if (st > 0) {
		//Play a sound
		st--;
	}
}