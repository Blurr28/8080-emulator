#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "disasm.h"

typedef struct ConditionCodes{
    uint8_t     z:1;
    uint8_t     p:1;
    uint8_t     s:1;
    uint8_t     cy:1;
    uint8_t     ac:1;
    uint8_t     pad:3;
}ConditionCodes;

typedef struct State8080{
    uint8_t     a;
    uint8_t     b;
    uint8_t     c;
    uint8_t     d;
    uint8_t     e;
    uint8_t     h;
    uint8_t     l;
    uint16_t     sp;
    uint16_t     pc;
    uint8_t     *memory;
    struct  ConditionCodes   cc;
    uint8_t     int_enable;
}State8080;

int Parity(int x, int size){
    int p=0;
    x = (x & ((1<<size)-1));
    for (int i=0; i < size; i++ ){
        if (x& 0x1) p++;
        x = (x>>1); 
    }
    return (0 == (p & 0x1));
}

void LogicFlags(State8080 *state)
{
    state->cc.cy = state->cc.ac = 0;
    state->cc.z = (state->a ==0);
    state->cc.s = (0x80 == (0x80 & state->a));
    state->cc.p = Parity(state->a, 8);
}

void ArithFlags(State8080 *state, uint16_t res)
{
    state->cc.cy = (res > 0xff);
    state->cc.z = ((res&0xff) ==0);
    state->cc.s = (0x80 == (res & 0x80));
    state->cc.p = Parity(res&0xff, 8);
}

void UnimplementedInstructions(State8080* state){
    printf("\nerror: Instruction Not Implemented.\n");
    exit(1);
}

