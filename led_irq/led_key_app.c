#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
	int fd;
	//int val;
	
	/* 1. 判断参数 
	if (argc != 2) 
	{
		printf("Usage: %s <dev>\n", argv[0]);
		return -1;
	}*/

	/* 2. 打开文件 */
	fd = open("/dev/led_key", O_RDWR);
	if (fd == -1)
	{
		printf("can not open file %s\n", argv[1]);
		return -1;
	}
	
	while (1)
	{
		/* code */
	};
	
	
	close(fd);
	
	return 0;
}