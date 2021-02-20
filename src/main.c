#include "monopoly_bus.h"

void app_main(void)
{
    sendBuff = 12;
    
    printf("Hello \n");

    MNPLY_begin(10000);
    while(1)
    {
        MNPLY_broadcast_listen(sendBuff, &receiveBuff);
        if(receiveBuff == 10 || receiveBuff == 14)
        {
            printf("Hey guys! \n");
        }
        
    }
    
}