

#include <iostream>
#include "v4l2.h"


int main(void) 
{
    
    v4l2 test("/dev/video0");
    test.Open();
    


    return 0;
}
