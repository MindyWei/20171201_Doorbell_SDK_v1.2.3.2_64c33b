#include <sys/ioctl.h>
#include <sys/socket.h>
#include <pthread.h>
#include <time.h>
#include "lwip/ip.h"
#include "lwip/dns.h"
#include "ite/itp.h"
#include "iniparser/iniparser.h"
#include "sntp.h"
#include "doorbell.h"

#define DHCP_TIMEOUT_MSEC (60 * 1000) //60sec

static bool networkIsReady;
static bool networkHeartbeatIsReady;
static int networkSocket;
static bool networkToReset, networkQuit;
static ITPEthernetSetting settingCopy;

bool NetworkIsReady(void)
{
    return networkIsReady;
}

static void ResetEthernet(void)
{
    ITPEthernetSetting setting;
    ITPEthernetInfo info;
    unsigned long mscnt = 0;
    char buf[16], *saveptr;

    memset(&setting, 0, sizeof (ITPEthernetSetting));

    setting.index = 0;

    // dhcp
    setting.dhcp = theConfig.dhcp;

    // autoip
    setting.autoip = 0;

    // ipaddr
    strcpy(buf, theConfig.ipaddr);
    setting.ipaddr[0] = atoi(strtok_r(buf, ".", &saveptr));
    setting.ipaddr[1] = atoi(strtok_r(NULL, ".", &saveptr));
    setting.ipaddr[2] = atoi(strtok_r(NULL, ".", &saveptr));
    setting.ipaddr[3] = atoi(strtok_r(NULL, " ", &saveptr));

    // netmask
    strcpy(buf, theConfig.netmask);
    setting.netmask[0] = atoi(strtok_r(buf, ".", &saveptr));
    setting.netmask[1] = atoi(strtok_r(NULL, ".", &saveptr));
    setting.netmask[2] = atoi(strtok_r(NULL, ".", &saveptr));
    setting.netmask[3] = atoi(strtok_r(NULL, " ", &saveptr));

    // gateway
    strcpy(buf, theConfig.gw);
    setting.gw[0] = atoi(strtok_r(buf, ".", &saveptr));
    setting.gw[1] = atoi(strtok_r(NULL, ".", &saveptr));
    setting.gw[2] = atoi(strtok_r(NULL, ".", &saveptr));
    setting.gw[3] = atoi(strtok_r(NULL, " ", &saveptr));

    ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_RESET, &setting);

    printf("Wait ethernet cable to plugin");
    while (!ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_IS_CONNECTED, NULL))
    {
        sleep(1);
        putchar('.');
        fflush(stdout);
    }

#if !defined(WIN32)
	/* Send ARP to detect IP duplication. */
	settingCopy = setting;
	ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_ARP_REPLY, &settingCopy);
#endif

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
            setting.dhcp = setting.autoip = 0;
            ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_RESET, &setting);
            break;
        }
    }
    puts("");

    if (ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_IS_AVAIL, NULL))
    {
        char* ip;

        info.index = 0;
        ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_GET_INFO, &info);
        ip = ipaddr_ntoa((const ip_addr_t*)&info.address);

        printf("IP address: %s\n", ip);

    #if CFG_SNTP_UPDATE_DELAY > 0
        {
            DeviceInfo managerInfoArray[MAX_MANAGER_COUNT];
            int i, count = AddressBookGetManagerInfoArray(managerInfoArray, MAX_MANAGER_COUNT);

            for (i = 0; i < count; i++)
                sntp_add_server_address(managerInfoArray[i].ip);

            if (theCenterInfo.ip[0] != '\0')
                sntp_add_server_address(theCenterInfo.ip);

            sntp_set_update_delay(CFG_SNTP_UPDATE_DELAY);
            sntp_set_port(CFG_SNTP_PORT);
            sntp_init();
        }
    #endif // CFG_SNTP_UPDATE_DELAY > 0

        if (setting.dhcp)
            DeviceInfoInitByDhcp(ip);

        if (theConfig.local_pos[0] && theConfig.local_ext[0])
            EventWriteRegisterDevice(info.hardwareAddress);

        networkIsReady = true;
    }
}

