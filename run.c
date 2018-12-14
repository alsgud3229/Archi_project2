/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   SCE212 Ajou University                                    */
/*   run.c                                                     */
/*   Adapted from CS311@KAIST                                  */
/*                                                             */
/***************************************************************/

#include <stdio.h>

#include "util.h"
#include "run.h"

/***************************************************************/
/*                                                             */
/* Procedure: get_inst_info                                    */
/*                                                             */
/* Purpose: Read insturction information                       */
/*                                                             */
/***************************************************************/
instruction* get_inst_info(uint32_t pc)
{
    return &INST_INFO[(pc - MEM_TEXT_START) >> 2];
}

/***************************************************************/
/*                                                             */
/* Procedure: process_instruction                              */
/*                                                             */
/* Purpose: Process one instrction                             */
/*                                                             */
/***************************************************************/
void process_instruction()
{	
	instruction *instr = get_inst_info(CURRENT_STATE.PC);
	CPU_State *cpu = &CURRENT_STATE;
	CURRENT_STATE.PC += 4;	

    short opcode = OPCODE(instr);
	unsigned char rs;
	unsigned char rt;
	short imm;
	unsigned char rd;
	unsigned char shamt;
    uint32_t target;
    short funct;

    printf("current instruction: %x\n", opcode);
    if(opcode==0){
    	 printf("current function %x\n", funct);
    }


	if(opcode == 0){ 	 // R type	
		rs=RS(instr);
		rt=RT(instr);
		rd=RD(instr);
		shamt=SHAMT(instr);
		funct=FUNC(instr);

		if(funct==0x21)	cpu->REGS[rd] = cpu->REGS[rs] + cpu->REGS[rt]; //addu
		else if(funct==0x24) cpu->REGS[rd] = cpu->REGS[rs] & cpu->REGS[rt]; //and
		else if(funct==0x8) CURRENT_STATE.PC = cpu->REGS[rs]; //jr
		else if(funct==0x27) cpu->REGS[rd] = ~(cpu->REGS[rs] | cpu->REGS[rt]); //nor
		else if(funct==0x25) cpu->REGS[rd] = cpu->REGS[rs] | cpu->REGS[rt]; //or
		else if(funct==0x2b) cpu->REGS[rd] = (cpu->REGS[rs] < cpu->REGS[rt]) ? 1:0; //sltu
		else if(funct==0x0) cpu->REGS[rd] = cpu->REGS[rt] << shamt; //sll
		else if(funct==0x2) cpu->REGS[rd] = cpu->REGS[rt] >> shamt;	//srl
		else if(funct==0x23) cpu->REGS[rd] = cpu->REGS[rs] - cpu->REGS[rt]; //subu	
		else printf("R type: exception occured.\n");
	}
	else if(opcode==2 || opcode==3){ // J type
		target = 4*TARGET(instr);

		if(opcode==2){	//	j
			JUMP_INST(target);
		} 	
		else if(opcode==3){	// jal
			cpu->REGS[31] = CURRENT_STATE.PC + 4;
			JUMP_INST(target);
		}
		else printf("J type: exception occured.\n");
	}
	else{				// I Type
		rs=RS(instr);
		rt=RT(instr);
		imm=IMM(instr);	

		if(opcode==0x9)	cpu->REGS[rt] = cpu->REGS[rs] + SIGN_EX(imm); //addiu
		else if(opcode==0xc) cpu->REGS[rt] = cpu->REGS[rs] & (imm | 0x00000000); //andi
		else if(opcode==0x4){ //beq
			BRANCH_INST(cpu->REGS[rs]==cpu->REGS[rt], CURRENT_STATE.PC+(SIGN_EX(imm)<<2), NULL);
		}
		else if(opcode==0x5){ //bne
			BRANCH_INST(cpu->REGS[rs]!=cpu->REGS[rt], CURRENT_STATE.PC+(SIGN_EX(imm)<<2), NULL);
		}
		else if(opcode==0xf) cpu->REGS[rt] = imm<<16; //lui
		else if(opcode==0x23) cpu->REGS[rt] = mem_read_32(cpu->REGS[rs] + SIGN_EX(imm)); //lw
		else if(opcode==0xd) cpu->REGS[rt] = cpu->REGS[rs] | (imm | 0x00000000); //ori
		else if(opcode==0xb) cpu->REGS[rt] = (cpu->REGS[rs] < SIGN_EX(imm)) ? 1:0;	//sltiu
		else if(opcode==0x2b) mem_write_32(cpu->REGS[rs] + SIGN_EX(imm), cpu->REGS[rt]); //sw
		else printf("I type: exception occured.\n");
	}


// ===================do not touch the below=========================


	if((CURRENT_STATE.PC - MEM_TEXT_START == 4*NUM_INST))    RUN_BIT=FALSE;
}
