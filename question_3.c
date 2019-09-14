#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

void check_error(int num, char *file_name, char* op, char* for_what)
{
	if(num < 0)
	{
		fprintf(stderr, "***ERROR***\nError: %s.\nFailed Syscall: %s\nFailed doing: %s\nObject failed: %s\n", strerror(errno), op, for_what, file_name);
		_exit(-1);
	}
}

int main(int argc, char **argv)
{
	int offset = 0;
	char buf[4096];
	int outfile = 1;
	if(argc==1)
	{
		fprintf(stderr, "Error: Not enough arguments provided\n");
		return -1;
	}
	else
	{
		if(!strcmp(argv[1],"-o"))
		{
			if(argc > 2)
			{
				outfile = open(argv[2], O_TRUNC | O_WRONLY | O_CREAT | O_APPEND, 0666);
				check_error(outfile, argv[2], "open", "Opening or creating output file");
				offset = 2;
			}
			else
			{
				fprintf(stderr, "Error: Not enough arguments provided\n");
				return -1;
			}
		}
		for(int i = 1 + offset; i < argc; i++)
		{
			int fd = 0;
			int read_write_count = 0;
			int is_binary = 0;
			
			if(strcmp(argv[i],"-" ))	
			{
				fd = open(argv[i], O_RDONLY);
				check_error(fd, argv[i], "open", "Opening a file for reading");
				read_write_count++;
			}		
			int size_of_file = 0;
			while(1)
			{	
				int cur_read = read(fd, buf, 4096);
				check_error(cur_read, strcmp(argv[i], "-") ? argv[i] : "<standard input>", "read", "Reading into a buffer");
				read_write_count++;
				if(cur_read == 0)
					break;
				if(!is_binary)
				{
					for(int j = 0; j < cur_read; j++)
					{
						if(!(isprint(buf[j])||isspace(buf[j])))
						{
							is_binary = 1;
							break;
						}
					}
				}
				// need to look for partial write p 14	
				check_error(write(outfile, buf,  cur_read), outfile == 1 ? "<standard out>" : argv[2], "write", "Writing to output");
				read_write_count++;
				size_of_file += cur_read;
			}
			if(!fd)
				check_error(close(fd), argv[i], "close", "Closing input file");
			else
				fflush(stdin);

			fprintf(stderr, "\n\nName of file: %s\nBytes transferred: %d\nNumber of read and writes: %d\n", strcmp(argv[i], "-") ? argv[i] : "<standard input>", size_of_file, read_write_count);
			if(is_binary)
				fprintf(stderr, "This is a binary file\n");
		}
	}
	return 0;
}

