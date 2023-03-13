/* This project is based on the MIPS Assembler of CS61C in UC Berkeley.
   The framework of this project is been modified to be suitable for RISC-V
   in CS110 course in ShanghaiTech University by Zhijie Yang in March 2019.
   Updated by Chibin Zhang and Zhe Ye in March 2021. Modified by Lei Jia 
   to be part of the project in Feb 2023.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assembler.h"
#include "tables.h"
#include "translate.h"
#include "translate_utils.h"

#define MAX_ARGS 3
#define BUF_SIZE 1024
const char *IGNORE_CHARS = " \f\n\r\t\v,()";

/*******************************
 * Helper Functions
 *******************************/

/* you should not be calling this function yourself. */
static void raise_label_error(uint32_t input_line, const char *label)
{
    write_to_log("Error - invalid label at line %d: %s\n", input_line, label);
}

/* call this function if more than MAX_ARGS arguments are found while parsing
   arguments.

   INPUT_LINE is which line of the input file that the error occurred in. Note
   that the first line is line 1 and that empty lines are included in the count.

   EXTRA_ARG should contain the first extra argument encountered.
 */
static void raise_extra_argument_error(uint32_t input_line,
                                       const char *extra_arg)
{
    write_to_log("Error - extra argument at line %d: %s\n", input_line,
                 extra_arg);
}

/* You should call this function if write_original_code() or translate_inst()
   returns 0.

   INPUT_LINE is which line of the input file that the error occurred in. Note
   that the first line is line 1 and that empty lines are included in the count.
 */
static void raise_instruction_error(uint32_t input_line, const char *name,
                                    char **args, int num_args)
{

    write_to_log("Error - invalid instruction at line %d: ", input_line);
    log_inst(name, args, num_args);
}

/* Truncates the string at the first occurrence of the '#' character. */
static void skip_comments(char *str)
{
    char *comment_start = strchr(str, '#');
    if (comment_start)
    {
        *comment_start = '\0';
    }
}

/* Reads STR and determines whether it is a label (ends in ':'), and if so,
   whether it is a valid label, and then tries to add it to the symbol table.

   INPUT_LINE is which line of the input file we are currently processing. Note
   that the first line is line 1 and that empty lines are included in this
   count.

   BYTE_OFFSET is the offset of the NEXT instruction (should it exist).

   Four scenarios can happen:
    1. STR is not a label (does not end in ':'). Returns 0.
    2. STR ends in ':', but is not a valid label. Returns -1.
    3a. STR ends in ':' and is a valid label. Addition to symbol table fails.
        Returns -1.
    3b. STR ends in ':' and is a valid label. Addition to symbol table succeeds.
        Returns 1.
 */
static int add_if_label(uint32_t input_line, char *str, uint32_t byte_offset,
                        SymbolTable *symtbl, int check_align)
{
    
    size_t len = strlen(str);
    if (str[len - 1] == ':')
    {
        str[len - 1] = '\0';
        if (is_valid_label(str))
        {
            /* add to table and return number of tokens, otherwise return -1 */
            if (add_to_table(symtbl, str, byte_offset, check_align) == 0)
            {
                return 1;
            }
            else
            {
                return -1;
            }
        }
        else
        {
            /* we raise error if no valid label is found */
            raise_label_error(input_line, str);
            return -1;
        }
    }
    else
    {
        return 0;
    }
}

/*******************************
 * Implement the Following
 *******************************/

/* First pass of the assembler. You should implement pass_two() first.

   This function should read each line, strip all comments, scan for labels,
   and pass instructions to write_original_code(), pass data to write_static_data(). 
   The symbol table and data segment should also been built and written to 
   specified file. The input file may or may not be valid. Here are some guidelines:

   1. Only one of ".data" or ".text" may be present per line. It must be the 
      first token present. Once you see a ".data" or ".text", the rest part
      will be treat as data or text segment until the next ".data" or ".text"
      Appears. Ignore the rest token of this line. Default to text segment 
      if not indicated.
    
   2. For data segment, we only considering ".word", ".half", ".byte" types.
      These types must be the first token of a line. The rest of the tokens 
      in this line will be stored as variables of that type in the data segment.
      You can assume that these numbers are legal with their types.

   3. Only one label may be present per line. It must be the first token
      present. Once you see a label, regardless of whether it is a valid label
      or invalid label, You can treat the NEXT token in this line as the 
      beginning of an instruction or a static data.

   4. If the first token is not a label, treat it as the name of an
      instruction. DO NOT try to check it is a valid instruction in this pass.
   
   5. Everything after the instruction name in the same line should be treated 
      as arguments to that instruction. If there are more than MAX_ARGS 
      arguments, call raise_extra_argument_error() and pass in the first extra 
      argument. Do not write that instruction to the original file (eg. don't call
      write_original_code())
   
   6. Only one instruction should be present per line. You do not need to do
       anything extra to detect this - it should be handled by guideline 5.
   
   8. A line containing only a label is valid. The address of the label should
      be the address of the next instruction or static data regardless of 
      whether there is a next instruction or static data or not.
   
   9. If an instruction contains an immediate, you should output it AS IS.
   
   10. Comments should always be skipped before any further process.

   11. Note that the starting address of the text segment is defined as TEXT_BASE
   in "../types.", and the starting address of the data segment is defined as 
   DATA_BASE in "../types.".

   12. Think about how you should use write_original_code() and write_static_data().
   
   Just like in pass_two(), if the function encounters an error it should NOT
   exit, but process the entire file and return -1. If no errors were
   encountered, it should return 0.
 */

