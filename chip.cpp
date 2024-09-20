#include <iostream>
#include <cstring>
#include <fstream>
#include <cstdint>
#include <chrono>
#include <random>
#include <SDL.h>
const unsigned int start_address = 0x200;
const unsigned int font_size = 80;
const unsigned int font_start_address = 0x50;
const unsigned int vheight = 32;
const unsigned int vwidth = 64;
//creating a shell that establishes the specifications of chip-8
class chip8 {
public:
uint8_t registers[16]{};
uint16_t memory[4096]{};
uint16_t index{};
uint16_t pc{};
uint16_t stack[16]{};
uint16_t sp{};
uint32_t video[64 * 32]{};
uint8_t keyboard[16]{};
uint8_t soundtimer{};
uint8_t delaytimer{};
uint16_t opcode;
void chip8::OP_00E0();
void chip8::OP_00EE();
void chip8::OP_2nnn();
void chip8::OP_1nnn();
void chip8::OP_3xkk();
void chip8::OP_4xkk();
void chip8::OP_5xy0();
void chip8::OP_6xkk();
void chip8::OP_7xkk();
void chip8::OP_8xy0();
void chip8::OP_8xy1();
void chip8::OP_8xy2();
void chip8::OP_8xy3();
void chip8::OP_8xy4();
void chip8::OP_8xy5();
void chip8::OP_8xy6();
void chip8::OP_8xy7();
void chip8::OP_8xyE();
void chip8::OP_9xy0();
void chip8::OP_Annn();
void chip8::OP_Bnnn();
void chip8::OP_Cxkk();
void chip8::OP_Dxyn();
void chip8::OP_Ex9E();
void chip8::OP_ExA1();
void chip8::OP_Fx07();
void chip8::OP_Fx0A();
void chip8::OP_Fx15();
void chip8::OP_Fx18();
void chip8::OP_Fx1E();
void chip8::OP_Fx29();
void chip8::OP_Fx33();
void chip8::OP_Fx55();
void chip8::OP_Fx65();
uint8_t font[font_size]{
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};
void chip8::romreader(char const* filename);
std::default_random_engine randGen;
std::uniform_int_distribution<uint8_t> randByte;
};
//the chip-8 constructer 
chip8::chip8()
: randGen(std::chrono::system_clock::now().time_since_epoch().count())
{
    pc = start_address;
    int i = 0;
    while (i < font_size)
    {
        memory[font_start_address + i] = font[i];
        i++;
    }
    // Initialize RNG
		randByte = std::uniform_int_distribution<uint8_t>(0, 255U);
}
//this function reads roms into a buffer and puts them into the memory of chip-8 starting at 0x200 
void chip8::romreader(char const* filename){
std::ifstream file(filename , std::ios::binary | std::ios::ate);

if (!file){

    std::cout << "rom couldnt be opened !\n";
    file.close();
}else {

    std::cout << "reading the rom.....\n";
    std::streampos fsize = file.tellg();
    /*the reason why we create buffers as a pointer is because we dont pot the value of the file read into the buffer 
    the buffer itself is only a pointer that points to the memory address where the actual value is that is also why we
    set the size of the buffer while doing the same thing for the file.read() because it stops a possible buffer overflow*/
    char* buffer = new char [fsize];
    file.seekg(0 , std::ios::beg);
    file.read(buffer, fsize);
    file.close();
    int i = 0;
    //putting the contents of the rom into the chip-8 memory 
    while(i<fsize){

        memory[start_address + i ] = buffer[i];
        i++;
    }
    delete[] buffer;
}
}
//opcode functions from here 
void chip8::OP_00E0(){
    std::memset(video, 0 , sizeof(video));
}
//changed the place of sp--; in this one 
void chip8::OP_00EE(){

    pc = stack[sp];
    sp--;

}
void chip8::OP_2nnn(){
    uint16_t address = (opcode & 0x0fffu);
    pc = stack[sp];
    sp++;
    pc = address;

}
void chip8::OP_1nnn(){
    uint16_t address = (opcode & 0x0fffu);
    pc = address;
}
void chip8::OP_3xkk(){
    uint8_t vx = (opcode & 0x0f00u) >> 8u;
    uint8_t kk = (opcode & 0x00ff);
    if (registers[vx] == kk){
        sp += 2;
    }


}
void chip8::OP_4xkk(){
    uint8_t vx = (opcode & 0x0f00u) >> 8u;
    uint8_t kk = (opcode &0x00ffu);
    if (registers[vx] != kk )
    {
        sp +=2;
    }
}
void chip8::OP_5xy0(){
    uint8_t vx = (0x0f00u & opcode) >> 8u;
    uint8_t vy = (opcode & 0x00f0u) >> 4u;
    if (registers[vx] == registers[vy])
    {
        sp +=2;
    }
    
}
void chip8::OP_6xkk(){
    uint8_t vx = (opcode & 0x0f00u) >> 8u;
    uint8_t kk = (opcode & 0x00ffu);
    registers[vx] = kk;

}
void chip8::OP_7xkk(){
    uint8_t vx = (opcode & 0x0f00u) >> 8u;
    uint8_t kk = (opcode & 0x00ffu);
    registers[vx] += kk;
}
void chip8::OP_8xy0(){
    uint8_t vx = (opcode & 0x0f00u) >> 8u;
    uint8_t vy = (opcode & 0x00f0u) >> 4u;
    registers[vx] = registers[vy];
}
void chip8::OP_8xy1(){
    uint8_t vx = (opcode & 0x0f00u) >> 8u;
    uint8_t vy = (opcode & 0x00f0u) >> 4u;
    registers[vx] |= registers[vy];
}
void chip8::OP_8xy2(){
    uint8_t vx = (opcode & 0x0f00u) >> 8u;
    uint8_t vy = (opcode & 0x00f0u) >> 4u;
    registers[vx] &= registers[vy];
}
void chip8::OP_8xy3(){
    uint8_t vx = (opcode & 0x0f00u) >> 8u;
    uint8_t vy = (opcode & 0x00f0u) >> 4u;
    registers[vx] ^= registers[vy];
}
void chip8::OP_8xy4(){
    uint8_t vx = (opcode & 0x0f00u) >> 8u;
    uint8_t vy = (opcode & 0x00f0u) >> 4u;
    if (registers[vx] + registers[vy] > 255)
    {
        registers[0xf] = 1;
        registers[vx] += registers[vy];
        registers[vx] &= 0xffu;
    }else {
        registers[0xf] = 0;
        registers[vx] += registers[vy];
        registers[vx] &= 0xffu;
    }
    
}
void chip8::OP_8xy5(){
    uint8_t vx = (opcode & 0x0f00u) >> 8u;
    uint8_t vy = (opcode & 0x00f0u) >> 4u;
    if (registers[vx] > registers[vy]){
        registers[0xf] = 1;
        registers[vx] -= registers[vy];
    }else{
         registers[0xf] = 0;
         registers[vx] -= registers[vy];
    }
}
void chip8::OP_8xy6(){
    uint8_t vx = (opcode & 0x0f00u) >> 8u;
    if ((registers[vx] & 0b00000001u) == 0b00000001u)
    {
        registers[0xf] = 1;
        registers[vx] = registers[vx] / 2;
    }else {
        registers[0xf] = 0;
        registers[vx] = registers[vx] / 2;
    }
    
}
void chip8::OP_8xy7(){
    uint8_t vx = (opcode & 0x0f00u) >> 8u;
    uint8_t vy = (opcode & 0x00f0u) >> 4u;
    if (registers[vy] > registers[vx])
    {
        registers[0xf] = 1;
    }else {
        registers[0xf] = 0;
    }
    registers[vx] -= registers[vy];

}
void chip8::OP_8xyE(){
    uint8_t vx = (opcode & 0x0f00u) >> 8u;
    if ((registers[vx] & 0b10000000) == 0b10000000)
    {
        registers[0xf] = 1;
    }else{
        registers[0xf] = 0;
    }
    registers[vx] <<= 1;
}
void chip8::OP_9xy0(){
    uint8_t vx = (opcode & 0x0f00u) >> 8u;
    uint8_t vy = (opcode & 0x00f0u) >> 4u;
    if (registers[vx] != registers[vy])
    {
        pc += 2;
    }
    
}
void chip8::OP_Annn(){
    uint16_t address = (opcode & 0x0fffu);
    index = address;
}
void chip8::OP_Bnnn(){
    uint16_t address = (opcode & 0x0fffu);
    pc = registers[0] + address;
}
void chip8::OP_Cxkk(){
    uint8_t vx = (opcode & 0x0f00u) >> 8u;
    uint8_t kk = (opcode & 0x00ff);
    registers[vx] = (kk & randByte(randGen));
}
//set the vwidh and vheight by myself
void chip8::OP_Dxyn(){
    uint8_t vx = (opcode & 0x0f00u) >> 8u;
    uint8_t vy = (opcode & 0x00f0u) >> 4u;
    uint8_t n = (opcode & 0x000fu);
    uint8_t x_pos = vwidth % registers[vx];
    uint8_t y_pos = vheight % registers[vy];
    registers[0xF] = 0;

	for (unsigned int row = 0; row < n; ++row)
	{
		uint8_t spriteByte = memory[index + row];

		for (unsigned int col = 0; col < 8; ++col)
		{
			uint8_t spritePixel = spriteByte & (0x80u >> col);
			uint32_t* screenPixel = &video[(y_pos + row) * vwidth + (x_pos + col)];

			// Sprite pixel is on
			if (spritePixel)
			{
				// Screen pixel also on - collision
				if (*screenPixel == 0xFFFFFFFF)
				{
					registers[0xF] = 1;
				}

				// Effectively XOR with the sprite pixel
				*screenPixel ^= 0xFFFFFFFF;
			}
		}
	}
}
//did it different in here too
void chip8::OP_Ex9E(){
    uint8_t vx = (opcode & 0x0f00u) >> 8u;
    if (keyboard[registers[vx]])
    {
        pc += 2;
    }
    
}
void chip8::OP_ExA1(){
    uint8_t vx = (opcode & 0x0f00u) >> 8u;
    if (!keyboard[registers[vx]])
    {
        pc += 2;
    }
}
void chip8::OP_Fx07(){
    uint8_t vx = (opcode & 0x0f00u) >> 8u;
    registers[vx] = delaytimer;
}
//didnt listen to chatgpt and did my own thing will rewrite if it doesnt work
void chip8::OP_Fx0A(){
    uint8_t vx = (opcode & 0x0f00u) >> 8u;
    int keyispressed = 0;
    while(!keyispressed){
        if(keyboard[0x0]){
        registers[vx] = 0;
        keyispressed = 1;
    }else if(keyboard[0x1]){
        registers[vx] = 1;
            keyispressed = 1;
    }else if(keyboard[0x2]){
        registers[vx] = 2;
        keyispressed = 1;
    }else if(keyboard[0x3]){
        registers[vx] = 3;
        keyispressed = 1;
    }else if(keyboard[0x4]){
        registers[vx] = 4;
        keyispressed = 1;
    }else if(keyboard[0x5]){
        registers[vx] = 5;
        keyispressed = 1;
    }else if(keyboard[0x6]){
        registers[vx] = 6;
        keyispressed = 1;
    }else if(keyboard[0x7]){
        registers[vx] = 7;
        keyispressed = 1;
    }else if(keyboard[0x8]){
        registers[vx] = 8;
        keyispressed = 1;
    }else if(keyboard[0x9]){
        registers[vx] = 9;
        keyispressed = 1;
    }else if(keyboard[0xA]){
        registers[vx] = 10;
        keyispressed = 1;
    }else if(keyboard[0xB]){
        registers[vx] = 11;
        keyispressed = 1;
    }else if(keyboard[0xC]){
        registers[vx] = 12;
        keyispressed = 1;
    }else if(keyboard[0xD]){
        registers[vx] = 13;
        keyispressed = 1;
    }else if(keyboard[0xE]){
        registers[vx] = 14;
        keyispressed = 1;
    }else if(keyboard[0xF]){
        registers[vx] = 15;
        keyispressed = 1;
    }
}
/*if (!keyispressed){
    pc -= 2;

}*/
}
void chip8::OP_Fx15(){
    uint8_t vx = (opcode & 0x0f00u) >> 8u;
    delaytimer = registers[vx];
}
void chip8::OP_Fx18(){
    uint8_t vx = (opcode & 0x0f00u) >> 8u;
    soundtimer = registers[vx];
}
void chip8::OP_Fx1E(){
    uint8_t vx = (opcode & 0x0f00u) >> 8u;
    index += registers[vx];
}
void chip8::OP_Fx29(){
    uint8_t vx = (opcode & 0x0f00u) >> 8u;
    uint8_t digit = registers[vx];
    index = font_start_address + (5 * digit);
}
void chip8::OP_Fx33(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t value = registers[Vx];
	// Ones-place
	memory[index + 2] = value % 10;
	value /= 10;
	// Tens-place
	memory[index + 1] = value % 10;
	value /= 10;
	// Hundreds-place
	memory[index] = value % 10;
}
void chip8::OP_Fx55(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    int i = 0;
    while(i <= Vx){
        memory[index + i] = registers[i];
        i++;
    }

}
void chip8::OP_Fx65(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    int i = 0;
    while (i <= Vx)
    {
        registers[i] = memory[index + i];
        i++;
    }
    
}
// instructions finished after 3 days at 3 am 20th of december a day before deparing for uni
