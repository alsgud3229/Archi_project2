/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   SCE212 Ajou University                                    */
/*   parse.c                                                   */
/*   Adapted from CS311@KAIST                                  */
/*                                                             */
/***************************************************************/

#include <stdio.h>

#include "util.h"
#include "parse.h"

int text_size;
int data_size;

instruction parsing_instr(const char *buffer, const int index)
{
    instruction instr;
    mem_write_32(MEM_TEXT_START+index, fromBinary((char*)buffer));
    instr.value=fromBinary((char*)buffer);

    char buf[7];
    strncpy(buf, buffer, 6);
    buf[6]='\0';
    instr.opcode=fromBinary(buf);

    if(fromBinary(buf)==0){ // R type
        strncpy(buf, buffer+6, 5);
        buf[5]='\0';
        instr.r_t.r_i.rs=fromBinary(buf);

        strncpy(buf, buffer+11, 5);
        buf[5]='\0';
        instr.r_t.r_i.rt=fromBinary(buf);

        strncpy(buf, buffer+16, 5);
        buf[5]='\0';
        instr.r_t.r_i.r_i.r.rd=fromBinary(buf);

        strncpy(buf, buffer+21, 5);
        buf[5]='\0';
        instr.r_t.r_i.r_i.r.shamt=fromBinary(buf);

        strncpy(buf, buffer+26, 6);
        buf[6]='\0';
        instr.func_code=fromBinary(buf);
    }
    else if(fromBinary(buf)==2 || fromBinary(buf)==3){ // J type
        instr.r_t.target=fromBinary((char*)buffer+6);
    }
    else{   // I Type
        strncpy(buf, buffer+6, 5);
        buf[5]='\0';
        instr.r_t.r_i.rs=fromBinary(buf);       

        strncpy(buf, buffer+11, 5);
        buf[5]='\0';
        instr.r_t.r_i.rt=fromBinary(buf);

        instr.r_t.r_i.r_i.imm=fromBinary((char*)buffer+16);
    }

    return instr;
}

void parsing_data(const char *buffer, const int index)  // index 4씩 증가함, buffer는 32비트 한개 워드 전체
{
    mem_write_32(MEM_DATA_START+index, fromBinary((char *)buffer));
}

void print_parse_result()
{
    int i;
    printf("Instruction Information\n");

    for(i = 0; i < text_size/4; i++)
    {
        // printf("INST_INFO[%d].value : %x\n",i, INST_INFO[i].value);
        printf("INST_INFO[%d].opcode : %d\n",i, INST_INFO[i].opcode);

	    switch(INST_INFO[i].opcode)
        {
            //Type I
            case 0x9:		//(0x001001)ADDIU
            case 0xc:		//(0x001100)ANDI
            case 0xf:		//(0x001111)LUI	
            case 0xd:		//(0x001101)ORI
            case 0xb:		//(0x001011)SLTIU
            case 0x23:		//(0x100011)LW
            case 0x2b:		//(0x101011)SW
            case 0x4:		//(0x000100)BEQ
            case 0x5:		//(0x000101)BNE
                printf("INST_INFO[%d].rs : %d\n",i, INST_INFO[i].r_t.r_i.rs);
                printf("INST_INFO[%d].rt : %d\n",i, INST_INFO[i].r_t.r_i.rt);
                printf("INST_INFO[%d].imm : %d\n",i, INST_INFO[i].r_t.r_i.r_i.imm);
                break;

            //TYPE R
            case 0x0:		//(0x000000)ADDU, AND, NOR, OR, SLTU, SLL, SRL, SUBU  if JR
                printf("INST_INFO[%d].func_code : %d\n",i, INST_INFO[i].func_code);
                printf("INST_INFO[%d].rs : %d\n",i, INST_INFO[i].r_t.r_i.rs);
                printf("INST_INFO[%d].rt : %d\n",i, INST_INFO[i].r_t.r_i.rt);
                printf("INST_INFO[%d].rd : %d\n",i, INST_INFO[i].r_t.r_i.r_i.r.rd);
                printf("INST_INFO[%d].shamt : %d\n",i, INST_INFO[i].r_t.r_i.r_i.r.shamt);
                break;

            //TYPE J
            case 0x2:		//(0x000010)J
            case 0x3:		//(0x000011)JAL
                printf("INST_INFO[%d].target : %d\n",i, INST_INFO[i].r_t.target);
                break;

            default:
                printf("Not available instruction\n");
                assert(0);
        }
    }

    // return; // @@@@@@@@@@@@@@@@@@@@ 나중에 지우기

    printf("Memory Dump - Text Segment\n");
    for(i = 0; i < text_size; i+=4)
        printf("text_seg[%d] : %x\n", i, mem_read_32(MEM_TEXT_START + i));
    for(i = 0; i < data_size; i+=4)
        printf("data_seg[%d] : %x\n", i, mem_read_32(MEM_DATA_START + i));
    printf("Current PC: %x\n", CURRENT_STATE.PC);
}
