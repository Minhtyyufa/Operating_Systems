#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
char to_data[6] = {69, 67, 69, 51, 53, 0};
int to_bss[4097];

main()
{
	to_data[5] = 55;
	int zero = 0;
	mmap(1073745920,4096*3, PROT_READ, MAP_SHARED | MAP_ANON, 0,0);
	return 0;
}
	
	