#ifdef CFG_UDP_HEARTBEAT

static void NetworkInitHeartbeat(void)
{
    struct sockaddr_in sockAddr;
    int val = 1;

    networkSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (networkSocket == -1)
        return;

	ioctlsocket(networkSocket, FIONBIO, &val);

    memset((char*)&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    sockAddr.sin_port = htons(CFG_UDP_HEARTBEAT_PORT);

    if (bind(networkSocket, (struct sockaddr*)&sockAddr, sizeof(sockAddr)) == -1)
    {
        closesocket(networkSocket);
        networkSocket = -1;
        return;
    }
    networkHeartbeatIsReady = true;
}

static void NetworkResetHeartbeat(void)
{
    if (networkHeartbeatIsReady && networkSocket != -1)
    {
        closesocket(networkSocket);
        networkSocket = -1;
    }
    NetworkInitHeartbeat();
}
#endif // CFG_UDP_HEARTBEAT

static void* NetworkTask(void* arg)
{
    ResetEthernet();

#if !defined(WIN32)
	if (ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_IP_DUPLICATE, NULL)){
		settingCopy.dhcp = 1;
		ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_RESET, &settingCopy);
	}
#endif

    while (!networkQuit)
    {
        networkIsReady = ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_IS_CONNECTED, NULL);

        // check center server connection
        if (networkIsReady)
        {
            time_t now = time(NULL);
            struct sockaddr_in sockAddr;
            int slen = sizeof(sockAddr);

        #ifdef CFG_UDP_HEARTBEAT
            int count;
            char buf[4];

            if (!networkHeartbeatIsReady)
                NetworkInitHeartbeat();

            count = recvfrom(networkSocket, buf, 1, 0, (struct sockaddr*)&sockAddr, &slen);
            if (count > 0)
            {
                // send heartbeat packet to center
                //sockAddr.sin_family = AF_INET;
                //sockAddr.sin_addr.s_addr = inet_addr(theCenterInfo.ip);

                count = sendto(networkSocket, "*", 1, 0, (const struct sockaddr*)&sockAddr, sizeof (sockAddr));
            }
        #endif // CFG_UDP_HEARTBEAT
        }

        if (networkToReset)
        {
            ResetEthernet();
        #ifdef CFG_UDP_HEARTBEAT
            NetworkResetHeartbeat();
        #endif
            networkToReset = false;
        }

        sleep(1);
    }
    return NULL;
}

void NetworkSntpUpdate(void)
{
#if CFG_SNTP_UPDATE_DELAY > 0

    DeviceInfo managerInfoArray[MAX_MANAGER_COUNT];
    int i, count = AddressBookGetManagerInfoArray(managerInfoArray, MAX_MANAGER_COUNT);

    sntp_clear_server_addresses();

    for (i = 0; i < count; i++)
        sntp_add_server_address(managerInfoArray[i].ip);

    if (theCenterInfo.ip[0] != '\0')
        sntp_add_server_address(theCenterInfo.ip);

    sntp_update();
#endif // CFG_SNTP_UPDATE_DELAY > 0
}

void NetworkInit(void)
{
    pthread_t task;
    pthread_attr_t attr;

    networkIsReady = false;

#ifdef CFG_UDP_HEARTBEAT
    networkHeartbeatIsReady = false;
#endif
    networkToReset          = false;
    networkQuit             = false;
    networkSocket = -1;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&task, &attr, NetworkTask, NULL);
}

void NetworkExit(void)
{
    networkQuit = true;

#if CFG_SNTP_UPDATE_DELAY > 0
    sntp_deinit();
#endif
}

void NetworkReset(void)
{
    networkToReset  = true;
}
