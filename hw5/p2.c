#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>

int check_error(int n, char *msg, char *syscall, char* file)
{
	if(n < 0)
	{
		if(file == NULL)
		{
			fprintf(stderr, "ERROR: %s\nSyscall failed on: %s\nDoing: %s\n", strerror(errno), syscall, msg);
			exit(-1);
		}
		else
		{
			fprintf(stderr, "ERROR: %s\nSyscall failed on: %s\nDoing: %s\nFailed on: %s\n", strerror(errno), syscall, msg, file);
			exit(-1);
		}
	}
	return n;
}
		
void sig_reporter(int sig)
{
	fprintf(stderr, "Signal %d (%s) received\n", sig, strsignal(sig));
	exit(sig);
}

void test_read(char *map, int fd)
{	
	map[3] = 'a';
	check_error(lseek(fd, 3, SEEK_SET), "Pos to 3", "lseek", "junk.txt");
	char buf[1];
	check_error(read(fd, buf, 1), "Reading one byte after lseek", "read", "junk.txt");
	if('a' == buf[0])
	{
		printf("Test Succeeded\n");
		exit(1);
	}
	else
	{
		printf("Test Failed\n");
		exit(0);
	}
}
	


int main(int argc, char *argv[])
{
	//block all signals
	int i = 1;
	for(i =1; i < 33; i++)
		signal(i, sig_reporter);

	
	char *map;	
	char cur_dir[4096];
	
	if(getcwd(cur_dir, 4096) == NULL)
	{
		perror("Failed getting the current working directory");
		return -1;
	}
	strcat(cur_dir, "/junk.txt");
	
	
	int fd = check_error(open(cur_dir, O_CREAT | O_TRUNC | O_RDWR , 0666), "opening for reading and writing", "open", "junk.txt");
	char buf[4096];
	check_error(write(fd, buf, 4096), "writing a random buffer to file", "write", "junk.txt");
	
	if(argc < 2)
	{
		fprintf(stderr, "Not enough arguments provided\n");
		return -1;
	}
	switch(atoi(argv[1]))
	{
		case 1: 
			if((map = mmap(NULL, 4096, PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED)
			{
				perror("Failed mapping junk.txt to VA space with flag MAP_SHARED and prot PROT_READ");
				return -1;
			}
			printf("map[3]==%c\n", map[3]);
			printf("Writing a 'B'\n");
			map[3] = 'B';
			break;
		case 2: 
			if((map = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED);
			{
				perror("Failed mapping junk.txt to VA space with flag MAP_SHARED and prot PROT_READ");
				//return -1;
			}
			test_read(map, fd);
			break;
		case 3:
			
			if((map = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0)) == MAP_FAILED);
			{
				perror("Failed mapping junk.txt to VA space with flag MAP_SHARED and prot PROT_READ");
				return -1;
			}
			test_read(map, fd);
			break;
		case 4:;
			char new_path[4096];	
			if(getcwd(new_path, 4096) == NULL)
			{
				perror("Failed getting the current working directory");
				return -1;
			}
			strcat(new_path, "/small_file.txt");
			
			int small_fd = check_error(open(new_path, O_CREAT | O_TRUNC | O_RDWR, 0666), "opening for reading and writing with O_CREAT and O_TRUNC", "open", "small_file.txt");
			check_error(write(small_fd, buf, 32), "writing buf to small file", "write", "small_file.txt");
			
			if((map = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED);
			{
				perror("Failed mapping junk.txt to VA space with flag MAP_SHARED and prot PROT_READ");
				return -1;
			}
			map[32] = 'r';
			
			check_error(lseek(small_fd, 16, SEEK_END), "Pos to 16 bytes after the end", "lseek", "small_file.txt");
			check_error(write(small_fd, buf, 1), "writing 1 byte after lseek", "write", "small_file.txt");
			check_error(lseek(small_fd, 32, SEEK_SET), "Pos to 32 bytes", "lseek", "small_file.txt");
			
			char read_in[1];
			check_error(read(small_fd, read_in, 1), "reading one byte to a buffer", "read", "small_file.txt");

			if(read_in[0] == 'r')
			{
				fprintf(stderr, "Test Succeeded\n");
				exit(0);
			}
			else
			{
				fprintf(stderr, "Test Failed\n");
				exit(1);
			}
			break;
		default:
			fprintf(stderr, "Error: Invalid number or not an actual number passed to program");
			return -1;
			
	}
	
}		
		
	
		
	
