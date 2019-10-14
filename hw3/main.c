#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>


int check_if_num(char* word);
int check_error(int num, char *fail_msg, char *what, int exit_stat);
void handle_argv(int argc, char **argv);

int check_if_num(char* word)
{
    int i = 0;
    char c = word[0];
    for(i = 1; c != '\0'; i++)
    {
        if(!isdigit(c) && (c != '-'))
            return -1;
        c = word[i];
    }
    return 0;
}

int check_error(int num, char *fail_msg, char *what, int exit_stat)
{
    if(num < 0)
    {
        char msg[4096];
        strcpy(msg, fail_msg);
        sprintf(msg, msg, what);
        perror(msg);
        exit(exit_stat);
    } else
        return num;
}

void io_redirect(int argc, char **argv)
{
    for(int i = 0; i < argc; i++)
    {
        if(argv[i][0] == '<') {
            char new_path[4096];
            sprintf(new_path, "./%s", argv[i] + 1);
            int fd = check_error(open(new_path, O_RDONLY), "Error opening %s to redirect stdin", argv[i]+1, 1);
            check_error(dup2(fd, 0), "Error redirecting stdin to %s", argv[i]+1, 1);
            close(fd);

        }
        else if(argv[i][0] == '>')
        {
            int fd;
            char new_path[4096];
            if(argv[i][2] == '>')
            {
                sprintf(new_path, "./%s", argv[i]+2);
                fd = check_error(open(new_path, O_WRONLY | O_CREAT | O_APPEND, 0666),
                                 "Error opening %s to redirect stdout", argv[i] + 2, 1);
            } else {
                sprintf(new_path, "./%s", argv[i] + 1);
                fd = check_error(open(new_path, O_WRONLY | O_CREAT | O_TRUNC, 0666),
                                 "Error opening %s to redirect stdout", argv[i] + 1, 1);
            }
            check_error(dup2(fd, 1), "Error redirecting stdout to %s", argv[i]+1, 1);
            close(fd);

        }
        else if(argv[i][0] == '2' && argv[i][1] == '>')
        {
            int fd;
            char new_path[4096];
            if (argv[i][2] == '>')
            {
                sprintf(new_path, "./%s", argv[i]+2);
                fd = check_error(open(new_path, O_WRONLY | O_CREAT | O_APPEND, 0666),
                                 "Error opening %s to redirect stderr", argv[i] + 2, 1);
            } else
            {
                sprintf(new_path, "./%s", argv[i]+1);
                fd = check_error(open(new_path, O_WRONLY | O_CREAT | O_TRUNC, 0666),
                                 "Error opening %s to redirect stderr", argv[i] + 1, 1);
            }
            check_error(dup2(fd, 2), "Error redirecting stderr to %s", argv[i]+1, 1);
            close(fd);
        }
    }
}


int execute_command(char **argv, int argc, char **io_args, int io_argc)
{
    struct timeval start,end;
    gettimeofday(&start, NULL);
    int pid = check_error(fork(), "Error %s", "forking", 0);
    if(pid == 0)
    {
        io_redirect(io_argc, io_args);
        execvp(argv[0], argv);
        perror("exec sys call failed");
        exit(127);
    }
    else
    {
        struct rusage proc_usage;
        int proc_status;
        wait3(&proc_status, 0, &proc_usage);
        gettimeofday(&end, NULL);
        if(WIFSIGNALED(proc_status))
            printf("Child process %d exited with signal %d\n", pid, WTERMSIG(proc_status));
        else if(WIFEXITED(proc_status)) {
            if(!WEXITSTATUS(proc_status))
                printf("Child process %d exited normally\n", pid);
            else
                printf("Child process %d exited with a return value of %d\n", pid, WEXITSTATUS(proc_status));
        }

        printf("Real: %ld.%03lds, ", end.tv_sec -start.tv_sec, (end.tv_usec - start.tv_usec)/1000);
        printf("User: %ld.%03lds, ", proc_usage.ru_utime.tv_sec, proc_usage.ru_utime.tv_usec/1000);
        printf("Sys: %ld.%03lds\n", proc_usage.ru_stime.tv_sec, proc_usage.ru_stime.tv_usec/1000);

        return proc_status;
    }
}

int parser(FILE *std_in)
{
    int last_returned = 0;
    char *line = NULL;
    size_t buf_len = 0;
    //newline character at the end of line;
    while((getline(&line, &buf_len, std_in)) != -1) {
        if (line[0] == '#')
            continue;
        char *word = strtok(line, " \n");
        char args[buf_len][4096];
        char io_args[3][4096];
        int arg_c = 0;
        int io_arg_c = 0;
        while (word != NULL) {
            if (word[0] == '>' || word[0] == '<' || (word[0] == '2' && word[1] == '>')) {
                strcpy(io_args[io_arg_c], word);
                io_arg_c++;
            } else {
                strcpy(args[arg_c], word);
                arg_c++;
            }

            word = strtok(NULL, " \n");
        }

        if (!strcmp(args[0], "cd")) {
            if (arg_c > 1) {
                if(chdir(args[1]) < 0)
                    perror("Error changing directories");
            }
            else {
                if(chdir(getenv("HOME")) < 0)
                    perror("Error changing directories");
            }
        } else if (!strcmp(args[0], "pwd")) {
            char path[4096];
            printf("%s\n", getcwd(path, 4096));
        } else if (!strcmp(args[0], "exit")) {
            if (arg_c > 1) {
                if (!check_if_num(args[1])) {
                    exit(atoi(args[1]));
                } else {
                    fprintf(stderr, "Valid exit code not provided, exiting with -1");
                    exit(-1);
                }

            } else
                exit(last_returned);
        } else {
            char *new_args[arg_c + 1];
            for (int i = 0; i < arg_c; i++) {
                new_args[i] = args[i];
            }
            new_args[arg_c] = NULL;

            char *new_io_args[io_arg_c + 1];
            for (int i = 0; i < io_arg_c; i++) {
                new_io_args[i] = io_args[i];
            }
            new_io_args[io_arg_c] = NULL;

            last_returned = execute_command(new_args, arg_c, new_io_args, io_arg_c);
        }
    }
}

int main(int argc, char **argv) {

    if(argc > 1)
    {
        int fd = check_error(open(argv[1],O_RDONLY),"Error opening script %s", "path", -1);
        check_error(dup2(fd,0), "Error redirecting stdin to %s", argv[1], -1);
        check_error(close(fd), "Error closing file descriptor for %s", argv[1], -1);
    }
    FILE *std_in = fdopen(0, "r");

    if(std_in == NULL)
    {
        perror("Error opening stdin/input");
        return -1;
    }
    else
    {
        parser(std_in);
        return 0;
    }
    //io_redirect(argc, argv);
}