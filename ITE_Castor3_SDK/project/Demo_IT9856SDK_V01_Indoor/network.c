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
#include "thread_helper.h"
#include "wifiMgr.h"
#ifdef CFG_NET_FTP_SERVER
#include <unistd.h>
#include "ftpd.h"
#endif



//#define TEST_WIFI_DOWNLOAD

#ifdef TEST_WIFI_DOWNLOAD
#include "curl/curl.h"
#define HTTP_DOWNLOAD_ADDRESS  "http://192.168.191.100:23455/WAV/002-96000-24.wav"
#endif

//#define TEST_HTTPS_DOWNLOAD

#ifdef TEST_HTTPS_DOWNLOAD
#include "curl/curl.h"

static CURL *gpCurl;
static CURLM *gpCurlm;
static char *AL_uri = NULL;
static char gHeader[2048];
static int gCurlHandle=0;

static char gCurlTempBuffer[96*1024];
static int AL_File_Size = 0;   // size
static int    gCurlBufferSize=0;
#define AUDIOLINK_CONTENT_LENGTHS "Content-Length:"
#define CURL_READ_SIZE 64*1024
#define HTTPS_DOWNLOAD_ADDRESS  "https://telecom.taiwanmobile.com/emyfone/content/action/upload/HPS_Hendset_Phone/h01001101131500_7vdr_gd_1_2.jpg"

struct httpBuf
{
    uint8_t* buf;
    uint32_t pos;
};


#endif

#define DHCP_TIMEOUT_MSEC     (60 * 1000)   //60sec
#define HEARTBEAT_TIMEOUT_SEC (30 * 60)

static bool   networkIsReady, networkServerIsReady;
static bool   networkToReset, networkQuit;
static bool   networkHeartbeatIsReady;
static time_t networkLastRecvTime;
static int    networkSocket;
static ITPEthernetInfo networkInfo;
static ITPEthernetSetting settingCopy;


#ifdef CFG_NET_WIFI
static WIFI_MGR_SETTING gWifiSetting;
static int              gInit = 0; // wifi init
#endif
static struct timeval   tvStart = {0, 0}, tvEnd = {0, 0};


#ifdef TEST_WIFI_DOWNLOAD
static int gnTest =-1;

struct FtpBuf
{
    uint8_t* buf;
    uint32_t pos;
};

static size_t FtpWrite(void *buffer, size_t size, size_t nmemb, void *stream)
{
    struct FtpBuf* out = (struct FtpBuf*)stream;
    size_t s;
/*
    LOG_DEBUG "FtpWrite(0x%X,%d,%d,0x%X)\n", buffer, size, nmemb, stream LOG_END
*/
    // assert(out->buf);

    s = size * nmemb;
    memcpy(&out->buf[out->pos], buffer, s);
    out->pos += s;
    if (out->pos>510000){

        printf("FtpWrite(0x%X,%d,%d,0x%X)\n, buffer, size, nmemb, stream");
        out->pos = 0;
    }

    return s;
}

static int  httpDownload()
{
    CURL *curl;
    CURLcode res;
    struct FtpBuf ftpBuf;

    //InitNetwork();
    printf("[httpDownload] download : start %s\r\n");

    ftpBuf.buf = malloc(0x200000);
    ftpBuf.pos = 0;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (!curl) {
        printf("curl_easy_init() fail.\n");
        goto error;
    }

    curl_easy_setopt(curl, CURLOPT_URL, HTTP_DOWNLOAD_ADDRESS);
    //curl_easy_setopt(curl, CURLOPT_USERPWD, FTP_USER_NAME_PASSWORD);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, FtpWrite);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ftpBuf);

#ifndef NDEBUG
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
#endif

    res = curl_easy_perform(curl);

    /* always cleanup */
    curl_easy_cleanup(curl);

    if (CURLE_OK != res)
    {
        printf("curl fail: %d\n", res);
        goto error;
    }

    curl_global_cleanup();

    if(ftpBuf.buf){
        free(ftpBuf.buf);
        ftpBuf.buf = NULL;
    }
    return NULL;


    error:
    curl_global_cleanup();
    if(ftpBuf.buf){
        free(ftpBuf.buf);
        ftpBuf.buf = NULL;
    }
    return NULL;

}

