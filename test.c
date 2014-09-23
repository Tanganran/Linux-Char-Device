#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>



void main(int argc,char *argv[])
{
	int fd;
	int i = 0;
	unsigned char *map_buf;
	fd = open("/dev/simple_dev",O_RDWR);
	printf("fd is %d\n",fd);
	map_buf = mmap(NULL,200,PROT_READ,MAP_SHARED,fd,0);
	printf("mmapbuf 0x%08x\n",map_buf);
	for(i=0;i<100;i++)
	{
		printf("%02x-",map_buf[i]);
	}
	printf("\n");
}
