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
    int fd = open("/dev/input/event2",O_RDONLY);
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
            
            if((keys_input_event.type == EV_KEY) && (keys_input_event.code == KEY_1)){
                if(keys_input_event.value == 0){
                    printf("key 1 up,%d\n",keys_input_event.value);
                }else if(keys_input_event.value > 0){
                    printf("key 1 down,%d\n",keys_input_event.code);
                }
            } 
            if((keys_input_event.type == EV_KEY) && (keys_input_event.code == KEY_4)){
                if(keys_input_event.value == 0){
                    printf("key 4 up,%d\n",keys_input_event.code);
                }else if(keys_input_event.value > 0){
                    printf("key 4 down,%d\n",keys_input_event.code);
                }
            }
        }
    }
    return 0;
    
}
