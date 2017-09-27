#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define MAX_ARGS 20

int get_args(char *cmdline, char* args[])
{
    int i =0;
    if ((args[0] = strtok(cmdline, "\n\t ")) == NULL)
        return 0;
    while((args[++i] = strtok(NULL, "\n\t ")) != NULL)
    {
        if (i >= MAX_ARGS)
        {
            printf("Too Many args!\n");
            exit(1);
        }
    }
    return i;
}

void execute(char *cmdline)
{
    int pid, async;
    char* args[MAX_ARGS] = {NULL};
    int nargs = get_args(cmdline, args);

    if(nargs <= 0)
        return;
    if(!strcmp(args[0], "quit") || !strcmp(args[0], "exit"))
        exit(0);

    if(!strcmp(args[nargs-1], "&"))
    {
        async = 1;
        args[--nargs] = 0;
    }

    else
        async = 0;

    pid = fork();
    if(pid == 0)
    {
        execvp(args[0], args);
        perror("exec failed");
        exit(-1);
    }
    else if(pid >0)
    {
        if(!async)
            waitpid(pid, NULL, 0);
        else printf("PID %d\n", pid);
    }
    else
    {
        perror("Fork failed");
        exit(1);
    }
}

int main(int argc, char* argv[])
{
    char cmdline[BUFSIZ];
    if(argc == 2)
    {
        // reading from file
        char *file_name = argv[argc-1];
        FILE* file = fopen(file_name, "r");
        if(file == NULL)
        {
            printf("File %s does not exist.\n", file_name);
            exit(1);
        }
        while(fgets(cmdline, BUFSIZ, file) != NULL) 
        {
            char *save;
            char *token = strtok_r(cmdline, ";", &save);

            while(token != NULL)
            {
                execute(token);
                token = strtok_r(NULL, ";", &save);
            }
        }
    }
    else if(argc == 1)
    {
        while(1)
        {
            printf("Jake Lopez> ");
                if(fgets(cmdline, BUFSIZ, stdin) == NULL) 
                {
                    perror("\n Exiting shell...");
                    exit(1);
                }
                char *save;
                char *token = strtok_r(cmdline, ";", &save);

                while(token != NULL)
                {
                    execute(token);
                    token = strtok_r(NULL, ";", &save);
                }
        }
    }
    else
    {
        fprintf(stderr, "Error: too many arguements\nUsage: myshell [batchfile]\n");
        exit(1);
    }
    return 0;
}
