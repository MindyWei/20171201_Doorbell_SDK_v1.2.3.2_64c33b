#include <sys/ioctl.h>
#include <assert.h>
#include <mqueue.h>
#include <string.h>
#include <unistd.h>
#include "ite/itp.h"
#include "ctrlboard.h"
#include "scene.h"

#define EXT_MAX_QUEUE_SIZE      8

static mqd_t extInQueue = -1;
static mqd_t extOutQueue = -1;
static pthread_t extInTask;
static pthread_t extOutTask;
static volatile bool extQuit;

static void* ExternalInTask(void* arg)
{
    while (!extQuit)
    {
        ExternalEvent ev;
		char readbuf[4];

#if defined(CFG_UART0_ENABLE) && !defined(CFG_DBG_UART0)
        if (read(ITP_DEVICE_UART0, readbuf, 4))
        {
            switch (readbuf[0])
            {
            case '0':
                ev.type = EXTERNAL_TEST0;
                break;

            case '1':
                ev.type = EXTERNAL_TEST1;
                ev.arg1 = readbuf[1];
                break;

            case '2':
                ev.type = EXTERNAL_TEST2;
                ev.arg1 = readbuf[1];
                ev.arg2 = readbuf[2];
                break;

            case '3':
                ev.type = EXTERNAL_TEST3;
                ev.arg1 = readbuf[1];
                ev.arg2 = readbuf[2];
                ev.arg3 = readbuf[3];
                break;

            case '4':
                ev.type = EXTERNAL_TEST4;
                ev.arg1 = readbuf[1];
                break;

            case '5':
                ev.type = EXTERNAL_TEST5;
                ev.arg1 = readbuf[1];
                break;

            default:
                ev.type = EXTERNAL_SHOW_MSG;
			    //printf("rcv %s\n", readbuf);
			    memset(ev.buf1, 0 , EXTERNAL_BUFFER_SIZE);				
                memcpy(ev.buf1, readbuf, 4);
                break;
            }
            mq_send(extInQueue, (const char*)&ev, sizeof (ExternalEvent), 0);
        }
#endif
        usleep(10000);
    }
    mq_close(extInQueue);
    extInQueue = -1;

    return NULL;
}

static void* ExternalOutTask(void* arg)
{
    while (!extQuit)
    {
        ExternalEvent ev;

        if (mq_receive(extOutQueue, (char*)&ev, sizeof(ExternalEvent), 0) > 0)
        {            
#if defined(CFG_UART0_ENABLE) && !defined(CFG_DBG_UART0)
            char writebuf[4];

            switch (ev.type)
            {
            case EXTERNAL_TEST0:
                writebuf[0] = '0';
				write(ITP_DEVICE_UART0, writebuf, 4);
                break;

            case EXTERNAL_TEST1:
                writebuf[0] = '1';
                writebuf[1] = (char)ev.arg1;
				write(ITP_DEVICE_UART0, writebuf, 4);
                break;

            case EXTERNAL_TEST2:
                writebuf[2] = '2';
                writebuf[1] = (char)ev.arg1;
                writebuf[2] = (char)ev.arg2;
				write(ITP_DEVICE_UART0, writebuf, 4);
                break;

            case EXTERNAL_TEST3:
                writebuf[0] = '3';
                writebuf[1] = (char)ev.arg1;
                writebuf[2] = (char)ev.arg2;
                writebuf[3] = (char)ev.arg3;
				write(ITP_DEVICE_UART0, writebuf, 4);
                break;

            case EXTERNAL_TEST4:
                writebuf[0] = '4';
                writebuf[1] = (char)ev.arg1;
				write(ITP_DEVICE_UART0, writebuf, 4);
                break;

            case EXTERNAL_TEST5:
                writebuf[0] = '5';
                writebuf[1] = (char)ev.arg1;
				write(ITP_DEVICE_UART0, writebuf, 4);
                break;


            case EXTERNAL_SHOW_MSG:
				write(ITP_DEVICE_UART0, ev.buf1, 4);
                //printf("external uart send: %s\n", ev.buf1);
                break;
            }
#endif
        }
    }
    mq_close(extOutQueue);
    extOutQueue = -1;

    return NULL;
}

void ExternalInit(void)
{
    struct mq_attr qattr;
    pthread_attr_t attr;

    qattr.mq_flags = 0;
    qattr.mq_maxmsg = EXT_MAX_QUEUE_SIZE;
    qattr.mq_msgsize = sizeof(ExternalEvent);

    extInQueue = mq_open("external_in", O_CREAT | O_NONBLOCK, 0644, &qattr);
    assert(extInQueue != -1);

    extOutQueue = mq_open("external_out", O_CREAT, 0644, &qattr);
    assert(extOutQueue != -1);

    extQuit = false;

#if defined(CFG_UART0_ENABLE) && !defined(CFG_DBG_UART0)
	itpRegisterDevice(ITP_DEVICE_UART0, &itpDeviceUart0);
    ioctl(ITP_DEVICE_UART0, ITP_IOCTL_INIT, NULL);
    ioctl(ITP_DEVICE_UART0, ITP_IOCTL_RESET, CFG_UART0_BAUDRATE);
#endif

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&extInTask, &attr, ExternalInTask, NULL);
    pthread_create(&extOutTask, &attr, ExternalOutTask, NULL);
}

void ExternalExit(void)
{
    extQuit = true;
}

int ExternalReceive(ExternalEvent* ev)
{
    assert(ev);

    if (extQuit)
        return 0;

    if (mq_receive(extInQueue, (char*)ev, sizeof(ExternalEvent), 0) > 0)
        return 1;

    return 0;
}

int ExternalSend(ExternalEvent* ev)
{
    assert(ev);

    if (extQuit)
        return -1;

    return mq_send(extOutQueue, (char*)ev, sizeof(ExternalEvent), 0);
}
