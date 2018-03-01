#include <sys/ioctl.h>
#include <sys/socket.h>
#include <pthread.h>
#include <time.h>
#include "lwip/ip.h"
#include "lwip/dns.h"
#include "ite/itp.h"
#include "iniparser/iniparser.h"
#include "config.h"

#define DHCP_TIMEOUT_MSEC       (60 * 1000) //60sec

static bool networkIsReady, networkCallIsReady;
static bool networkServerIsReady;
static int  call_socket;
static bool networkToReset, networkQuit;
static char sync_code[4] = {0x00, 0x00, 0x01, 0xfc};
static CALL_STATE call_state = CALL_IDLE;

#if 0//def CFG_NET_ETHERNET_MULTI_INTERFACE
#define interface_1_gw "192.168.191.254" //connect to lobby
#define interface_2_gw "192.168.190.254" //connect to PC
#endif

static  struct timeval tvStart = {0, 0}, tvEnd = {0, 0};

bool NetworkIsReady(void)
{
    return networkIsReady;
}

bool NetworkServerIsReady(void)
{
    return networkIsReady && networkServerIsReady;
}

static void NetworkCallInit(void)
{
	struct sockaddr_in addr;
	
	call_socket = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
	if(call_socket < 0){
		printf("[NetworkCallInit]sockek create failure\n");
		networkCallIsReady = false;
	}
	else{
		printf("[NetworkCallInit]udp socket create OK!\n");
		networkCallIsReady = true;
	}

	memset(&addr,'\0',sizeof(struct sockaddr_in));
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=htonl(INADDR_ANY);
	addr.sin_port=htons(5060);
	if( bind (call_socket,(struct sockaddr *)&addr, sizeof(struct sockaddr_in)) < 0 ){
		printf("[NetworkCallInit]bind udp  error!\n"); 
	}
	else
		printf("[NetworkCallInit]bind udp ok! \n");
}

CALL_STATE NetworkCheckIncoming(void)
{
	char buf[5] = {0};
	struct sockaddr_in remote_addr;
	int len = sizeof(struct sockaddr_in), ret = 0;
	fd_set readfds;
	struct timeval timeout = {0};

	FD_ZERO(&readfds);
	FD_SET(call_socket,&readfds);
	timeout.tv_sec = 0;
	timeout.tv_usec = 5000;
	select(call_socket+1,&readfds,NULL,NULL,&timeout); 
	if(FD_ISSET(call_socket,&readfds))
	{
		ret = recvfrom(call_socket,buf,sizeof(buf),0,(struct sockaddr*)&remote_addr,&len);
		if(ret > 0 && memcmp(buf, sync_code, 4) == 0)
		{	
			call_state = buf[4];
			printf("call received 0x%x from %s\n", call_state, inet_ntoa(remote_addr.sin_addr));
		}
	}
	return call_state;
}

int NetworkCheckAck(void)
{
	char buf[5] = {0};
	struct sockaddr_in remote_addr;
	int len = sizeof(struct sockaddr_in), ret = 0;
	fd_set readfds;
	struct timeval timeout = {0};
	CALL_STATE state = CALL_IDLE;

	FD_ZERO(&readfds);
	FD_SET(call_socket,&readfds);
	timeout.tv_sec = 0;
	timeout.tv_usec = 100000;
	select(call_socket+1,&readfds,NULL,NULL,&timeout); 
	if(FD_ISSET(call_socket,&readfds))
	{
		ret = recvfrom(call_socket,buf,sizeof(buf),0,(struct sockaddr*)&remote_addr,&len);
		if(ret > 0 && memcmp(buf, sync_code, 4) == 0)
		{	
			state = buf[4];
			printf("call ack received from %s\n", inet_ntoa(remote_addr.sin_addr));
		}
	}
	return (state == CALL_ACK) ? 1 : 0;
}

void NetworkCallSend(CALL_STATE state)
{
	struct sockaddr_in addr;
	int addr_len = sizeof(struct sockaddr_in);
	char call_buf[5] = {0};
	call_state = state;

	memcpy(call_buf, sync_code, 4);
	call_buf[4] = call_state;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(5060);
	addr.sin_addr.s_addr = inet_addr("192.168.190.132");	
	sendto(call_socket, call_buf, sizeof(call_buf), 0, (const struct sockaddr *)&addr, addr_len);
}