int pass_one(FILE *input, FILE *original, FILE *symtbl, FILE *data) {
    SymbolTable* table=create_table(SYMBOLTBL_UNIQUE_NAME);
    int is_data=0; /* default to .text */
    uint32_t byted_offset=0, bytet_offset=0;
    /*testing*/
    char buf[BUF_SIZE]={0};
    int error=0;
    int linenum=0;/*testing*/
    char* temp=NULL;
    int tmp=0;
    int i,j;
    int argnum=0;
    char data_buf[9]={0};
    char tmp_buf[9]={0};/*testing*/
    int size_buf=0;
    long k=0;
    /*testing*/
    if(!input||!original||!symtbl||!data) return -1;
    while(fgets(buf,BUF_SIZE,input)){
        skip_comments(buf);
        linenum+=1; tmp=0;
        temp=strtok(buf,IGNORE_CHARS);
        if(temp==NULL) continue;
        /*data*/
        else if(!strcmp(temp,".data")){
            is_data=1;continue;
        }/*text*/
        else if(!strcmp(temp,".text")){
            is_data=0;continue;
        }
        else{
            if(is_data) tmp=add_if_label(linenum,temp,byted_offset+DATA_BASE,table,0);
            else  tmp=add_if_label(linenum,temp,bytet_offset+TEXT_BASE,table,0);
            if(tmp==-1){/*testing*/
                error=-1;continue;
            }
            if(tmp==1){
                temp=strtok(NULL,IGNORE_CHARS);
                if(!temp) continue; 
            }/*testing*/
        }
        if(is_data){/*testing*/
            if(strcmp(temp,".word")==0){
                temp=strtok(NULL,IGNORE_CHARS);
                while(temp){
                    k=strtol(temp,NULL,0);/* k exists when ptr exists */
                    sprintf(tmp_buf,"%08x",(int)k);
                    if (size_buf==1||size_buf==2||size_buf==4||size_buf==6){
                        for (i=0;i<8;i++){
                            j = (i+size_buf<=7)? (i+size_buf):(i+size_buf-8);
                            data_buf[i] = tmp_buf [j] ;
                            if(j==7){
                                fprintf(data,"%s\n",data_buf);
                                strcpy(data_buf,"00000000");
                            }
                        }
                    }
                  /*testing*/
                    /*write_static_data(data,(Byte*)temp);*/
                    byted_offset+=4;
                    temp=strtok(NULL,IGNORE_CHARS);
                }/*testing*/
            continue;
            }/*testing*/
            else if(strcmp(temp,".byte")==0){
                /* buf_2 to k at most 4 bytes of data, aka a line */
                temp=strtok(NULL,IGNORE_CHARS);
                while(temp){      
                    k=strtol(temp,NULL,0);/* k exists when ptr exists */
                    sprintf(tmp_buf,"%02x",(int)k);
                    /*testing*/
                    data_buf[7-size_buf-1]=tmp_buf[0];
                    data_buf[7-size_buf]=tmp_buf[1];
                    size_buf+=2;
                    /*if(size_buf==8){
                        size_buf=0;
                        fprintf(data,"%s\n",data_buf);
                        strcpy(data_buf,"00000000");
                    }*/
                    byted_offset+=1;
                    temp=strtok(NULL,IGNORE_CHARS);
                }/*testing*/
                continue;
            }
            else if(strcmp(temp,".half")==0){
                temp=strtok(NULL,IGNORE_CHARS);/*testing*/
                while(temp){             
                    k=strtol(temp,NULL,0);/* k exists when ptr exists */
                    sprintf(tmp_buf,"%04x",(int)k);
                    if(size_buf<=4){
                        for (i = 0;i<4;i++){
                            data_buf[4+i-size_buf] = tmp_buf[i];
                            if(size_buf == 4){
                                size_buf=0;
                                fprintf(data,"%s\n",data_buf);
                                strcpy(data_buf,"00000000");
                            }
                        }
                    }
                    /*if(size_buf==6){
                        for(i =0;i<2;i++) data_buf[i]=tmp_buf[i+2];
                        fprintf(data,"%s\n",data_buf);
                        strcpy(data_buf,"00000000");
                        data_buf[6]=tmp_buf[0];
                        data_buf[7]=tmp_buf[1];
                    }*/
                    byted_offset+=2;
                    temp=strtok(NULL,IGNORE_CHARS);
                }
                continue;
            }/*testing*/
        }
        else{
            char* name=NULL; /*k name*/
            char* args[MAX_ARGS]={0};/*max argument number: 3 */
            argnum=0;
            if(temp){
                name= temp;
                temp=strtok(NULL,IGNORE_CHARS);
                while(temp){
                    if(argnum>=MAX_ARGS){
                        raise_extra_argument_error(linenum,temp);
                        error=-1;
                        break;
                    }/*testing*/
                    args[argnum]=temp;
                    argnum++;
                    temp=strtok(NULL,IGNORE_CHARS);
                }
                bytet_offset+=4*write_original_code(original,name,args,argnum);
            }
           /*testing*/
        }
    }
    if(size_buf!=0) fprintf(data,"%s\n",data_buf);
    write_table(table, symtbl);
    free_table(table);/*testing*/
    return error;
}/*testing*/

