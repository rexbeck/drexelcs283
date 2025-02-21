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
    char *cmd_buff, first;
    int rc = 0;
    cmd_buff_t cmd;

    while(true){
        cmd_buff = malloc(SH_CMD_MAX);
        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
            printf("\n");
            rc = OK;
            break;
        }

        //remove the trailing \n from cmd_buff
        cmd_buff[strcspn(cmd_buff,"\n")] = '\0';

        // CHECK FOR EMPTY COMMAND
        first = *cmd_buff;
        if (first == '\0')
        {
            printf(CMD_WARN_NO_CMD);
            rc = WARN_NO_CMDS;
            break;
        }

        cmd.argc = 0;
        memset(cmd.argv, '\0', sizeof(cmd.argv));
        cmd._cmd_buffer = cmd_buff;
        
        char *idx = strstr(cmd_buff, "\"");

        if (idx == NULL) 
        {
            char *token = strtok(cmd_buff, " ");
            while (token != NULL){
                cmd.argv[cmd.argc] = strdup(token);
                cmd.argc++;
                token = strtok(NULL, " ");
            }
        }
        else {
            char *quote_token = strtok(cmd_buff, "\"");
            quote_token = strtok(NULL, "\"");
            
            char *space_token = strtok(cmd_buff, " ");
            cmd.argv[0] = strdup(space_token);
            cmd.argv[1] = strdup(quote_token);
            cmd.argc = 2;
        }

        if (strcmp(cmd.argv[0], EXIT_CMD) == 0)
        {
            rc = OK;
            break;
        }
        else if (strcmp(cmd.argv[0], "cd") == 0)
        {
            if (cmd.argc == 1) continue;
            else if (cmd.argc > 2){
                printf("Command \"cd\" only one (1) argument.\n");
                continue;
            }
            
            if (chdir(cmd.argv[1]) != 0){
                printf("chdir to %s failed\n", cmd.argv[1]);
                continue;
            }
        }
        else {
            pid_t pid = fork();
            if (pid == 0) {
                // Child process
                rc = execvp(cmd.argv[0], cmd.argv);
                if (rc == -1) {
                    printf("Execvp %s failed", cmd.argv[0]);
                    rc = ERR_EXEC_CMD;
                    break;
                }
            } else if (pid > 0) {
                // Parent process
                wait(NULL);
            } else {
                printf("Fork error");
                rc = ERR_EXEC_CMD;
                break;
            }
        }
    }

    free(cmd_buff);
    return rc;
}