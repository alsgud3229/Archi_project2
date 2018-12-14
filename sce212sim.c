/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   SCE212 Ajou University                                    */
/*   sce212.c                                                  */
/*   Adapted from CS311@KAIST                                  */
/*                                                             */
/***************************************************************/

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
/*          DO NOT MODIFY THIS FILE!                            */
/*          You should only the parse.c and run.c files!        */
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "util.h"
#include "parse.h"
#include "run.h"

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename) {                   
    FILE *prog;
    int ii, word;
    char buffer[33];
    //to notifying data & text segment size
    int flag = 0;
    int text_index = 0;
    int data_index = 0;

    /* Open program file. */
    prog = fopen(program_filename, "r");

    if (prog == NULL) {
        printf("Error: Can't open program file %s\n", program_filename);
        exit(-1);
    }

    /* Read in the program. */
    ii = 0;

    //read 32bits + '\0' = 33
    while (fgets(buffer,33,prog) != NULL) {
 
        if (flag == 0) {

            //check text segment size
            text_size = fromBinary(buffer);
            NUM_INST = text_size/4;
            //initial memory allocation of text segment
            INST_INFO = malloc(sizeof(instruction)*(text_size/4));
            init_inst_info(text_size/4);

        } else if (flag == 1) {

            //check data segment size
            data_size = fromBinary(buffer);
            //initial memory allocation of data segment
            //if you would like to add data, you can re-allocate memory.
            /* data_seg = malloc(sizeof(uint32_t)*(data_size/4)); */

        } else {
            
            if(ii < text_size) {
                INST_INFO[text_index++] = parsing_instr(buffer, ii);
            } else if(ii < text_size + data_size) {
                parsing_data(buffer, ii-text_size);
            } else {
                //Do not enter this case
                //assert(0);
                //However, there is a newline in the input file
            }
            ii += 4;
        }
        flag++;
    }

    CURRENT_STATE.PC = MEM_TEXT_START;
    //printf("Read %d words from program into memory.\n\n", ii/4);
}

/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */ 
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(char *program_filename) {
    int i;

    init_memory();
    load_program(program_filename);
    RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {
    char** tokens;
    int count = 1;
    int addr1 = 0;
    int addr2 = 0;
    int num_inst = 0;
    int i = 100;		//for loop

    int mem_dump_set = 0;
    int debug_set = 0;
    int num_inst_set = 0;

    /* Error Checking */
    if (argc < 2) {
        printf("Error: usage: %s [-m addr1:addr2] [-d] [-n num_instr] inputBinary\n", argv[0]);
        exit(1);
    }

    initialize(argv[argc-1]); // filename 받고 메모리영역 초기화후 load program

    //for checking parse result
    // print_parse_result(); // 분석 결과 체크하기 위한 함수

    while (count != argc-1) {

        if (strcmp(argv[count], "-m") == 0) {
            tokens = str_split(argv[++count],':');  // -m 필드에서받은 주소 각각 addr1, addr2에 저장후 mem_dump_set = 1 셋팅

            addr1 = (int)strtol(*(tokens), NULL, 16);
            addr2 = (int)strtol(*(tokens+1), NULL, 16);
            mem_dump_set = 1;

        } else if(strcmp(argv[count], "-d") == 0) {  // -d 필드 있으면 debug_set = 1 셋팅
            debug_set = 1;

        } else if(strcmp(argv[count], "-n") == 0) { // -n 카운트에서 받은 숫자 nun_inst에 셋팅후 nun_inst_set=1 셋팅
            num_inst = (int)strtol(argv[++count], NULL, 10);
            num_inst_set = 1;

        } else{
            printf("Error: usage: %s [-m addr1:addr2] [-d] [-n num_instr] inputBinary\n", argv[0]);
            exit(1);
        }
        count++;
    }  //while 문 끝

    if (num_inst_set) {
        i = num_inst;       // n번 돌리기위해 i=100에 n값 덮어씀
    }

    if (debug_set) {        // -d, 디버깅 모드 일때 
        printf("Simulating for %d cycles...\n\n", i);

        for(; i > 0; i--){  // n번 돌린것을 모두 보여줌으로써 디버깅하기위함
            cycle();    //n번의 process_instruction 출력
            rdump();    //n번의 rdump 출력

            if (mem_dump_set) {     // -m 필드했으면 mdump한다
                mdump(addr1, addr2); //n번 출력
            }
        }  //n번의 for문 종료

    } else {        // 디버깅모드 아닐때
        run(i);     
        rdump();    // n번 돌린값을 1회 출력

        if (mem_dump_set) {     // -m 필드했으면 mdump한다
            mdump(addr1, addr2);
        }
    }

    return 0;
}
