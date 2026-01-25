#include "chip8.h"
#include <fstream>
#include <cstring>

bool Chip8::load(const char* file_path) {
    std::ifstream file(file_path, std::ios::binary | std::ios::ate);
    if (!file)
        return false;

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    // chip8 programs start at 0x200
    if (size > (4096 - 0x200))
        return false;

    file.read(reinterpret_cast<char*>(&memory[0x200]), size);
    return true;
}
void Chip8::updateTimers() {
    if (delayTimer > 0)
        --delayTimer;
    if (soundTimer > 0)
        --soundTimer;
}


Chip8::~Chip8() = default;

Chip8::Chip8() {
    pc = 0x200;
    opcode = 0;
    I = 0;
    sp = 0;

    std::memset(memory, 0, sizeof(memory));
    std::memset(V, 0, sizeof(V));
    std::memset(stack, 0, sizeof(stack));
    std::memset(gfx, 0, sizeof(gfx));
    std::memset(key, 0, sizeof(key));

    delayTimer = 0;
    soundTimer = 0;
    
    constexpr size_t FONTSET_SIZE = 80;


static const uint8_t fontset[FONTSET_SIZE] =
{
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
    for(int i = 0; i < 80; i++){
    memory[0x50 + i] = fontset[i];
    }
}
    bool Chip8::isWaitingForKey() const {
    return waitingForKey;
}
void Chip8::onKeyPress(uint8_t key) {
    if (waitingForKey) {
        V[waitingRegister] = key;
        waitingForKey = false;
    }
}

    void Chip8::cycle() {

        if (waitingForKey) {
        return;
    }

    //  FETCH 
    opcode = (memory[pc] << 8) | memory[pc + 1];


    //  DECODE & EXECUTE 
    switch (opcode & 0xF000) {
        case 0x0000: //00E_
            switch (opcode & 0x00FF) {
                case 0x00E0: // CLS
                std::memset(gfx, 0, sizeof(gfx));
                drawFlag = true;
                pc += 2;
                break;

             case 0x00EE: // RET
              --sp;
              pc = stack[sp];
      //        pc += 2;
              break;
        default:
            break;
    }
    break;

        case 0x1000: { // JP addr
            pc = opcode & 0x0FFF;
            break;
        }
        case 0x2000: { // CALL addr
            stack[sp] = pc + 2;
            ++sp;
            pc = opcode & 0x0FFF;
            break;
        }
        case 0x3000: { // SE Vx, byte
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t value = opcode & 0x00FF;
            if (V[x] == value) {
                pc += 4;
            }
            else pc += 2;
            break;
        }
        case 0x4000:{ // SNE Vx, byte
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t value = opcode & 0x00FF;
            if (V[x]!=value){
                pc+=4;
            }
            else pc+=2;
            break;
        }
        case 0x5000: { // SE Vx, Vy
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t y = (opcode & 0x00F0) >> 4;
            if (V[x] == V[y]) {
                pc += 4;
            }
            else pc+=2;
            break;
        }

        case 0x6000: { // LD Vx, byte
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t value = opcode & 0x00FF;
            V[x] = value;
            pc += 2;
            break;
        }

        case 0x7000: { // ADD Vx, byte
            uint8_t x = (opcode & 0x0F00) >> 8;
            uint8_t value = opcode & 0x00FF;
            V[x] += value;
            pc += 2;
            break;
        }
        case 0x8000: { //8XY_
            uint8_t x = (opcode & 0x0F00) >> 8;
             uint8_t y = (opcode & 0x00F0) >> 4;

    switch (opcode & 0x000F) {
        case 0x0:
         V[x] = V[y];
         pc += 2;
         break;
        case 0x1:
            V[x] |= V[y];
            pc += 2;
            break;
        case 0x2:
            V[x] &= V[y];
            pc += 2;
            break;
        case 0x3:
            V[x] ^= V[y];
            pc += 2;
            break;
        case 0x4: {
            uint16_t sum = V[x] + V[y];
            V[0xF] = (sum > 255) ? 1 : 0;
            V[x] = sum & 0xFF;
            pc += 2;
            break;
        }
        case 0x5:
            V[0xF] = (V[x] > V[y]) ? 1 : 0;
            V[x] -= V[y];
            pc += 2;
            break;
        case 0x6:
            V[0xF] = V[x] & 0x1;
            V[x] >>= 1;
            pc += 2;
            break;
        case 0x7:
            V[0xF] = (V[y] > V[x]) ? 1 : 0;
            V[x] = V[y] - V[x];
            pc += 2;
            break;
        case 0xE:
            V[0xF] = (V[x] & 0x80) >> 7;
            V[x] <<= 1;
            pc += 2;
            break;
        default:
            break; 
    }
        break;
    }
    case 0x9000: { // SNE Vx, Vy
    if ((opcode & 0x000F) == 0) {
        uint8_t x = (opcode & 0x0F00) >> 8;
        uint8_t y = (opcode & 0x00F0) >> 4;

        if (V[x] != V[y])
            pc += 4;
        else
            pc += 2;
    }
    break;
}

    case 0xA000: { // LD I, addr
        I = opcode & 0x0FFF;
        pc += 2;
        break;
    }
    case 0xB000: { // JP V0, addr
        pc = (opcode & 0x0FFF) + V[0];
        break;
    }
    
    case 0xC000: { // RND Vx, byte
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t kk = opcode & 0x00FF;
    V[x] = (rand() % 256) & kk;
    pc += 2;
    break;
}

    case 0xD000: { // DRW Vx, Vy, nibble
    uint8_t x = V[(opcode & 0x0F00) >> 8];
    uint8_t y = V[(opcode & 0x00F0) >> 4];
    uint8_t height = opcode & 0x000F;

    V[0xF] = 0;
    for (int row = 0; row < height; ++row) {
        uint8_t spriteByte = memory[I + row];

        for (int col = 0; col < 8; ++col) {
            if ((spriteByte & (0x80 >> col)) != 0) {

                int px = (x + col) % 64;
                int py = (y + row) % 32;
                int index = py * 64 + px;

                if (gfx[index] == 1)
                    V[0xF] = 1;

                gfx[index] ^= 1;
            }
        }
    }
    drawFlag = true;
    pc += 2;
    break;
}
    case 0xE000: {
        uint8_t x = (opcode & 0x0F00) >> 8;
        switch (opcode & 0x00FF) {
            case 0x9E://SKP Vx
                if (key[V[x]] != 0) {
                    pc += 4;
                }
                else pc += 2;
                break;
            case 0xA1://SKNP Vx
                if (key[V[x]] == 0) {
                    pc += 4;
                }
                else pc += 2;
                break;
        }
        break;
    }

    case 0xF000: {
        uint8_t x = (opcode & 0x0F00) >> 8;
        switch (opcode & 0x00FF) {
            case 0x07: // LD Vx, DT
            V[x] = delayTimer;
            pc += 2;
            break;

            case 0x15: // LD DT, Vx
            delayTimer = V[x];
            pc += 2;
             break;

            case 0x18: // LD ST, Vx
            soundTimer = V[x];
            pc += 2;
               break;

            case 0x33: //BCD
             memory[I]     = V[x] / 100;
             memory[I + 1] = (V[x] / 10) % 10;
             memory[I + 2] = V[x] % 10;
                pc += 2;
             break;
             
             case 0x55: // LD [I], V0..Vx
             for (int i = 0; i <= x; ++i) {
             memory[I + i] = V[i];
             }
             pc += 2;
             break;

            case 0x65: // LD V0..Vx, [I]
                 for (int i = 0; i <= x; ++i) {
                     V[i] = memory[I + i];
                    }
                    pc += 2;
             break;


            case 0x0A: { // FX0A LD Vx, K
                waitingForKey = true;
                waitingRegister = x;
               // pc -= 2;
                break;
            }
            case 0x1E: // ADD I, Vx
             I += V[x];
             pc += 2;
             break;

            case 0x29:
                I = V[x] * 5 + 0x50;
                pc += 2;
                break;
    }


}
    }
    //  TIMERS 
   // if (delayTimer > 0) --delayTimer;
   // if (soundTimer > 0) --soundTimer;


    }