/* Second pass of the assembler.

   This function should read an original file and the corresponding symbol table
   file, translates it into basic code and machine code. You may assume:
    1. The input file contains no comments
    2. The input file contains no labels
    3. The input file contains at maximum one instruction per line
    4. All instructions have at maximum MAX_ARGS arguments
    5. The symbol table file is well formatted
    6. The symbol table file contains all the symbol required for translation
    7. The line may contain only one "-" which is placeholder you can ignore.
   If an error is reached, DO NOT EXIT the function. Keep translating the rest of
   the document, and at the end, return -1. Return 0 if no errors were encountered. */

int pass_two(FILE *original, FILE *symtbl, FILE *basic, FILE *machine) {
    /* YOUR CODE HERE */
    SymbolTable *table = create_table_from_file(SYMBOLTBL_UNIQUE_NAME,symtbl);
    char *args[MAX_ARGS];
    int error = 0;
    char buf[BUF_SIZE];
    /* line number & byte offset */
    int linenum = 0,  offset = 0;
    char *name = NULL; char* temp= NULL;
    int argnum;int transinst;
    while (fgets(buf, sizeof(buf), original)){
        /*int i = 0;*/
        linenum ++;
        temp = strtok(buf, IGNORE_CHARS);
        write_to_log("%s ", temp);
        if(temp) name = temp;
        else{
            error =-1;
            continue;
        }
        if(!strcmp(temp,"-")) continue;/* if '-' continue */
        argnum = 0;
        /* step to the next line if there is no character left*/
        temp = strtok(NULL,IGNORE_CHARS);
        while(temp!=NULL){
            args[argnum] = temp;
            temp = strtok(NULL,IGNORE_CHARS);
            argnum++;
        }
        
        /*for (i = 0; i < argnum; i++)
            write_to_log("%s ", args[i]);
        write_to_log("%d", argnum);
        write_to_log("\n");*/
        
        /* translate the instruction */
        transinst = translate_inst(basic, machine, name, args, argnum, 4*(linenum-1), table);
        if (!transinst){ /* 0 if error */
            raise_instruction_error(linenum, name, args, argnum);
            error=-1;
        }
        else{/* add offset */
            offset += 4;
        }
    }
    /* no characters left */
    free_table(table);
    return error;
}

/*******************************
 * Do Not Modify Code Below
 *******************************/


/* Runs the two-pass assembler. Most of the actual work is done in pass_one()
   and pass_two().
 */
int assemble(int mode, FILE *input, FILE *data, FILE *symtbl, FILE *orgin, FILE *basic, FILE *text)
{
    int err = 0;

    /* Execute pass one if mode two is not specified */
    if (mode != 2)
    {
        rewind(input);
        if (pass_one(input, orgin, symtbl, data) != 0)
        {
            err = 1;
        }
    }

    /* Execute pass two if mode one is not specified */
    if (mode != 1)
    {
        rewind(orgin);
        rewind(symtbl);
        if (pass_two(orgin, symtbl, basic, text) != 0)
        {
            err = 1;
        }
    }

    /* Error handling */
    if (err) {
        write_to_log("One or more errors encountered during assembly operation.\n");
    } else {
        write_to_log("Assembly operation completed successfully!\n");
    }

    return err;
}