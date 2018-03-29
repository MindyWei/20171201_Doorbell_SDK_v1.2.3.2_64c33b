#include <assert.h>
#include <stdio.h>
#include <pthread.h>

#include "ite/itp.h"

#include "siproxd.h"

static void* SiproxdTask(void* arg)
{
    static char* args[] =
    {
        "-c",
        " A:\\siproxd",
    #ifndef NDEBUG
        "-d",
        "32"
    #endif
    };

    return (void*) siproxd_main(ITH_COUNT_OF(args), args);
}



void SiproxdInit(void)
{
    pthread_t task;

    pthread_create(&task, NULL, SiproxdTask, NULL);
}


