#include <stdio.h>
#include <unistd.h>
#include <string.h>


void handle_argv(int argc, char **argv);

int in_fd = 0;
int out_fd = 1;
int err_fd = 2;
void handle_argv(int argc, char **argv)
{
    for(int i = 1; i < argc; i++)
    {
        if(argv[i][0] == '<') {
            char new_path[4096];
            sprintf(new_path, "./%s", argv[i] + 1);
            fopen(new_path, "w");
        }
    }
}



int main(int argc, char **argv) {
    handle_argv(argc, argv);
    return 0;
}
