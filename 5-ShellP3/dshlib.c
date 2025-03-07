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
    int num_cmd = clist->num, left_pipe[2], right_pipe[2], status;
    pid_t pid[num_cmd];

    DO_OR_DIE(pipe(left_pipe), "left pipe");
    DO_OR_DIE(pipe(right_pipe), "right pipe");

    // first child
    DO_OR_DIE(pid[0] = fork(), "fork");
    if (pid[0] == 0)
    {
        DO_OR_DIE(dup2(left_pipe[WRITE_END], STDOUT_FILENO), "dup2 left_pipe stdout");
        // close both ends
        close(left_pipe[READ_END]);
        close(left_pipe[WRITE_END]);
        close(right_pipe[READ_END]);
        close(right_pipe[WRITE_END]);
        
        cmd_buff_t cmd = clist->commands[0];
        DO_OR_DIE(execvp(cmd.argv[0], cmd.argv), "execvp");
    }

    // middle children, only needed when 3 or more
    if (num_cmd >= 3)
    {
        for(int i = 1; i < num_cmd - 1; i++)
        {
            DO_OR_DIE(pid[i] = fork(), "fork");
            if (pid[i] == 0)
            {
                DO_OR_DIE(dup2(left_pipe[READ_END], STDIN_FILENO), "dup2 left stdin");
                DO_OR_DIE(dup2(right_pipe[WRITE_END], STDOUT_FILENO), "dup2 right stdout");

                // closes all ends of both pipes
                close(left_pipe[READ_END]);
                close(left_pipe[WRITE_END]);
                close(right_pipe[READ_END]);
                close(right_pipe[WRITE_END]);

                cmd_buff_t cmd = clist->commands[i];
                DO_OR_DIE(execvp(cmd.argv[0], cmd.argv), "execvp");
            }
        }
    }

    // last child
    DO_OR_DIE(pid[num_cmd - 1] = fork(), "fork");
    if (pid[num_cmd - 1] == 0)
    {
        DO_OR_DIE(dup2(right_pipe[READ_END], STDIN_FILENO), "dup2 right stdin");
        // close both ends of pipe
        close(left_pipe[READ_END]);
        close(left_pipe[WRITE_END]);
        close(right_pipe[READ_END]);
        close(right_pipe[WRITE_END]);
        
        cmd_buff_t cmd = clist->commands[num_cmd - 1];
        DO_OR_DIE(execvp(cmd.argv[0], cmd.argv), "execvp");
    }

    close(left_pipe[READ_END]);
    close(left_pipe[WRITE_END]);
    close(right_pipe[READ_END]);
    close(right_pipe[WRITE_END]);

    for (int i = 0; i < num_cmd - 1; i++)
    {
        waitpid(pid[i], &status, 0);
    }

    return OK;
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

        // create command_list
        rc = build_cmd_list(cmd_buff, command_list);
        if (rc != OK)
        {
            free(command_list);
            return rc;
        }
        
        if (command_list->num == 1)
        {
            // WHEN ONLY A SINGLE COMMAND WAS PROVIDED
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
    }

    free(command_list);
    free(cmd_buff);
    return rc;
}