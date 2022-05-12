#include <stdio.h>
#include <stdint.h>

int main()
{
    struct student{
        int8_t hi;
        int8_t test
    };

    struct student p1 = {20, 20};
    char* buf = (char * ) &p1;
    for(int i = 0; i < sizeof(p1); i++){
        printf("%02x ",*buf++);
    }
    return 0;
};