static int TestHTTPDownload()
{
    int nRet;
    int bIsAvail;
    do {
    nRet = wifiMgr_is_wifi_available(&bIsAvail);
    if (bIsAvail){
        httpDownload();
    }
    usleep(2000000);
    usleep(2000000);
    printf("ready to download \n\n");
    usleep(2000000);
    } while (1);

}

static int createHttpThread()
{
    pthread_t task;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize(&attr, (255 * 1024));
    pthread_create(&task, &attr, TestHTTPDownload, NULL);
    return 0;

}


#endif


#ifdef TEST_HTTPS_DOWNLOAD

// callback when data received
static size_t CurlHttpsWrite(void *buffer, size_t size, size_t nmemb, void *stream)
{
    struct httpBuf* out = (struct httpBuf*)stream;
    size_t s;

    printf("[CurlHttps] CurlHttpsWrite 0x%X,%d,%d,0x%X \n",buffer, size, nmemb, stream);
#if 0
    if (fout==NULL && (fout= fopen(outFileName, "wb")) == NULL){
        printf("[Audiolink] Can not open '%s' #line %d \n",outFileName,__LINE__);
    }
    fwrite(buffer,size,nmemb,fout);
#endif
    s = size * nmemb;
#if 1
    memcpy(&out->buf[out->pos], buffer, s);
    out->pos += s;
#else
    if (gCurlBufferSize>=0){
    memcpy(&gCurlTempBuffer[gCurlBufferSize],buffer,s);
    gCurlBufferSize+=s;
    } else {
        printf("[Audiolink] AudiolinkCurlHttpsWrite gCurlBufferSize %d  < 0 , #line %d \n",gCurlBufferSize,__LINE__);
    }

    do{
        if (gCurlBufferSize>CURL_READ_SIZE){
            usleep(20000);
        } else {

        }

    }while (gCurlBufferSize>=CURL_READ_SIZE);
#endif
    return s;
}

// callback when data received
static size_t CurlHttpsGetHeader(void *buffer, size_t size, size_t nmemb, void *stream)
{
    size_t s;
    char cTemp[128];
    char cLength[128];
    int nLength;
    char * pch= NULL;

//    printf("[CurlHttps]CurlHttpsGetHeader 0x%X,%d,%d,0x%X \n",buffer, size, nmemb, stream);
//    printf("[%s]\n",buffer);

    if (strstr(buffer,AUDIOLINK_CONTENT_LENGTHS)){
        pch =strstr(buffer,AUDIOLINK_CONTENT_LENGTHS);
        sscanf(pch+16, "%d", &nLength);
        if (nLength >=0 ){
            printf("[CurlHttps] CurlHttpsGetHeader content length %d #line %d \n",nLength,__LINE__);
            AL_File_Size = nLength;
        }
    }
    s = size * nmemb;
    return s;
}


