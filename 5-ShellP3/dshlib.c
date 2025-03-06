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


int exec_local_cmd_loop()
{
    char *cmd_buff, first;
    int rc = 0;
    cmd_buff_t cmd;
    command_list_t *command_list;

    command_list = malloc(sizeof(command_list_t));
    if (!command_list) {
        return ERR_MEMORY;
    }

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

        rc = build_cmd_list(cmd_buff, command_list);
        if (rc != OK)
        {
            free(command_list);
            return rc;
        }
        printf("command_list->num = %d\n", command_list->num);
        for(int i = 0; i > command_list->num; i++){
            printf("loop\n");
            printf("%d.\n", i);
            for (int j = 0; j > command_list->commands[i].argc; j++){
                printf("\t%s\n", command_list->commands[i].argv[j]);
            }
        }
        printf("there should be things above\n");
        return OK;
        // cmd.argc = 0;
        // memset(cmd.argv, '\0', sizeof(cmd.argv));
        // cmd._cmd_buffer = cmd_buff;
        
        // char *idx = strstr(cmd_buff, "\"");

        // if (idx == NULL) 
        // {
        //     char *token = strtok(cmd_buff, " ");
        //     while (token != NULL){
        //         cmd.argv[cmd.argc] = strdup(token);
        //         cmd.argc++;
        //         token = strtok(NULL, " ");
        //     }
        // }
        // else {
        //     char *quote_token = strtok(cmd_buff, "\"");
        //     quote_token = strtok(NULL, "\"");
            
        //     char *space_token = strtok(cmd_buff, " ");
        //     cmd.argv[0] = strdup(space_token);
        //     cmd.argv[1] = strdup(quote_token);
        //     cmd.argc = 2;
        // }

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

    free(command_list);
    free(cmd_buff);
    return rc;
}