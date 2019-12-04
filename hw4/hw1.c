#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

//Helper function for checking errors
int check_error(int num, char *file_name, char* op, char* for_what)
{
	if(num < 0)
	{
		fprintf(stderr, "***ERROR***\nError: %s.\nFailed Syscall: %s\nFailed doing: %s\nObject failed: %s\n", strerror(errno), op, for_what, file_name);
		_exit(-1);
	}
	else
		return num;
}

//Function that copies file from input to output
void copy_file(int fd, char *file_name, int read_write_count, int outfile, char *outfile_name)
{
	char buf[4096];
	int size_of_file = 0;
	int is_binary = 0;

	while(1)
	{	
		//Reading the file into the buffer
		int cur_read = check_error(read(fd, buf, 4096), file_name, "read", "Reading into a buffer");
		read_write_count++;

		//If no more left to read, leave loop
		if(cur_read == 0)
			break;

		//Binary file check
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
		size_of_file += cur_read;
		int buf_offset = 0;

		//Partial writes 
		while(cur_read > 0)
		{
			buf_offset = check_error(write(outfile, buf+buf_offset,  cur_read), outfile_name , "write", "Writing to output");
			cur_read -= buf_offset;
			read_write_count++;
		}
	}
	
	//Reporting the file information to standard error
	if(fd >=3)
		check_error(close(fd), file_name, "close", "Closing input file");

	fprintf(stderr, "\n\nName of file: %s\nBytes transferred: %d\nNumber of read and writes: %d\n", file_name, size_of_file, read_write_count);
	if(is_binary)
		fprintf(stderr, "This is a binary file\n");

}

int main(int argc, char **argv)
{
	int offset = 0;
	int outfile = 1;

	//Handles the output flag
	if(argc > 1 && !strcmp(argv[1],"-o"))
	{
		if(argc > 2)
		{
			outfile = open(argv[2], O_TRUNC | O_WRONLY | O_CREAT, 0666);
			check_error(outfile, argv[2], "open", "Opening or creating output file");
			offset = 2;
		}
		else
		{
			fprintf(stderr, "Error: Not enough arguments provided\n");
			return -1;
		}
	}

	//No input files
	if(1+offset == argc)
		copy_file(0, "<standard input>", 0, outfile, offset ? argv[2]: "<standard output>");
	else
	{
		//Loop through all the input files and copy into output fiile
		for(int i = 1 + offset; i < argc ; i++)
		{
			int fd = strcmp(argv[i], "-") ? check_error(open(argv[i], O_RDONLY), argv[i], "open", "Opening a file for reading"): 0;
			copy_file(fd, fd ? argv[i] : "<standard input>", fd ? 1: 0, outfile, offset ? argv[2] : "<standard output>");
		}
	}

	return 0;
}



