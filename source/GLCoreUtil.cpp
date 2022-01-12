#include "GLCoreUtil.h"
#include <stdlib.h>
#include <time.h> 
float GLCoreUtil::randomFloat()
{
    srand((unsigned)time(NULL));
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}