int CurlGetHttpsExample(void)
{
    CURLcode res;
    struct httpBuf tBuf;

    printf("CurlGetHttpsExample: start\r\n");

    tBuf.buf = malloc(64*1024);//(CFG_RAM_SIZE / 2);
    tBuf.pos = 0;

    gpCurl = curl_easy_init();
    if (!gpCurl){
        printf("curl_easy_init() fail.\n");
        goto error;
    }

    gpCurlm = curl_multi_init();
    if (!gpCurlm){
        printf("curl_multi_init() fail.\n");
        goto error;
    }

    curl_easy_setopt(gpCurl, CURLOPT_URL, HTTPS_DOWNLOAD_ADDRESS);

    /* disable progress meter, set to 0L to enable and disable debug output */
    curl_easy_setopt(gpCurl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(gpCurl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(gpCurl, CURLOPT_SSL_VERIFYHOST, 0L);

    // get header
    curl_easy_setopt(gpCurl, CURLOPT_HEADERFUNCTION, CurlHttpsGetHeader);
    curl_easy_setopt(gpCurl, CURLOPT_HEADERDATA, gHeader);

    /* send all data to this function  */
    curl_easy_setopt(gpCurl, CURLOPT_WRITEFUNCTION, CurlHttpsWrite);
    curl_easy_setopt(gpCurl, CURLOPT_WRITEDATA, &tBuf);

/*
    curl_easy_setopt(gpCurl, CURLOPT_SEEKFUNCTION, seek_callback);
    curl_easy_setopt(gpCurl, CURLOPT_SEEKDATA, &tBuf);
    fseek(&tBuf, 64*1024*8, SEEK_SET);
*/

    curl_easy_setopt(gpCurl, CURLOPT_BUFFERSIZE, CURL_READ_SIZE);

    curl_easy_setopt(gpCurl, CURLOPT_VERBOSE, 1L);

    curl_multi_add_handle(gpCurlm, gpCurl);
    printf("curl_multi_perform \n");

    do{
        usleep(2000);
        res = curl_multi_perform(gpCurlm,&gCurlHandle);
        if (CURLE_OK != res) {
            printf("curl fail: %d \n",res);
            break;
        }

    } while (gCurlHandle!=0);


    printf("[CurlGetHttpsExample] curl_multi_perform close #line %d \n",__LINE__);


    if (CURLE_OK != res) {
        printf("curl fail: %d \n",res);
        goto error;
    }

    printf("curl size %d \n",tBuf.pos);
    //fclose(fout);


error:
     curl_multi_cleanup(gpCurlm);

    /* always cleanup */
    curl_easy_cleanup(gpCurl);

    if(tBuf.buf){
        memset(tBuf.buf,0,sizeof(64*1024));
        free(tBuf.buf);
        tBuf.buf = NULL;
    }


    printf("[CurlGetHttpsExample] CurlGetSoundCloud close #line %d \n",__LINE__);

    usleep(2000);
    memset(gCurlTempBuffer,0,sizeof(gCurlTempBuffer));
    gCurlBufferSize = 0;


    return 0;

}

#endif



bool NetworkIsReady(void)
{
    return networkIsReady;
}

bool NetworkServerIsReady(void)
{
    return networkIsReady && networkServerIsReady;
}

/* Dynamic address change to Static address. */
void NetworkDynamicToStatic(void)
{
	//DHCP is enabled last time, but DHCP is disabled now.
	if ((theConfig.dhcp == 0) && (theConfig.dhcp_ipaddr != NULL)){

		if (!strcmp(theConfig.default_ipaddr, theConfig.ipaddr) ||
				!strcmp(theConfig.dhcp_ipaddr, theConfig.ipaddr))
			strcpy(theConfig.ipaddr, theConfig.default_ipaddr);

	}
}

void ResetAddr(uint8_t* eth_setting, char* config_addr){
	char *saveptr, *pToken = NULL;
	int count = 0;

	pToken = strtok_r(config_addr, ".", &saveptr);
	eth_setting[count] = atoi(pToken);

	while(count < 3){
		count++;

		pToken = strtok_r(NULL, ".", &saveptr);
		eth_setting[count] = atoi(pToken);
	}
}

static void NetworkMultiNetif(ITPEthernetSetting* setting)
{
    char buf[16];
/*---------------------Interface_0 setting---------------------*/

#ifdef CFG_NET_ETHERNET_MULTI_INTERFACE_DHCP
    setting[0].dhcp   = theConfig.dhcp;
#else
    setting[0].dhcp   = 0;
#endif

    setting[0].autoip = 0;

	strcpy(buf, theConfig.ipaddr_0);
    ResetAddr(&setting[0].ipaddr,  &buf);

    strcpy(buf, theConfig.netmask_0);
	ResetAddr(&setting[0].netmask, &buf);

	strcpy(buf, theConfig.gw_0);
	ResetAddr(&setting[0].gw,      &buf);

	ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_RESET, &setting[0]);


/*---------------------Interface_Main setting------------------*/

    setting[1].dhcp   = 0; //Use static IP only by multi-netif mode

    setting[1].autoip = 0;

	strcpy(buf, theConfig.ipaddr);
    ResetAddr(&setting[1].ipaddr,  &buf);

    strcpy(buf, theConfig.netmask);
    ResetAddr(&setting[1].netmask, &buf);

	strcpy(buf, theConfig.gw);
    ResetAddr(&setting[1].gw,      &buf);

	ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_RESET, &setting[1]);
}

