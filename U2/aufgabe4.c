#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
int copy(char *source, char *target)
{
	int fd = open(source, O_RDWR);
	int fd2 = open(target, O_CREAT,0777);
  	int clos1 = close( fd2);
	int fd3 = open(target, O_RDWR);
  	int seek = lseek( fd, 0L,SEEK_END);
  	int seek2 = lseek( fd, 0,SEEK_SET);
	char * buffer = (char*) malloc(seek*sizeof(char));
	int h = read(fd, buffer, seek);
	int wr = write(fd3, buffer, seek);
	free (buffer);
 	int clos = close(fd);
 	int clos2 = close(fd3);
	return 0;
}

int main(){
	char name[] ="ahmed.txt";
	copy("text.txt",name);
	return 0;
}