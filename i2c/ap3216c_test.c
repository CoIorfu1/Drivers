#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv){
    int fd;
    char buf[6];
    int len;

    fd = open("/dev/ap3216c", O_RDWR);
    if(fd < 0){
        printf("open fail\n");
    }

    len = read(fd, buf, sizeof(buf));
    for(len = 0;len<6;len++) {
        printf("%02x", buf[len]);
    }    
    printf("\n");
    close(fd);
    return 0;
}