#include <stdio.h>
#include <fcntl.h>

void main(int argc,char *argv[])
{
	int fd;
	fd = open("/dev/simple_dev",O_RDWR);
	printf("fd is %d\n",fd);
}