static void ResetEthernet(void)
{
    ITPEthernetSetting setting[CFG_NET_ETHERNET_COUNT];
    unsigned long      mscnt = 0;
	char               buf[16];
    int                i;

	for (i = 0; i < CFG_NET_ETHERNET_COUNT; i++){
		memset(&setting[i], 0, sizeof (ITPEthernetSetting));

        setting[i].index = i;
    }

#ifdef CFG_NET_ETHERNET_MULTI_INTERFACE
	NetworkMultiNetif(&setting);
#else //default ethernet netif set

#ifndef CFG_NET_ETHERNET_WIFI
	if (!theConfig.dhcp)
		NetworkDynamicToStatic();
#endif

    // dhcp
    setting[0].dhcp   = theConfig.dhcp;

    // autoip
    setting[0].autoip = 0;

    // ipaddr
    strcpy(buf, theConfig.ipaddr);
    ResetAddr(&setting[0].ipaddr,   &buf);

    // netmask
    strcpy(buf, theConfig.netmask);
    ResetAddr(&setting[0].netmask,  &buf);

    // gateway
    strcpy(buf, theConfig.gw);
    ResetAddr(&setting[0].gw,       &buf);

	if (theConfig.dhcp)
		theConfig.dhcp_ipaddr_writen = false;

	sleep(1); //wait for eXosip_init ready

    ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_RESET, &setting[CFG_NET_ETHERNET_COUNT - 1]);
#endif

    printf("Wait ethernet cable to plugin...\n");
    while (!ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_IS_CONNECTED, NULL))
    {
        sleep(1);
        mscnt += 100;
        putchar('.');
        fflush(stdout);

		/*When DHCP is setting enable but cable is disconnect, use default IP*/
        if (setting[CFG_NET_ETHERNET_COUNT - 1].dhcp == true)
        {
            if (mscnt >= DHCP_TIMEOUT_MSEC)
            {
                printf("\nDHCP timeout, cable no plugin, use default settings\n");
                setting[CFG_NET_ETHERNET_COUNT - 1].dhcp = setting[CFG_NET_ETHERNET_COUNT - 1].autoip = 0;
                ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_RESET, &setting[CFG_NET_ETHERNET_COUNT - 1]);

                break;
            }
        }
    }

#if !defined(WIN32)
	/* Send ARP to detect IP duplication. */
	settingCopy = setting[CFG_NET_ETHERNET_COUNT - 1];
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
            setting[CFG_NET_ETHERNET_COUNT - 1].dhcp = setting[CFG_NET_ETHERNET_COUNT - 1].autoip = 0;
            ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_RESET, &setting[CFG_NET_ETHERNET_COUNT - 1]);
            break;
        }
    }
    puts("");

    if (ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_IS_AVAIL, NULL))
    {
        char *ip;

        for (i = 0; i < CFG_NET_ETHERNET_COUNT; i++)
        {
            networkInfo.index = i;
            ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_GET_INFO, &networkInfo);
            ip         = ipaddr_ntoa((const ip_addr_t *)&networkInfo.address);

            printf("IP address[%d]: %s\n", i, ip);
        }

        if (setting[0].dhcp)
            DeviceInfoInitByDhcp(ip);

        networkIsReady = true;
    }

}

