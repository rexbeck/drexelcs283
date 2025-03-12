#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "dshlib.h"
#define SPACE_STRING " "
#define QUOTE_STRING "\""
#define READ_END 0
#define WRITE_END 1
#define DO_OR_DIE(x, s) do { \
    if ((x) < 0) { \
        perror(s); \
        exit(1);   \
    } \
} while (0)

/**** 
 **** FOR REMOTE SHELL USE YOUR SOLUTION FROM SHELL PART 3 HERE
 **** THE MAIN FUNCTION CALLS THIS ONE AS ITS ENTRY POINT TO
 **** EXECUTE THE SHELL LOCALLY
 ****
 */


 cmd_buff_t build_cmd(char *cmd, int *rc)
 {
     cmd_buff_t cmd_buff = { .argc = 0, ._cmd_buffer = strdup(cmd) };
 
     if (cmd_buff._cmd_buffer == NULL) {
         *rc = ERR_MEMORY;
         return cmd_buff;
     }
 
     char *quote = strstr(cmd, QUOTE_STRING);
 
     if (quote == NULL) // HANDLES WHEN NO QUOTES ARE FOUND
     {
         char *arg = strtok(cmd_buff._cmd_buffer, SPACE_STRING);
         
         while (arg != NULL){
             if (cmd_buff.argc == CMD_ARGV_MAX){
                 *rc = ERR_TOO_MANY_COMMANDS;
                 break;
             }
             cmd_buff.argv[cmd_buff.argc] = strdup(arg);
             cmd_buff.argc++;
             arg = strtok(NULL, SPACE_STRING);
         }
     }
     else { // HANDLES WHEN COMMAND HAS QUOTES
         char *args = strtok(cmd, QUOTE_STRING);
         char *quoted_arg = strtok(NULL, QUOTE_STRING);
         
         char *arg = strtok(args, SPACE_STRING);
         while (arg != NULL){
             cmd_buff.argv[cmd_buff.argc] = strdup(arg);
             cmd_buff.argc++;
             arg = strtok(NULL, SPACE_STRING);
         }
         cmd_buff.argv[cmd_buff.argc] = strdup(quoted_arg);
         cmd_buff.argc++;
     }
     return cmd_buff;
 }
 
 
 
 void copy_cmd_buff(cmd_buff_t *dest, const cmd_buff_t *src)
 {
     dest->argc = src->argc;
     dest->_cmd_buffer = strdup(src->_cmd_buffer);
     if (!dest->_cmd_buffer) return;
 
     for (int i = 0; i < src->argc; i++)
     {
         dest->argv[i] = strdup(src->argv[i]);
     }
 }
 
 
 
 
 int build_cmd_list(char *cmd_line, command_list_t *clist)
 {
     int counter = 0, rc = 0;
     char *cmd_copy = strdup(cmd_line);
     char *pipe_ptr;
 
     char *pipe = strstr(cmd_line, PIPE_STRING);
     if (pipe == NULL)
     {
         cmd_buff_t cmd_buff = build_cmd(cmd_line, &rc);
         if (rc == 0)
         {
             clist->commands[0] = cmd_buff;
             clist->num = 1;
         }
     }
     else
     {
         char *pipe_token = strtok_r(cmd_copy, PIPE_STRING, &pipe_ptr);
         while (pipe_token != NULL)
         {
             if (counter >= CMD_MAX) {
                 free(cmd_copy);
                 return ERR_TOO_MANY_COMMANDS;
             }
 
             cmd_buff_t cmd_buff = build_cmd(pipe_token, &rc);
             if (rc != 0)
             {
                 free(cmd_copy);
                 return rc;
             }
             copy_cmd_buff(&clist->commands[counter], &cmd_buff);
             //clist->commands[counter] = cmd_buff;
             counter++;
             clist->num = counter;
             pipe_token = strtok_r(NULL, PIPE_STRING, &pipe_ptr);
         }
     }
     free(cmd_copy);
     return rc;
 }
 
 
 int exec_cmd(cmd_buff_t *cmd)
 {
     if (strcmp(cmd->argv[0], EXIT_CMD) == 0)
     {
         return OK_EXIT;
     }
     else if (strcmp(cmd->argv[0], "cd") == 0)
     {
         if (cmd->argc == 1) {
             return ERR_EXEC_CMD;
         }
         else if (cmd->argc > 2){
             printf("Command \"cd\" only one (1) argument.\n");
             return ERR_EXEC_CMD;
         }
         
         if (chdir(cmd->argv[1]) != 0){
             printf("chdir to %s failed\n", cmd->argv[1]);
             return ERR_EXEC_CMD;
         }
     }
     else {
         pid_t pid;
         DO_OR_DIE(pid = fork(), "fork");
         if (pid == 0) {
             // Child process
             DO_OR_DIE(execvp(cmd->argv[0], cmd->argv), "execvp");
         } else if (pid > 0) {
             // Parent process
             wait(NULL);
         }
     }
     return OK;
 }


 
 int execute_pipeline(command_list_t *clist)
 {
     int pipes[clist->num - 1][2], num = clist->num;  // Array of pipes
     pid_t pids[clist->num];        // Array to store process IDs
 
     // Create all necessary pipes
     for (int i = 0; i < num - 1; i++) {
         if (pipe(pipes[i]) == -1) {
             perror("pipe");
             exit(EXIT_FAILURE);
         }
     }
 
     // Create processes for each command
     for (int i = 0; i < num; i++) {
         pids[i] = fork();
         if (pids[i] == -1) {
             perror("fork");
             exit(EXIT_FAILURE);
         }
 
         if (pids[i] == 0) {  // Child process
             // Set up input pipe for all except first process
             if (i > 0) {
                 dup2(pipes[i-1][0], STDIN_FILENO);
             }
 
             // Set up output pipe for all except last process
             if (i < num - 1) {
                 dup2(pipes[i][1], STDOUT_FILENO);
             }
 
             // Close all pipe ends in child
             for (int j = 0; j < num - 1; j++) {
                 close(pipes[j][0]);
                 close(pipes[j][1]);
             }
 
             // Execute command
             cmd_buff_t cmd = clist->commands[i];
             execvp(cmd.argv[0], cmd.argv);
             perror("execvp");
             exit(EXIT_FAILURE);
         }
     }
 
     // Parent process: close all pipe ends
     for (int i = 0; i < num - 1; i++) {
         close(pipes[i][0]);
         close(pipes[i][1]);
     }
 
     // Wait for all children
     for (int i = 0; i < num; i++) {
         waitpid(pids[i], NULL, 0);
     }
 
     return OK;
 }

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
int exec_local_cmd_loop()
{
    char *cmd_buff, first;
    int rc = 0;
    cmd_buff_t cmd;
    command_list_t *command_list;

    while(true){
        cmd_buff = malloc(SH_CMD_MAX);
        if (!cmd_buff){
            return ERR_MEMORY;
        }
        command_list = malloc(sizeof(command_list_t));
        if (!command_list) {
            return ERR_MEMORY;
        }

        printf("%s", SH_PROMPT);
        if (fgets(cmd_buff, ARG_MAX, stdin) == NULL){
            rc = WARN_NO_CMDS;
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

        // create command_list
        rc = build_cmd_list(cmd_buff, command_list);
        if (rc != OK)
        {
            break;
        }
        
        // run command
        if (command_list->num == 1)
        {
            cmd = command_list->commands[0];
            rc = exec_cmd(&cmd);
        }
        else 
        {
            rc = execute_pipeline(command_list);
        }

        if (rc == OK_EXIT)
        {
            break;
        }

        free(command_list);
        free(cmd_buff);
    }

    free(command_list);
    free(cmd_buff);
    return rc;
}