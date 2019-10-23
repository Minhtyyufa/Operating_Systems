#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <limits.h>

int script_fd = 0;

int check_if_num(char* word);
int check_error(int num, char *fail_msg, char *what, int exit_stat);
void parser(FILE *std_in);

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
    //If there are more than 3 io arguments it uses the most recent one per fd, which I believe is what bash does.
    //If we want to limit the # of io_args to 3, I wouldj uncomment this code.
    /*
    if(argc > 3) {
        fprintf(stderr, "Error: Too many io args provided");
        exit(1);
    }
     */
    for(int i = 0; i < argc; i++)
    {
        if(argv[i][0] == '<') {
            char new_path[PATH_MAX];
            sprintf(new_path, "./%s", argv[i] + 1);
            int fd = check_error(open(new_path, O_RDONLY), "Error opening %s to redirect stdin", argv[i]+1, 1);
            check_error(dup2(fd, 0), "Error redirecting stdin to %s", argv[i]+1, 1);
            close(fd);

        }
        else if(argv[i][0] == '>')
        {
            int fd;
            char new_path[PATH_MAX];
            if(argv[i][1] == '>')
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
            char new_path[PATH_MAX];
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
        else
        {
            fprintf(stderr, "Program Error: a non-io arg was passed to io-arg");
            exit(1);
        }
    }
}


int execute_command(char **argv, int argc, char **io_args, int io_argc)
{
    struct timeval start,end, real;
    gettimeofday(&start, NULL);
    int pid = check_error(fork(), "Error %s", "forking", 0);
    if(pid == 0)
    {
        if(script_fd != 0)
            close(script_fd);
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
        else if(WIFSTOPPED(proc_status))
            printf("Child process %d was stopped by signal %d\n", pid, WSTOPSIG(proc_status));
        else if(WIFCONTINUED(proc_status))
            printf("Child process %d was continued\n", pid);

        timersub(&end, &start, &real);
        printf("Real: %ld.%03lds, ", real.tv_sec, (real.tv_usec)/1000);
        printf("User: %ld.%03lds, ", proc_usage.ru_utime.tv_sec, proc_usage.ru_utime.tv_usec/1000);
        printf("Sys: %ld.%03lds\n", proc_usage.ru_stime.tv_sec, proc_usage.ru_stime.tv_usec/1000);

        return proc_status;
    }
}

void parser(FILE *in_file)
{
    int last_returned = 0;
    char *line = NULL;
    size_t buf_len = 0;

    //newline character at the end of line;
    while((getline(&line, &buf_len, in_file)) != -1) {
        if (line[0] == '#')
            continue;
        if(buf_len > 4096) {
            fprintf(stderr, "Error: Line too long, continuing to next line");
            continue;
        }
        char *word = strtok(line, " \n");
        char args[buf_len][buf_len];
        char io_args[buf_len][buf_len];
        int arg_c = 0;
        int io_arg_c = 0;
        while (word != NULL) {
            if (word[0] == '>' || word[0] == '<' || (word[0] == '2' && word[1] == '>')) {

                strcpy(io_args[io_arg_c], word);
                io_arg_c++;
            }
            else {
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
        }
        else if (!strcmp(args[0], "pwd")) {
            char path[PATH_MAX];
            if(getcwd(path, PATH_MAX) == NULL)
                perror("Error with getcwd, moving on to next command");
            else
                printf("%s\n", path);
        }
        else if (!strcmp(args[0], "exit")) {
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
        script_fd = check_error(open(argv[1],O_RDONLY),"Error opening script %s", "path", -1);

    FILE *in_file = fdopen(script_fd, "r");
    if(in_file == NULL)
    {
        if(script_fd == 0)
            perror("Error opening stdin");
        else
            perror("Error opening script");
        return -1;
    }
    else
    {
        parser(in_file);
        return 0;
    }
}