#ifdef CFG_NET_WIFI
static int wifiCallbackFucntion(int nState)
{
    switch (nState)
    {
    case WIFIMGR_STATE_CALLBACK_CONNECTION_FINISH:
        printf("[Indoor]WifiCallback connection finish \n");
#if defined (CFG_NET_ETHERNET_WIFI)
        ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_RESET_DEFAULT, NULL);
#ifdef TEST_WIFI_DOWNLOAD
        createHttpThread();
#endif
#else
        WebServerInit();
#ifdef CFG_NET_FTP_SERVER
        ftpd_setlogin(theConfig.user_id, theConfig.user_password);
        ftpd_init();
#endif
#endif
        break;

    case WIFIMGR_STATE_CALLBACK_CLIENT_MODE_DISCONNECT_30S:
        printf("[Indoor]WifiCallback connection disconnect 30s \n");
        break;

    case WIFIMGR_STATE_CALLBACK_CLIENT_MODE_RECONNECTION:
        printf("[Indoor]WifiCallback connection reconnection \n");
        break;

    case WIFIMGR_STATE_CALLBACK_CLIENT_MODE_TEMP_DISCONNECT:
        printf("[Indoor]WifiCallback connection temp disconnect \n");
        break;

    case WIFIMGR_STATE_CALLBACK_CLIENT_MODE_CONNECTING_FAIL:
        printf("[Indoor]WifiCallback connecting fail, please check ssid,password,secmode \n");
        break;

    default:
        printf("[Indoor]WifiCallback unknown %d state  \n", nState);
        break;
    }
}

static ResetWifi()
{
//    ITPEthernetSetting setting;
    char buf[16], *saveptr;

    memset(&gWifiSetting.setting, 0, sizeof(ITPEthernetSetting));

    gWifiSetting.setting.index  = 0;

    // dhcp
    gWifiSetting.setting.dhcp   = 1; //Always open DHCP when WIFI enable


    // autoip
    gWifiSetting.setting.autoip = 0;

    // ipaddr
    strcpy(buf, theConfig.ipaddr);
	ResetAddr(&gWifiSetting.setting.ipaddr,   &buf);

    // netmask
    strcpy(buf, theConfig.netmask);
    ResetAddr(&gWifiSetting.setting.netmask,  &buf);

    // gateway
    strcpy(buf, theConfig.gw);
    ResetAddr(&gWifiSetting.setting.gw,       &buf);
}

#endif

#ifdef CFG_UDP_HEARTBEAT

static void NetworkInitHeartbeat(void)
{
    struct sockaddr_in sockAddr;
    int                val = 1;

    networkSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (networkSocket == -1)
        return;

    ioctlsocket(networkSocket, FIONBIO, &val);

    memset((char *)&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_family      = AF_INET;
    sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    sockAddr.sin_port        = htons(CFG_UDP_HEARTBEAT_PORT);

    if (bind(networkSocket, (struct sockaddr *)&sockAddr, sizeof(sockAddr)) == -1)
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
#ifdef CFG_NET_WIFI
    int nTemp;
#endif

#ifdef CFG_NET_ETHERNET
    ResetEthernet();
#endif

#if defined(CFG_NET_FTP_SERVER) && !defined(CFG_NET_WIFI)
	ftpd_setlogin(theConfig.user_id, theConfig.user_password);
	ftpd_init();
#endif

#ifdef TEST_HTTPS_DOWNLOAD
    CurlGetHttpsExample();
#endif

    if (ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_IS_AVAIL, NULL))
    {
    #if CFG_SNTP_UPDATE_DELAY > 0
        {
            DeviceInfo managerInfoArray[MAX_MANAGER_COUNT];
            int        i, count = AddressBookGetManagerInfoArray(managerInfoArray, MAX_MANAGER_COUNT);

            for (i = 0; i < count; i++)
                sntp_add_server_address(managerInfoArray[i].ip);

            if (theCenterInfo.ip[0] != '\0')
                sntp_add_server_address(theCenterInfo.ip);

            sntp_set_update_delay(CFG_SNTP_UPDATE_DELAY);
            sntp_set_port(CFG_SNTP_PORT);
            sntp_init();
        }
    #endif     // CFG_SNTP_UPDATE_DELAY > 0

        if (theConfig.area[0] && theConfig.building[0] && theConfig.unit[0] && theConfig.floor[0] && theConfig.room[0] && theConfig.ext[0])
            EventWriteRegisterDevice(networkInfo.hardwareAddress);

        // sync do not disturb configuration with extension devices
        if (strcmp(theDeviceInfo.ext, "01"))
            RemoteGetDontDisturbConfig();
        else
            RemoteSetDontDisturbConfig();
    }

#if !defined(WIN32)
	if (ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_IP_DUPLICATE, NULL)){
		settingCopy.dhcp = 1;
		ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_RESET, &settingCopy);
	}
