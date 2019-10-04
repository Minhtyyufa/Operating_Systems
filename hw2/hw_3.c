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
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>

int found;
long dev_major;
long dev_minor;
int mtime;
int mtime_flag;
int only_vol;

int user_flag;
struct passwd *u_pw;
int num_groups;
struct group *u_gr;
gid_t *groups;

int check_if_num(char* word);
int get_perm(mode_t mode, struct passwd *f_pw, struct group *f_gr);
void explore_dir(char *path);


int main(int argc, char **argv)
{
	int opt;
	while((opt= getopt(argc,argv, "vm:u:")) != -1)
        {
		switch (opt)
		{
			case 'm':
				if(optarg)
				{
					if(check_if_num(optarg) ==-1)
					{
						fprintf(stderr,"Error: Argument for -m is not a number\n");
						return -1;
					else
					{
						mtime = atoi(optarg);
						mtime_flag = 1;
					}
				}
				else
				{
					fprintf(stderr, "Error: Not enough arguments provided\n");
					return -1;
				}
				break;
			case 'v':
				only_vol = 1;
				break;
			case 'u':
				if(optarg)
				{
					user_flag = 1;
					u_pw = getpwnam(optarg);
					if (u_pw ==NULL)
					{
						if(check_if_num(optarg) == 0)
							u_pw =getpwuid(atoi(optarg));
						else
						{	
							fprintf(stderr, "Error: No such user\n");
							return -1;
						}
					}
					printf("%ld\n",u_pw->pw_uid);

					getgrouplist(u_pw->pw_name,u_pw->pw_gid, NULL, &num_groups); 
					groups = malloc(num_groups * sizeof(gid_t));
					if(groups == NULL)
					{
						perror("Failed malloc'ing groups");
						return -1;
					}
						
					if(getgrouplist(u_pw->pw_name,u_pw->pw_gid, groups, &num_groups) == -1)
					{
						perror("Failed getting groups");
						return -1;
					}
				}
				else
				{
					fprintf(stderr, "Error: Not enough arguments provided\n");
					return -1;
				}
				break;
			default:
				fprintf(stderr,"Error: Invalid flag\n");
				return -1;
				break;
		}	
	}
	if(optind == argc)
	{
		fprintf(stderr, "Error: Not enough arguments provided\n");
		return -1;
	}
	else
		explore_dir(argv[optind]);	
	return 0;

}

int check_if_num(char* word)
{
	int i = 0;
	char c = word[0];
	for(i = 1; c != '\0'; i++)
	{
		if(!isdigit(c))
			return -1;
		c = word[i];
	}
	return 0;
}
		 

int get_perm(mode_t mode,struct passwd *f_pw, struct group *f_gr )
{
	char buf[11];
	strcpy(buf, "\0");
	switch (S_IFMT & mode)
	{
		case S_IFSOCK:
			strcat(buf, "s");
			break;
		case S_IFLNK:
			strcat(buf, "l");
			break;
		case S_IFREG:
			strcat(buf,"-");
			break;
		case S_IFBLK:
			strcat(buf,"b");
			break;
		case S_IFDIR:
			strcat(buf,"d");
			break;
		case S_IFCHR:
			strcat(buf,"c");
			break;
		case S_IFIFO:
			strcat(buf,"p");
			break;
		default:
			fprintf(stderr, "Error: Unrecognized file type\n");
			return;
			break;
	}
	
	strcat(buf, (mode & S_IRUSR) ? "r": "-");
	strcat(buf, (mode & S_IWUSR) ? "w": "-");
	if(mode & S_ISUID)
		strcat(buf, (mode & S_IXUSR) ? "s": "S");
	else
		strcat(buf, (mode & S_IXUSR) ? "x": "-");

	strcat(buf, (mode & S_IRGRP) ? "r": "-");
	strcat(buf, (mode & S_IWGRP) ? "w": "-");
	if(mode & S_ISGID)
		strcat(buf, (mode & S_IXGRP) ? "s": "S");
	else
		strcat(buf, (mode & S_IXGRP) ? "x": "-");

	strcat(buf, (mode & S_IROTH) ? "r": "-");
	strcat(buf, (mode & S_IWOTH) ? "w": "-");
	if(mode & S_ISVTX)
		strcat(buf, (mode & S_IXOTH) ? "t": "T");
	else
		strcat(buf, (mode & S_IXOTH) ? "x": "-");
	
	if(user_flag)
	{
		if(f_pw->pw_uid==u_pw->pw_uid)
		{
			if(buf[1] != 'r')
				return -1;
			else
			{
				printf("%s\t", buf);
				return 0;
			}
		}
		int i =0;
		for(i = 0; i < num_groups; i++)
		{
			u_gr = getgrgid(groups[i]);
				if(u_gr !=NULL && u_gr == f_gr)
				{
					if(buf[4] != 'r')
						return -1;
					else
					{
						printf("%s\t", buf);
						return 0;
					}
				}	
		}
		if(buf[7] != 'r')
			return -1;
		else
		{	
			printf("%s\t", buf);	
			return 0;
		}
	}
	else
	{
		printf("%s\t", buf);	
		return;
	}

}




void explore_dir(char *path)
{
	struct dirent *dirent_pt;
	DIR *dir_pt;
	dir_pt = opendir(path);
	if(dir_pt == NULL)
	{
		if(errno == EACCES)
			fprintf(stderr, "Can't open directory %s\n", path);
		else if(errno != ENOTDIR)
		{
			fprintf(stderr, "Error opening %s: %s\n", path, strerror(errno));
			exit(-1);
		}
	}
	struct stat file_stat;
	if(lstat(path, &file_stat) >= 0)
	{
		if(!found)
		{
			struct stat first_stat;
			if(lstat(path, &first_stat) >= 0)
			{
				dev_major = major(first_stat.st_dev);
				dev_minor = minor(first_stat.st_dev);
			}
			found = 1; 
		}
		if(mtime_flag)
		{
			if(mtime > 0 && (difftime(time(0),file_stat.st_mtime) <= mtime))
				return;
			else if(mtime < 0 && (difftime(time(0),file_stat.st_mtime) >= -mtime))
				return;					
		}	
		
		struct passwd *pwd; 
		pwd = getpwuid(file_stat.st_uid);
		struct group *gp = getgrgid(file_stat.st_gid);	
		//printf("%lo\t", (unsigned long) file_stat.st_mode);
		if(get_perm(file_stat.st_mode, pwd, gp) == -1)
			return;
		// need read write perms
		
		printf("%ld\t%lld\t",(long) file_stat.st_ino, (long long) (file_stat.st_blocks/2 + file_stat.st_blocks%2));
		printf("%ld\t", (long) file_stat.st_nlink);

		if(pwd != NULL)
			printf("%s\t", pwd->pw_name);
		else
			printf("%ld\t", file_stat.st_uid);
		
		if(gp != NULL)
			printf("%s\t", gp->gr_name);
		else
			printf("%ld\t", file_stat.st_gid);

		if ((file_stat.st_mode & S_IFMT) != S_IFBLK && (file_stat.st_mode & S_IFMT) != S_IFCHR)
			printf("%ld\t", file_stat.st_size);
		else
			printf("%ld,%ld\t", (long) major(file_stat.st_rdev),(long) minor(file_stat.st_rdev));
		
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
			

		if(only_vol && (major(file_stat.st_dev)!= dev_major || minor(file_stat.st_dev)!=dev_minor))
		{
			fprintf(stderr,"note: not crossing mount point at %s\n", path);
			return;
		}   
	}
	else
	{
		if(errno != EACCES)
		{
			perror("Error");
			exit(-1);
		}
	}

		
	if (dir_pt != NULL)	
	{
		
		while((dirent_pt = readdir(dir_pt)) != NULL)
		{
			if(strcmp(dirent_pt->d_name,".") && strcmp(dirent_pt->d_name,".."))	
			{
				char new_path[4096];
				strcpy(new_path, path);
				strcat(new_path, "/");
				strcat(new_path, dirent_pt->d_name);
				explore_dir(new_path);		
			}
		}
		if(closedir(dir_pt) < 0)
		{
			fprintf(stderr, "Error closing %s: %s", path, strerror(errno));
			exit(-1);
		}

	}
	return;
}
