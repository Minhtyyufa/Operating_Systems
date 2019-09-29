#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <stdlib.h>

int found;
long dev_major;
long dev_minor;
int mtime;
int mtime_flag;
int only_vol;
void explore_dir(char *path);

int main(int argc, char **argv)
{
	int opt;
	while((opt= getopt(argc,argv, "mv:")) != -1)
	{
		switch (opt)
		{
			case 'm':
				if(optarg)
				{
					mtime = atoi(optarg);
					mtime_flag = 1;
				}
				else
				{
					printf("Error: Not enough arguments provided\n");
					return -1;
				}
				break;
			case 'v':
				only_vol = 1;
				break;
			default:
				printf("Error: Invalid flag\n");
				return -1;
		}	
	}
	explore_dir(argv[1]);	
}


void explore_dir(char *path)
{
	struct dirent *dirent_pt;
	DIR *dir_pt;
	dir_pt = opendir(path);
	
	if(!found && (dir_pt != NULL))
	{
		struct stat first_stat;
		if(lstat(path, &first_stat) >= 0)
		{
			dev_major = major(first_stat.st_dev);
			dev_minor = minor(first_stat.st_dev);
		}
		found = 0; 
	}

	if (dir_pt == NULL)
	{
		
		struct stat file_stat;
		if(lstat(path, &file_stat) >= 0)
		{
			// need read write perms
			struct passwd *pwd; 
			pwd = getpwuid(file_stat.st_uid);
			struct group *gp = getgrgid(file_stat.st_gid);	
			
			printf("%ld\t%lld\t",(long) file_stat.st_ino, (long long) file_stat.st_blocks/2);
			printf("%ld\t", (long) file_stat.st_nlink);
			if(pwd != NULL)
				printf("%s\t", pwd->pw_name);
			else
				printf("%ld\t", file_stat.st_uid);
			if(gp != NULL)
				printf("%s\t", gp->gr_name);
			else
				printf("%ld\t", file_stat.st_gid);
			if ((file_stat.st_mode & S_IFMT) != S_IFBLK || (file_stat.st_mode & S_IFMT) != S_IFCHR)
				printf("%ld\t", file_stat.st_size);
			else
				printf("%d,%d\t", major(file_stat.st_rdev), minor(file_stat.st_rdev));
			
			char date_str[4096];
			struct tm lt;
			localtime_r(&file_stat.st_mtime, &lt);
			strftime(date_str, sizeof(date_str),"%c",&lt);
			printf("%s\t",date_str);

			printf("%s", path);

			if((file_stat.st_mode & S_IFMT) == S_IFLNK)
			{
				char link_name[4096];
				if(readlink(path, link_name, 4096)<0)
					return;
				else
				printf(" -> %s", link_name);
			}

			printf("\n");
			
		}
		else
			perror("lstat");
		return;
		// list file stuff here
	}
	else
	{
		
		while((dirent_pt = readdir(dir_pt)) != NULL)
		{
			if(strcmp(dirent_pt->d_name,".") && strcmp(dirent_pt->d_name,".."))	
			{
				char new_path[4096];
				strcpy(new_path, path);
				strcat(new_path, "/");
				strcat(new_path, dirent_pt->d_name);
				//printf("%s\n", new_path);
				explore_dir(new_path);		
			}
		}
	}
}