#endif

    while (!networkQuit)
    {
#ifdef CFG_NET_WIFI
        gettimeofday(&tvEnd, NULL);

        nTemp = itpTimevalDiff(&tvStart, &tvEnd);
        if (nTemp > 5000 && gInit == 0)
        {
            printf("init wifi \n");
#ifdef CFG_NET_WIFI_HOSTAPD
            nTemp = wifiMgr_init(WIFIMGR_MODE_SOFTAP, 0, gWifiSetting);
#else
            nTemp = wifiMgr_init(WIFIMGR_MODE_CLIENT, 0, gWifiSetting);
#endif
            gInit = 1;
        }
        else if (gInit == 1)
        {
            networkIsReady = wifiMgr_is_wifi_available(&nTemp);
            networkIsReady = (bool)nTemp;
        }
#endif


#ifdef CFG_NET_ETHERNET
        networkIsReady = ioctl(ITP_DEVICE_ETHERNET, ITP_IOCTL_IS_CONNECTED, NULL);
#endif
        // check center server connection
        if (networkIsReady)
        {
            time_t             now  = time(NULL);
            struct sockaddr_in sockAddr;
            int                slen = sizeof(sockAddr);

#ifdef CFG_UDP_HEARTBEAT
            int                count, diff;
            char               buf[4];

            if (!networkHeartbeatIsReady)
                NetworkInitHeartbeat();

            // check center is alive or not
            diff = (int) difftime(now, networkLastRecvTime);
            if (diff >= HEARTBEAT_TIMEOUT_SEC)
            {
                networkServerIsReady = false;
            }

            count = recvfrom(networkSocket, buf, 1, 0, (struct sockaddr *)&sockAddr, &slen);
            if (count > 0)
            {
                networkServerIsReady = true;
                networkLastRecvTime  = now;

                // send heartbeat packet back to center
                //sockAddr.sin_family = AF_INET;
                //sockAddr.sin_addr.s_addr = inet_addr(theCenterInfo.ip);

                count = sendto(networkSocket, "*", 1, 0, (const struct sockaddr *)&sockAddr, sizeof(sockAddr));
            }
#endif         // CFG_UDP_HEARTBEAT
        }

        if (networkToReset)
        {
#ifdef CFG_NET_ETHERNET
            ResetEthernet();
#endif

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
    int        i, count = AddressBookGetManagerInfoArray(managerInfoArray, MAX_MANAGER_COUNT);

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
    int            ret = 0;

    networkIsReady          = false;
    networkServerIsReady    = true;
#ifdef CFG_UDP_HEARTBEAT
    networkHeartbeatIsReady = false;
#endif
    networkToReset          = false;
    networkQuit             = false;
    networkSocket           = -1;
    networkLastRecvTime     = time(NULL);

#ifdef CFG_NET_WIFI
    snprintf(gWifiSetting.ssid,     WIFI_SSID_MAXLEN,     theConfig.ssid);
    snprintf(gWifiSetting.password, WIFI_PASSWORD_MAXLEN, theConfig.password);
    snprintf(gWifiSetting.secumode, WIFI_SECUMODE_MAXLEN, theConfig.secumode);
    gWifiSetting.wifiCallback = wifiCallbackFucntion;
    ResetWifi();

    WifiMgr_clientMode_switch(theConfig.wifi_status);

    gettimeofday(&tvStart, NULL);
#endif
    CreateWorkerThread(NetworkTask, NULL);
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
    networkToReset = true;
}

