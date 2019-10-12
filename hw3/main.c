#include <stdio.h>
#include <unistd.h>
#include <string.h>
<<<<<<< HEAD
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <ctype.h>


int out_fd = 1;
int err_fd = 2;

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
=======


void handle_argv(int argc, char **argv);

int in_fd = 0;
int out_fd = 1;
int err_fd = 2;
void handle_argv(int argc, char **argv)
{
    for(int i = 1; i < argc; i++)
>>>>>>> 6fdac1f53a1dbc1928b49d2b98a5612266e52b52
    {
        if(argv[i][0] == '<') {
            char new_path[4096];
            sprintf(new_path, "./%s", argv[i] + 1);
<<<<<<< HEAD
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
            check_error(dup2(fd, 1), "Error redirecting stderr to %s", argv[i]+1, 1);
            close(fd);
=======
            fopen(new_path, "w");
>>>>>>> 6fdac1f53a1dbc1928b49d2b98a5612266e52b52
        }
    }
}


<<<<<<< HEAD
void execute_command()
{

}


int main(int argc, char **argv) {
    FILE *std_in;
    std_in = fdopen(0, "r");
    int last_returned = 0;
    if(std_in == NULL)
    {
        perror("Error opening stdin");
        return -1;
    }
    else
    {
        char *line = NULL;
        size_t buf_len = 0;
        ssize_t line_len;
        //newline character at the end of line;
        while((line_len = getline(&line, &buf_len, std_in)) != -1)
        {
            if(line[0] == '#')
                continue;
            char *word = strtok(line, " \n");
            char args[buf_len][4096];
            char io_args[3][4096];
            int arg_c = 0;
            int io_arg_c = 0;
            while(word != NULL)
            {
                if(word[0] == '>' || word[0] == '<' || (word[0] == '2' && word[1] == '>'))
                {
                    strcpy(io_args[io_arg_c], word);
                    io_arg_c++;
                } else {
                    strcpy(args[arg_c], word);
                    arg_c ++;
                }

                word = strtok(NULL, " \n");
            }

            for(int i = 0; i < arg_c; i++)
            {
                printf("%s\n", args[i]);
            }

            if(!strcmp(args[0], "cd"))
            {
                if(arg_c > 1)
                    check_error(chdir(args[1]), "Error changing directory to %s", args[1], -1);
                else
                    check_error(chdir("~"), "Error changing directory to %s", "the home directory", -1);
            }
            else if(!strcmp(args[0], "pwd"))
            {
                char path[4096];
                printf("%s\n", getcwd(path, 4096));
            }
            else if(!strcmp(args[0], "exit"))
            {
                if(arg_c > 1) {
                    if(!check_if_num(args[1]))
                    {
                        exit(atoi(args[1]));
                    } else
                    {
                        fprintf(stderr, "Valid exit code not provided, exiting with -1");
                        exit(-1);
                    }

                } else
                    exit(last_returned);
            }
            else
            {
                last_returned = execute_command();
            }
        }
        return 0;
    }
    //io_redirect(argc, argv);
}
=======

int main(int argc, char **argv) {
    handle_argv(argc, argv);
    return 0;
}
>>>>>>> 6fdac1f53a1dbc1928b49d2b98a5612266e52b52
