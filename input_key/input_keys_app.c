#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <linux/input.h>

//存放输入事件信息
struct input_event keys_input_event;

int main(int argc,char *argv[]){
    int err;
    int fd = open("/dev/input/event1",O_RDONLY);
    if(fd<0){
        printf("open fail\n");
    }
    printf("open ok\n");
    while (1)
    {
        err = read(fd,&keys_input_event,sizeof(keys_input_event));
        if(err <0){
            printf("read fail\n");
        }
        if(err>0){
            switch (keys_input_event.type)
            {
            case EV_KEY:
                printf("key's value = %d\n",keys_input_event.code);
                break;
            
            default:
                break;
            }
        }
    }
    return 0;
    
}