void NetworkCallIdle(void)
{
	call_state = CALL_IDLE;
}

static void ResetEthernet(void)
{
	ITPEthernetSetting setting[CFG_NET_ETHERNET_COUNT];
    ITPEthernetInfo info;
    unsigned long mscnt = 0;
    char buf[16];
	int i;
    char* saveptr;

	for (i = 0; i < CFG_NET_ETHERNET_COUNT; i++){
		memset(&setting[i], 0, sizeof (ITPEthernetSetting));

		setting[i].index = i;
	}

    // dhcp
	setting[0].dhcp = 0;

    // autoip
	setting[0].autoip = 0;

    // ipaddr
	strcpy(buf, "192.168.190.66");
	setting[0].ipaddr[0] = atoi(strtok_r(buf, ".", &saveptr));
	setting[0].ipaddr[1] = atoi(strtok_r(NULL, ".", &saveptr));
	setting[0].ipaddr[2] = atoi(strtok_r(NULL, ".", &saveptr));
	setting[0].ipaddr[3] = atoi(strtok_r(NULL, " ", &saveptr));

    // netmask
    strcpy(buf, "255.255.255.0");
	setting[0].netmask[0] = atoi(strtok_r(buf, ".", &saveptr));
	setting[0].netmask[1] = atoi(strtok_r(NULL, ".", &saveptr));
	setting[0].netmask[2] = atoi(strtok_r(NULL, ".", &saveptr));
	setting[0].netmask[3] = atoi(strtok_r(NULL, " ", &saveptr));

    // gateway
	strcpy(buf, "192.168.190.254");
	setting[0].gw[0] = atoi(strtok_r(buf, ".", &saveptr));
	setting[0].gw[1] = atoi(strtok_r(NULL, ".", &saveptr));
	setting[0].gw[2] = atoi(strtok_r(NULL, ".", &saveptr));
	setting[0].gw[3] = atoi(strtok_r(NULL, " ", &saveptr));

	ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_RESET, &setting[CFG_NET_ETHERNET_COUNT - 1]);

	printf("Wait ethernet cable to plugin...\n");
    while (!ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_IS_CONNECTED, NULL))
    {
        sleep(1);
		mscnt += 100;
        putchar('.');
        fflush(stdout);

		if (setting[0].dhcp == true){
			if (mscnt >= DHCP_TIMEOUT_MSEC){
				printf("\nDHCP timeout, use default settings\n");
				setting[0].dhcp = setting[0].autoip = 0;
				ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_RESET, &setting[0]);

				break;
			}
		}
    }

    printf("\nWait DHCP settings");
    while (!ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_IS_AVAIL, NULL))
    {
        usleep(100000);
        mscnt += 100;

        putchar('.');
        fflush(stdout);

        if (mscnt >= DHCP_TIMEOUT_MSEC)
        {
            printf("\nDHCP timeout, use default settings\n");
			setting[0].dhcp = setting[0].autoip = 0;
			ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_RESET, &setting[0]);
            break;
        }
    }
    puts("");

    if (ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_IS_AVAIL, NULL))
    {
        char* ip;

		for(i =0; i<CFG_NET_ETHERNET_COUNT; i++){
	        info.index = i;
        ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_GET_INFO, &info);
        ip = ipaddr_ntoa((const ip_addr_t*)&info.address);

			printf("IP address[%d]: %s \r\n", i, ip);
		}
        networkIsReady = true;
    }
}

static void* NetworkTask(void* arg)
{
    ResetEthernet();

    while (!networkQuit)
    {
        networkIsReady = ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_IS_CONNECTED, NULL);

        if (networkToReset)
        {

            ResetEthernet();
            networkToReset = false;
        }
		if(!networkCallIsReady && networkIsReady)
			NetworkCallInit();

        sleep(1);
    }
    return NULL;
}

void NetworkInit(void)
{
    pthread_t task;
    pthread_attr_t attr;
    int ret = 0;

    networkIsReady          = false;
    networkServerIsReady    = true;

    networkToReset          = false;
    networkQuit             = false;
    call_socket           = -1;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&task, &attr, NetworkTask, NULL);
}

void NetworkExit(void)
{
    networkQuit = true;
}

void NetworkReset(void)
{
    networkToReset  = true;
}

