#include <sys/time.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "doorbell.h"
#include "tv_encoder/cs8556.h"


void TVOutInit(void)
{
#ifndef _MSC_VER
    printf("CS8556Initial enter\n");
    CS8556Initial();
#endif

    // TODO: IMPLEMENT
}

void TVoutExit(void)
{
    int i;


    // TODO: IMPLEMENT
}

void RunTVOut(void)
{
     // TODO: IMPLEMENT
}