int Emulate8080(State8080* state){
    unsigned char *opcode = &state->memory[state->pc];
    //print the next instruction to be executed
    Disassembler(state->memory, state->pc);
    state->pc +=1;
    switch (*opcode){
        case 0x00:  break;
        case 0x01:  state->c = opcode[1];
                    state->b = opcode[2];
                    state->pc +=2;
                    break;
        case 0x02:  UnimplementedInstructions(state); break;
        case 0x03:  UnimplementedInstructions(state); break;
        case 0x04:  UnimplementedInstructions(state); break;
        case 0x05:  //DCR B
                    {
                    uint8_t x = state->b -1;
                    state->cc.z = (0== x);
                    state->cc.s = (0x80 == (x & 0x80));
                    state->cc.p = Parity(x, 8);
                    state->b = x;
                    }
                    break;
        case 0x06:  //MVI B
                    {
                    state->b = opcode[1];
                    state->pc += 1;
                    break;
                    }
        case 0x07:  UnimplementedInstructions(state); break;
        case 0x08:  UnimplementedInstructions(state); break;
        case 0x09:  //DAD B
                    {
                    uint32_t hl = (state->h << 8) | state->l;
                    uint32_t bc = (state->b << 8) | state->c;
                    uint32_t res = hl + bc;
                    state->h = (res & 0xff00 ) >> 8;
                    state->l = (res & 0xff);
                    state->cc.cy = ((res & 0xffff0000) != 0);
                    }
                    break;
        case 0x0a:  UnimplementedInstructions(state); break;
        case 0x0b:  UnimplementedInstructions(state); break;
        case 0x0c:  UnimplementedInstructions(state); break;
        case 0x0d:  //DCR C
                    {
                    uint8_t x = state->c -1;
                    state->cc.z = (x == 0);
                    state->cc.s = (0x80 == (x & 0x80));
                    state->cc.p = Parity(x, 8);
                    state->c = x;
                    }
                    break;
        case 0x0e:  //MVI C
                    {
                    state->c = opcode[1];
                    state->pc+=1;
                    }
                    break;
        case 0x0f:  //RRC
                    {
                    uint8_t x = state->a;
                    state->a = ((x & 1) << 7) | (x >> 1);
                    state->cc.cy = (1 == (x&1));
                    }
                    break;
        case 0x10:  UnimplementedInstructions(state); break;
        case 0x11:  //LXI D
                    {
                    state->d = opcode[2];
                    state->e = opcode[1];
                    state->pc += 2;
                    break;
                    }
        case 0x12:  UnimplementedInstructions(state); break;
        case 0x13:  //INX D
                    {
                    state->e++;
                    if (state->e ==0) state->d++;
                    break;
                    }
        case 0x14:  UnimplementedInstructions(state); break;
        case 0x15:  UnimplementedInstructions(state); break;
        case 0x16:  UnimplementedInstructions(state); break;
        case 0x17:  UnimplementedInstructions(state); break;
        case 0x18:  UnimplementedInstructions(state); break;
        case 0x19:  //DAD D
                    {
                    uint32_t hl = (state->h << 8) | state->l;
                    uint32_t de = (state->d << 8) | state->e;
                    uint32_t res = hl + de;
                    state->h = (res & 0xff00 ) >> 8;
                    state->l = (res & 0xff);
                    state->cc.cy = ((res & 0xffff0000) != 0);
                    }
                    break;
        case 0x1a:  //LDAX D
                    {
                    uint16_t x = (state->d <<8) | state->e; 
                    state->a = state->memory[x];
                    break;
                    }
        case 0x1b:  UnimplementedInstructions(state); break;
        case 0x1c:  UnimplementedInstructions(state); break;
        case 0x1d:  UnimplementedInstructions(state); break;
        case 0x1e:  UnimplementedInstructions(state); break;
        case 0x1f:  //RAR
                    {
                    uint8_t x = state->a;
                    state->a = (state->cc.cy << 7) | (x >> 1);
                    state->cc.cy = (1 == 1&x);
                    }
                    break;
        case 0x20:  UnimplementedInstructions(state); break;
        case 0x21:  //LXI H
                    {
                    state->l = opcode[1];
                    state->h = opcode[2];
                    state->pc += 2;
                    break;
                    }
        case 0x22:  UnimplementedInstructions(state); break;
        case 0x23:  //INX H
                    {
                    state->l++;
                    if (state->l ==0) state->h++;
                    break;
                    }
        case 0x24:  UnimplementedInstructions(state); break;
        case 0x25:  UnimplementedInstructions(state); break;
        case 0x26:  //MVI H
                    {
                    state->h = opcode[1];
                    state->pc += 1;
                    }
                    break;
        case 0x27:  UnimplementedInstructions(state); break;
        case 0x28:  UnimplementedInstructions(state); break;
        case 0x29:  //DAD H
                    {
                    uint32_t x = (state->h << 8) | (state->l);
                    uint32_t hl = x +x;
                    state->h = (hl & 0xff00) >> 8;
                    state->l = hl & 0xff;
                    state->cc.cy = ((hl & 0xffff0000) != 0);
                    }
                    break;
        case 0x2a:  UnimplementedInstructions(state); break;
        case 0x2b:  UnimplementedInstructions(state); break;
        case 0x2c:  UnimplementedInstructions(state); break;
        case 0x2d:  UnimplementedInstructions(state); break;
        case 0x2e:  UnimplementedInstructions(state); break;
        case 0x2f:  //CMA
                    {
                    state->a = ~state->a;
                    break;
                    }
        case 0x30:  UnimplementedInstructions(state); break;
        case 0x31:  //LXI SP
                    {
                    state->sp = (opcode[2] << 8) | opcode[1];
                    state->pc += 2;
                    break;
                    }
        case 0x32:  //STA adr
                    {
                    uint16_t adr = (opcode[2]<<8) | opcode[1];
                    state->memory[adr] = state->a;
                    state->pc +=2;
                    }
                    break;
        case 0x33:  UnimplementedInstructions(state); break;
        case 0x34:  UnimplementedInstructions(state); break;
        case 0x35:  UnimplementedInstructions(state); break;
        case 0x36:  //MVI M
                    {
                    uint16_t x = (state->h << 8) | state-> l;
                    state->memory[x] = opcode[1];
                    state->pc++;
                    }
                    break;
        case 0x37:  UnimplementedInstructions(state); break;
        case 0x38:  UnimplementedInstructions(state); break;
        case 0x39:  UnimplementedInstructions(state); break;
        case 0x3a:  //LDA adr
                    {
                    uint16_t x = (opcode[2] <<8) | opcode[1];
                    state->a = state->memory[x];
                    state->pc +=2;
                    }
                    break;
        case 0x3b:  UnimplementedInstructions(state); break;
        case 0x3c:  UnimplementedInstructions(state); break;
        case 0x3d:  UnimplementedInstructions(state); break;
        case 0x3e:  //MVI A
                    {
                    state->a = opcode[1];
                    state->pc += 1;
                    }
                    break;
        case 0x3f:  UnimplementedInstructions(state); break;
        case 0x40:  UnimplementedInstructions(state); break;
        case 0x41:  state->b = state->c; break;
        case 0x42:  state->b = state->d; break;
        case 0x43:  state->b = state->e; break;
        case 0x44:  UnimplementedInstructions(state); break;
        case 0x45:  UnimplementedInstructions(state); break;
        case 0x46:  UnimplementedInstructions(state); break;
        case 0x47:  UnimplementedInstructions(state); break;
        case 0x48:  UnimplementedInstructions(state); break;
        case 0x49:  UnimplementedInstructions(state); break;
        case 0x4a:  UnimplementedInstructions(state); break;
        case 0x4b:  UnimplementedInstructions(state); break;
        case 0x4c:  UnimplementedInstructions(state); break;
        case 0x4d:  UnimplementedInstructions(state); break;
        case 0x4e:  UnimplementedInstructions(state); break;
        case 0x4f:  UnimplementedInstructions(state); break;
        case 0x50:  UnimplementedInstructions(state); break;
        case 0x51:  UnimplementedInstructions(state); break;
        case 0x52:  UnimplementedInstructions(state); break;
        case 0x53:  UnimplementedInstructions(state); break;
        case 0x54:  UnimplementedInstructions(state); break;
        case 0x55:  UnimplementedInstructions(state); break;
        case 0x56:  //MOV D,M
                    {
                    uint16_t x = (state->h << 8) | state->l;
                    state->d = state->memory[x];
                    break;
                    }
        case 0x57:  UnimplementedInstructions(state); break;
        case 0x58:  UnimplementedInstructions(state); break;
        case 0x59:  UnimplementedInstructions(state); break;
        case 0x5a:  UnimplementedInstructions(state); break;
        case 0x5b:  UnimplementedInstructions(state); break;
        case 0x5c:  UnimplementedInstructions(state); break;
        case 0x5d:  UnimplementedInstructions(state); break;
        case 0x5e:  //MOV E,M
                    {
                    uint16_t x = (state->h << 8) | state->l;
                    state->e = state->memory[x];
                    break;
                    }
        case 0x5f:  UnimplementedInstructions(state); break;
        case 0x60:  UnimplementedInstructions(state); break;
        case 0x61:  UnimplementedInstructions(state); break;
        case 0x62:  UnimplementedInstructions(state); break;
        case 0x63:  UnimplementedInstructions(state); break;
        case 0x64:  UnimplementedInstructions(state); break;
        case 0x65:  UnimplementedInstructions(state); break;
        case 0x66:  //MOV H,M
                    {
                    uint16_t x = (state->h << 8) | state->l;
                    state->h = state->memory[x];
                    break;
                    }
        case 0x67:  UnimplementedInstructions(state); break;
        case 0x68:  UnimplementedInstructions(state); break;
        case 0x69:  UnimplementedInstructions(state); break;
        case 0x6a:  UnimplementedInstructions(state); break;
        case 0x6b:  UnimplementedInstructions(state); break;
        case 0x6c:  UnimplementedInstructions(state); break;
        case 0x6d:  UnimplementedInstructions(state); break;
        case 0x6e:  UnimplementedInstructions(state); break;
        case 0x6f:  //MOV LA
                    {
                    state->l = state->a;
                    }
                    break;
        case 0x70:  UnimplementedInstructions(state); break;
        case 0x71:  UnimplementedInstructions(state); break;
        case 0x72:  UnimplementedInstructions(state); break;
        case 0x73:  UnimplementedInstructions(state); break;
        case 0x74:  UnimplementedInstructions(state); break;
        case 0x75:  UnimplementedInstructions(state); break;
        case 0x76:  UnimplementedInstructions(state); break;
        case 0x77:  //MOV M,A
                    {
                    uint16_t x = (state->h << 8) | state->l;
                    state->memory[x] = state->a;
                    }
                    break;
        case 0x78:  UnimplementedInstructions(state); break;
        case 0x79:  UnimplementedInstructions(state); break;
        case 0x7a:  //MOV A,D
                    {
                    state->a = state->d;
                    }
                    break;
        case 0x7b:  //MOV A,E
                    {
                    state->a = state->e;
                    }
                    break;
        case 0x7c:  //MOV A,H
                    {
                    state->a = state->h;
                    break;
                    }
                    break;
        case 0x7d:  UnimplementedInstructions(state); break;
        case 0x7e:  //MOV A,M
                    {
                    uint16_t x = (state->h << 8) | state->l;
                    state->a = state->memory[x];
                    break;
                    }
        case 0x7f:  UnimplementedInstructions(state); break;
        case 0x80:  //ADD B
                {
                    uint16_t answer = (uint16_t) state->a + (uint16_t) state->b;
                    //zero flag
                    if ((answer & 0xff)==0)
                        state->cc.z =1;
                    else
                        state->cc.z=0;
                    //sign flag
                    if ((answer & 0x80) ==0)
                        state->cc.s=1;
                    else
                        state->cc.s=0;
                    //carry flag
                    if (answer > 0xff)
                        state->cc.cy =1;
                    else
                        state->cc.cy=0;
                    
                    //parity flag
                    state->cc.p = Parity(answer, 8);

                    //accumulator
                    state->a = (answer & 0xff);
                }
                break;
        case 0x81:  //Add C
                    { 
                    uint16_t answer = (uint16_t) state->a + (uint16_t) state->c;
                    //zero flag
                    state->cc.z = (0 == (answer & 0xff));
                    //sign flag
                    state->cc.s = (0 != (answer & 0x80));
                    //carry flag
                    state->cc.cy = (answer> 0xff);
                    //parity flag
                    state->cc.p = Parity(answer, 8);
                    //accumulator
                    state->a = answer;
                    }
                    break;
        case 0x82:  UnimplementedInstructions(state); break;
        case 0x83:  UnimplementedInstructions(state); break;
        case 0x84:  UnimplementedInstructions(state); break;
        case 0x85:  UnimplementedInstructions(state); break;
        case 0x86:  //ADD M
                    {
                        uint16_t offset = (state->h << 8) | (state-> l);
                        uint16_t answer = (uint16_t) state->a + (uint16_t) state->memory[offset];
                        state->cc.z = (0 == (answer & 0xff));
                        state->cc.s = (0 != (answer & 0x80));
                        state->cc.p = Parity(answer, 8);
                        state->a = answer;
                    }
                    break;
        case 0x87:  UnimplementedInstructions(state); break;
        case 0x88:  UnimplementedInstructions(state); break;
        case 0x89:  UnimplementedInstructions(state); break;
        case 0x8a:  UnimplementedInstructions(state); break;
        case 0x8b:  UnimplementedInstructions(state); break;
        case 0x8c:  UnimplementedInstructions(state); break;
        case 0x8d:  UnimplementedInstructions(state); break;
        case 0x8e:  UnimplementedInstructions(state); break;
        case 0x8f:  UnimplementedInstructions(state); break;
        case 0x90:  UnimplementedInstructions(state); break;
        case 0x91:  UnimplementedInstructions(state); break;
        case 0x92:  UnimplementedInstructions(state); break;
        case 0x93:  UnimplementedInstructions(state); break;
        case 0x94:  UnimplementedInstructions(state); break;
        case 0x95:  UnimplementedInstructions(state); break;
        case 0x96:  UnimplementedInstructions(state); break;
        case 0x97:  UnimplementedInstructions(state); break;
        case 0x98:  UnimplementedInstructions(state); break;
        case 0x99:  UnimplementedInstructions(state); break;
        case 0x9a:  UnimplementedInstructions(state); break;
        case 0x9b:  UnimplementedInstructions(state); break;
        case 0x9c:  UnimplementedInstructions(state); break;
        case 0x9d:  UnimplementedInstructions(state); break;
        case 0x9e:  UnimplementedInstructions(state); break;
        case 0x9f:  UnimplementedInstructions(state); break;
        case 0xa0:  UnimplementedInstructions(state); break;
        case 0xa1:  UnimplementedInstructions(state); break;
        case 0xa2:  UnimplementedInstructions(state); break;
        case 0xa3:  UnimplementedInstructions(state); break;
        case 0xa4:  UnimplementedInstructions(state); break;
        case 0xa5:  UnimplementedInstructions(state); break;
        case 0xa6:  UnimplementedInstructions(state); break;
        case 0xa7:  // ANA A
                    {
                    state->a = state->a & state->a;
                    LogicFlags(state);
                    break;
                    }
        case 0xa8:  UnimplementedInstructions(state); break;
        case 0xa9:  UnimplementedInstructions(state); break;
        case 0xaa:  UnimplementedInstructions(state); break;
        case 0xab:  UnimplementedInstructions(state); break;
        case 0xac:  UnimplementedInstructions(state); break;
        case 0xad:  UnimplementedInstructions(state); break;
        case 0xae:  UnimplementedInstructions(state); break;
        case 0xaf:  //XRA A
                    {
                    state->a = state->a^state->a;
                    LogicFlags(state);
                    }
                    break;
        case 0xb0:  UnimplementedInstructions(state); break;
        case 0xb1:  UnimplementedInstructions(state); break;
        case 0xb2:  UnimplementedInstructions(state); break;
        case 0xb3:  UnimplementedInstructions(state); break;
        case 0xb4:  UnimplementedInstructions(state); break;
        case 0xb5:  UnimplementedInstructions(state); break;
        case 0xb6:  UnimplementedInstructions(state); break;
        case 0xb7:  UnimplementedInstructions(state); break;
        case 0xb8:  UnimplementedInstructions(state); break;
        case 0xb9:  UnimplementedInstructions(state); break;
        case 0xba:  UnimplementedInstructions(state); break;
        case 0xbb:  UnimplementedInstructions(state); break;
        case 0xbc:  UnimplementedInstructions(state); break;
        case 0xbd:  UnimplementedInstructions(state); break;
        case 0xbe:  UnimplementedInstructions(state); break;
        case 0xbf:  UnimplementedInstructions(state); break;
        case 0xc0:  UnimplementedInstructions(state); break;
        case 0xc1:  //POP B
                    {
                    state->b = state->memory[state->sp+1];
                    state->c = state->memory[state->sp];
                    state->sp +=2;
                    }
                    break;
        case 0xc2:  //JNZ
                    {
                    if (!(state->cc.z)) state->pc = (opcode[2] << 8) | opcode[1];
                    else state->pc += 2; 
                    }
                    break;
        case 0xc3:  //JMP
                    {
                    state->pc = (opcode[2] << 8) | opcode[1];
                    break;
                    }
        case 0xc4:  UnimplementedInstructions(state); break;
        case 0xc5:  //PUSH B
                    {
                    state->memory[state->sp-2] = state->c;
                    state->memory[state->sp-1] = state->b;
                    state->sp -= 2;
                    }
                    break;
        case 0xc6:  //ADI
                    {
                    uint16_t answer = (uint16_t) state->a + (uint16_t) opcode[1];
                    state->cc.z = (0 == (answer & 0xff));
                    state->cc.s = (0 != (answer & 0x80));
                    state->cc.cy = (answer > 0xff);
                    state->cc.p = Parity(answer, 8);
                    state->a = answer; 
                    }
                    break;
        case 0xc7:  UnimplementedInstructions(state); break;
        case 0xc8:  UnimplementedInstructions(state); break;
        case 0xc9:  //RET
                    {
                    state->pc = (state->memory[state->sp+1]<<8) | state->memory[state->sp];
                    state->sp += 2;
                    break;
                    }
        case 0xca:  UnimplementedInstructions(state); break;
        case 0xcb:  UnimplementedInstructions(state); break;
        case 0xcc:  UnimplementedInstructions(state); break;
        case 0xcd:  //CALL 
                    {
                    uint16_t ret = state->pc+2;
                    state->memory[state->sp-1] = ((ret >> 8) & 0xff);
                    state->memory[state->sp-2] = ret & 0xff;
                    state->sp = state->sp-2;
                    state->pc = (opcode[2] << 8) | opcode[1];
                    }
                    break;
        case 0xce:  UnimplementedInstructions(state); break;
        case 0xcf:  UnimplementedInstructions(state); break;
        case 0xd0:  UnimplementedInstructions(state); break;
        case 0xd1:  //POP D
                    {
                    state->d = state->memory[state->sp+1];
                    state->e = state->memory[state->sp];
                    state->sp +=2;
                    }
                    break;
        case 0xd2:  UnimplementedInstructions(state); break;
        case 0xd3:  //OUT
                    {
                    state->pc++;
                    }
                    break;
        case 0xd4:  UnimplementedInstructions(state); break;
        case 0xd5:  //PUSH D
                    {
                    state->memory[state->sp -1] = state->d;
                    state->memory[state->sp -2] = state->e;
                    state->sp -= 2; 
                    }
                    break;
        case 0xd6:  UnimplementedInstructions(state); break;
        case 0xd7:  UnimplementedInstructions(state); break;
        case 0xd8:  UnimplementedInstructions(state); break;
        case 0xd9:  UnimplementedInstructions(state); break;
        case 0xda:  UnimplementedInstructions(state); break;
        case 0xdb:  UnimplementedInstructions(state); break;
        case 0xdc:  UnimplementedInstructions(state); break;
        case 0xdd:  UnimplementedInstructions(state); break;
        case 0xde:  UnimplementedInstructions(state); break;
        case 0xdf:  UnimplementedInstructions(state); break;
        case 0xe0:  UnimplementedInstructions(state); break;
        case 0xe1:  //POP H
                    {
                    state->h = state->memory[state->sp+1];
                    state->l = state->memory[state->sp];
                    state->sp +=2;
                    }
                    break;
        case 0xe2:  UnimplementedInstructions(state); break;
        case 0xe3:  UnimplementedInstructions(state); break;
        case 0xe4:  UnimplementedInstructions(state); break;
        case 0xe5:  //PUSH H
                    {
                    state->memory[state->sp -1] = state-> h;
                    state->memory[state->sp -2] = state->l;
                    state->sp -= 2;
                    }
                    break;
        case 0xe6:  //ANI
                    {
                    uint8_t x = state->a & opcode[1];
                    state->cc.z = (x==0);
                    state->cc.s = (0 != (x & 0x80));
                    state->cc.p = Parity(x, 8);
                    state->cc.cy = 0;
                    state->a = x;
                    state->pc++;
                    }
                    break;
        case 0xe7:  UnimplementedInstructions(state); break;
        case 0xe8:  UnimplementedInstructions(state); break;
        case 0xe9:  UnimplementedInstructions(state); break;
        case 0xea:  UnimplementedInstructions(state); break;
        case 0xeb:  //XCHG
                    {
                    uint8_t x = state->d;
                    uint8_t y = state->e;
                    state->d = state->h;
                    state->e = state->l;
                    state->h = x;
                    state->l = y;
                    }
                    break;
        case 0xec:  UnimplementedInstructions(state); break;
        case 0xed:  UnimplementedInstructions(state); break;
        case 0xee:  UnimplementedInstructions(state); break;
        case 0xef:  UnimplementedInstructions(state); break;
        case 0xf0:  UnimplementedInstructions(state); break;
        case 0xf1:  //POP PSW
                    {
                    state->a = state->memory[state->sp+1];
                    uint8_t x= state->memory[state->sp];
                    state->cc.z = (0x1 == (x & 0x1));
                    state->cc.s = (0x2 == (x & 0x2));
                    state->cc.p = (0x4 == (x & 0x4));
                    state->cc.cy = (0x5 == (x & 0x5));
                    state->cc.ac = (0x10 == (x & 0x10));
                    state->sp += 2;   
                    }
                    break;
        case 0xf2:  UnimplementedInstructions(state); break;
        case 0xf3:  UnimplementedInstructions(state); break;
        case 0xf4:  UnimplementedInstructions(state); break;
        case 0xf5:  //PUSH PSW
                    {
                    state-> memory[state->sp-1] = state->a;
                    uint8_t x = (state->cc.z |    
                            state->cc.s << 1 |    
                            state->cc.p << 2 |    
                            state->cc.cy << 3 |    
                            state->cc.ac << 4 );
                    state->memory[state->sp-2] = x;    
                    state->sp = state->sp - 2;    
                    }    
                    break;
        case 0xf6:  UnimplementedInstructions(state); break;
        case 0xf7:  UnimplementedInstructions(state); break;
        case 0xf8:  UnimplementedInstructions(state); break;
        case 0xf9:  UnimplementedInstructions(state); break;
        case 0xfa:  UnimplementedInstructions(state); break;
        case 0xfb:  //EI
                    {
                    state->int_enable = 1;
                    break;
                    }
        case 0xfc:  UnimplementedInstructions(state); break;
        case 0xfd:  UnimplementedInstructions(state); break;
        case 0xfe:  //CPI
                    {
                    uint8_t x = state->a - opcode[1];
                    state->cc.z = (0== x);
                    state->cc.cy = (0 > x);
                    state->cc.s = (0x80 == (x & 0x80));
                    state->cc.p = Parity(x, 8);
                    state->pc++;
                    }
                    break;
        case 0xff:  UnimplementedInstructions(state); break;

    }
    printf("\t");
	printf("%c", state->cc.z ? 'z' : '.');
	printf("%c", state->cc.s ? 's' : '.');
	printf("%c", state->cc.p ? 'p' : '.');
	printf("%c", state->cc.cy ? 'c' : '.');
	printf("%c  ", state->cc.ac ? 'a' : '.');
	printf("A $%02x B $%02x C $%02x D $%02x E $%02x H $%02x L $%02x SP %04x\n", state->a, state->b, state->c,
				state->d, state->e, state->h, state->l, state->sp);
    
    
    return 0;
}


