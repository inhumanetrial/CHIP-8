#ifndef CHIP_8_H
#define CHIP_8_H

#include <stdint.h>

class Chip8 {
private:
    uint8_t memory[4096];
    uint8_t V[16];
    
    uint16_t stack[16];
    uint8_t sp;

    uint16_t pc;
    uint16_t opcode;
    uint16_t I;

    uint8_t delayTimer;
    uint8_t soundTimer;
    
    bool waitingForKey = false;
    uint8_t waitingRegister = 0;


    void init();

public:
    uint32_t  gfx[64 * 32];
    uint8_t  key[16];
    bool drawFlag;
    Chip8();
    ~Chip8();
    void updateTimers();
    void cycle();
    bool load(const char *file_path);
    bool isWaitingForKey() const;
    void onKeyPress(uint8_t key);
};

#endif