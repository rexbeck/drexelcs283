#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "dshlib.h"

/*
 * Implement your exec_local_cmd_loop function by building a loop that prompts the 
 * user for input.  Use the SH_PROMPT constant from dshlib.h and then
 * use fgets to accept user input.
 * 
 *      while(1){
 *        printf("%s", SH_PROMPT);
 *        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
 *           printf("\n");
 *           break;
 *        }
 *        //remove the trailing \n from cmd_buff
 *        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';
 * 
 *        //IMPLEMENT THE REST OF THE REQUIREMENTS
 *      }
 * 
 *   Also, use the constants in the dshlib.h in this code.  
 *      SH_CMD_MAX              maximum buffer size for user input
 *      EXIT_CMD                constant that terminates the dsh program
 *      SH_PROMPT               the shell prompt
 *      OK                      the command was parsed properly
 *      WARN_NO_CMDS            the user command was empty
 *      ERR_TOO_MANY_COMMANDS   too many pipes used
 *      ERR_MEMORY              dynamic memory management failure
 * 
 *   errors returned
 *      OK                     No error
 *      ERR_MEMORY             Dynamic memory management failure
 *      WARN_NO_CMDS           No commands parsed
 *      ERR_TOO_MANY_COMMANDS  too many pipes used
 *   
 *   console messages
 *      CMD_WARN_NO_CMD        print on WARN_NO_CMDS
 *      CMD_ERR_PIPE_LIMIT     print on ERR_TOO_MANY_COMMANDS
 *      CMD_ERR_EXECUTE        print on execution failure of external command
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 1+)
 *      malloc(), free(), strlen(), fgets(), strcspn(), printf()
 * 
 *  Standard Library Functions You Might Want To Consider Using (assignment 2+)
 *      fork(), execvp(), exit(), chdir()
 */

void print_buffer(char *buff)
{
    printf("Buffer:  [");
    for (int i=0; i<ARG_MAX; i++){
        putchar(*(buff+i));
    }
    putchar(']');
    putchar('\n');
}


// TODO IMPLEMENT MAIN LOOP

// TODO IMPLEMENT parsing input to cmd_buff_t *cmd_buff

// TODO IMPLEMENT if built-in command, execute builtin logic for exit, cd (extra credit: dragon)
// the cd command should chdir to the provided directory; if no directory is provided, do nothing

// TODO IMPLEMENT if not built-in command, fork/exec as an external command
// for example, if the user input is "ls -l", you would fork/exec the command "ls" with the arg "-l"
int exec_local_cmd_loop()
{
    char *cmd_buff, current, argv[CMD_ARGV_MAX][ARG_MAX], *temp;
    int rc = 0, buff_idx = 0, arg_count = 0, temp_idx = 0;
    cmd_buff_t cmd;

    while(1){
        cmd_buff = malloc(SH_CMD_MAX);
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
            printf("\n");
            break;
        }

        //remove the trailing \n from cmd_buff
        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';

        print_buffer(cmd_buff);

        current = *(cmd_buff + buff_idx);
        temp = malloc(ARG_MAX);
        while (1)
        {
            if (current == SPACE_CHAR){
                // add arg to argv and empty temp.
                strcpy(argv[arg_count], temp);
                arg_count++;
                temp_idx = 0;
                memset(temp, '\0', sizeof(temp));
                buff_idx++;
                current = *(cmd_buff + buff_idx);
                continue;
            }
            else if (current == '\0'){
                strcpy(argv[arg_count], temp);
                arg_count++;
                break;
            }

            temp[temp_idx] = current;

            temp_idx++;
            buff_idx++;

            current = *(cmd_buff + buff_idx);
        }
        free(temp);
        cmd.argc = arg_count;
        cmd.argv[arg_count] = argv;
        cmd._cmd_buffer = cmd_buff;

        

        //IMPLEMENT THE REST OF THE REQUIREMENTS
        free(cmd_buff);
    }

    return OK;
}