void ReadFile(State8080* state, char* filename, uint32_t offset)
{
    /*Open the file containing the hex code*/
	FILE *fp = fopen(filename, "rb");
	if (fp == NULL)
	{
		printf("error: Couldn't open %s\n",filename);
		exit(1);
	}

	fseek(fp, 0L, SEEK_END);
	int fsize = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	uint8_t *buffer = &state->memory[offset];
    
	fread(buffer, fsize, 1, fp);
	fclose(fp);
}

State8080* Initialize8080(void)
{   
    State8080* state = calloc(1, sizeof(State8080));
    state->memory = malloc(0x10000);
    return state;
}

-(uint8_t) MachineIn(uint8_t port)
{
    uint8_t a;
    switch(port)
    {
        case 3:
        {
            uint16_t v = (shift << 8) | shift0;
            a = ((v>> (8-shift_offset)) & 0xff);
        }
        break;
    }
    return a;
}

void MachineOut (uint8_t port, uint8_t value)
{
    switch(port)
    {
        case 2:
            shift_offset = value & 0x7;
            break;
        case 4:
            shift0 = shift1;
            shift1 = value;
            break;
    }
}

int main(int argc, char**argv)
{
    int done = 0;
    int vblankcycles = 0;
    State8080* state = Initialize8080();

    ReadFile(state, "invaders.h", 0);
    ReadFile(state, "invaders.g", 0x800);
    ReadFile(state, "invaders.f", 0x1000);
    ReadFile(state, "invaders.e", 0x1800);

    while (done == 0)
    {
        uint8_t opcode = state->memory[state->pc];
        if (*opcode == 0xdb) //IN
        {
            uint8_t port = opcode[1];
            state->a = MachineIn(state, port);
            state->pc++;
        }
        else if (*opcode == 0xd3) //OUT
        {
            uint8_t port = opcode[1];
            MachineOut(state, port);
            
        }
        else
            done = Emulate8080(state);
    }
    return 0;
}