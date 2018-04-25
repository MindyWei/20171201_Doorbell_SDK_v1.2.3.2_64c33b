#include "user_function.h"
#include "openrtos/FreeRTOS.h"
#include "openrtos/queue.h"

int uart_mode = UART_START;
bool uart_busy = false;
bool uart_signal_busy_ing = false;
static sem_t UartSem;
bool uart_sent_once = false;
uint8_t will_use_channel = 0;
uint8_t sent_ing_cmd= 0;
uint8_t ID_LINK[4] = {0};
uint8_t INT_LINK[4] = {0};
uint8_t anser_id = 0;
uint8_t inter_number = 0;
uint8_t inter_id = 0;
bool inter_ing = false;
uint8_t receive_receive = 0;


timer_t uart_read_TimerId;		//UART 读取时间
#define	 uart_read_Time		1

#define  CMD_MAX		 7
#define UART_ALL			0xFF
#define UART_MASTER		0x01
#define UART_ID2			0x02
#define UART_ID3			0x03
#define UART_ID4			0x04

char prev_cmd[CMD_MAX] ;
char sent_cmd_p[CMD_MAX] ;

char cmd_str[50][20] =
{
	"CMD_DOOR_1",
	"CMD_DOOR_2",
	"CMD_CCTV_1",
	"CMD_CCTV_2",
	"CMD_RELEASE",
	"CMD_CALL_1",
	"CMD_CALL_2",
	"CMD_CALL_JOIN",
	"CMD_CALL_QUIT",
	"CMD_CALL_OVER",
	"CMD_CALL_ANSWER",
	"CMD_SIGNAL_BUSY",
	"CMD_INTER_CALL",
	"CMD_INTER_JOIN",
	"CMD_INTER_QUIT",
	"CMD_INTER_OVER",
	"CMD_INTER_ANSWER",
	"CMD_DOOR_1_OPEN",
	"CMD_DOOR_2_OPEN",
	"CMD_ERROR",
	"CMD_RETURN",
} ;

bool uart_get_inter_ing()
{
	return inter_ing;
}

char user_uart_read()
{
	char val = 0;
	usleep(1000);
	read(ITP_DEVICE_UART0, &val, 1);
	return val;
}

void user_uart_write(char val)
{
	usleep(1000);
	write(ITP_DEVICE_UART0, &val, 1);
}

void uart_read_start()
{
	struct itimerspec value;

	value.it_value.tv_sec   = uart_read_Time;
	value.it_value.tv_nsec  = 0;
	value.it_interval.tv_sec = value.it_interval.tv_nsec = 0;
	timer_settime(uart_read_TimerId, 0, &value, NULL);
}

void uart_read_end(timer_t timerid, int arg)
{
	uart_mode = UART_START;
}

static void UartCallback(void* arg1, uint32_t arg2)
{
	sem_post(&UartSem);
}

void uart_sent_cmd(char target,char cmd)
{
	int i;
	char sent_cmd[CMD_MAX];

	sent_cmd[0] = 0x36;
	sent_cmd[1] = theConfig.id+1;
	sent_cmd[2] = target;
	sent_cmd[3] = cmd;
	sent_cmd[4] = ((sent_cmd[1]+sent_cmd[2]+sent_cmd[3])&0xFF);
	sent_cmd[5] = 0xFF;
	sent_cmd[6] = 0xFF;
	for(i = 0; i < CMD_MAX; i++)
	{
		sent_cmd_p[i] = sent_cmd[i];
		user_uart_write(sent_cmd[i]);
	}
	printf(">>>>>>>>>>SENT_CMD--->ID: %d-------->CMD: 0x%x------->ID: %d\n",sent_cmd[1],sent_cmd[3],sent_cmd[2]);
	sent_ing_cmd = cmd;
	uart_sent_once = true;
	uart_mode = UART_START;
}

uint8_t uart_get_anser_id()
{
	return anser_id;
}
void uart_set_mode(int mode)
{
	uart_mode = mode;
	return;
}
void uart_set_busy()
{
	uart_signal_busy_ing = true;
}
void uart_clear_busy()
{
	door_call_num = 0;
	uart_signal_busy_ing = false;
}

bool uart_is_busy()
{
	return uart_signal_busy_ing;
}

bool uart_call_no_one()
{
	//printf("ID_LINK---------->%d %d %d %d\n",ID_LINK[0],ID_LINK[1],ID_LINK[2],ID_LINK[3]);
	if(!ID_LINK[0]&&!ID_LINK[1]&&!ID_LINK[2]&&!ID_LINK[3])
		return true;
	else
		return false;
}

void uart_clear_id_link()
{
	ID_LINK[0] = 0;
	ID_LINK[1] = 0;
	ID_LINK[2] = 0;
	ID_LINK[3] = 0;
}

bool uart_inter_no_one()
{
	//printf("ID_LINK---------->%d %d %d %d\n",ID_LINK[0],ID_LINK[1],ID_LINK[2],ID_LINK[3]);
	if(!INT_LINK[0]&&!INT_LINK[1]&&!INT_LINK[2]&&!INT_LINK[3])
		return true;
	else
		return false;
}

void uart_clear_inter_link()
{
	INT_LINK[0] = 0;
	INT_LINK[1] = 0;
	INT_LINK[2] = 0;
	INT_LINK[3] = 0;
}
uint8_t uart_get_inter_id()
{
	return inter_id;
}

void uart_clear_inter_id()
{
	inter_id = 0;
}

bool uart_cmd_process(char uart_cmd[CMD_MAX])					//事件处理
{
	bool cmd_return = true;

	switch(uart_cmd[3])
	{
	case CMD_MONITOR:
		if(signal_insert[DOOR_1])
		{
			event_uart = uart_cmd[3];
			uart_mode = UART_MON_DOOR1;
		}
		else if(signal_insert[DOOR_2])
		{
			event_uart = uart_cmd[3];
			uart_mode = UART_MON_DOOR2;
		}
		else
			uart_mode = UART_MON_NOC;
		
		cmd_return = false;
		break;
	case CMD_MON_DOOR1:
	case CMD_MON_DOOR2:
		event_uart = uart_cmd[3];
		uart_mode = UART_SIGNAL_BUSY_30;
		cmd_return = false;
		break;
		
	case CMD_CCTV:
		if(signal_insert[CCTV_1])
		{
			event_uart = uart_cmd[3];
			uart_mode = UART_MON_CCTV1;
		}
		else if(signal_insert[CCTV_2])
		{
			event_uart = uart_cmd[3];
			uart_mode = UART_MON_CCTV2;
		}
		else
			uart_mode = UART_MON_NOC;
		
		cmd_return = false;
		break;
	case CMD_MON_CCTV1:
	case CMD_MON_CCTV2:
		event_uart = uart_cmd[3];
		uart_mode = UART_SIGNAL_BUSY_60;
		cmd_return = false;
		break;
		
	case CMD_MON_NOC:
		//没有接CAM
		break;
		
	case CMD_SWITCH_1:
	case CMD_SWITCH_2:
	case CMD_SWITCH_3:
	case CMD_SWITCH_4:
		if(uart_cmd[3] == CMD_SWITCH_1 && signal_insert[DOOR_1])
		{
			event_uart = uart_cmd[3];
			uart_mode = UART_SWITCH_OK;
		}
		else if(uart_cmd[3] == CMD_SWITCH_2 && signal_insert[DOOR_2])
		{
			event_uart = uart_cmd[3];
			uart_mode = UART_SWITCH_OK;
		}
		else if(uart_cmd[3] == CMD_SWITCH_3 && signal_insert[CCTV_1])
		{
			event_uart = uart_cmd[3];
			uart_mode = UART_SWITCH_OK;
		}
		else if(uart_cmd[3] == CMD_SWITCH_4 && signal_insert[CCTV_2])
		{
			event_uart = uart_cmd[3];
			uart_mode = UART_SWITCH_OK;
		}
		else
			uart_mode = UART_MON_NOC;
		cmd_return = false;
		break;
	case CMD_SWITCH_OK:
		event_uart = uart_cmd[3];
		break;
		
	case CMD_POWER_ON:	
		if(cur_page == page_monitor || cur_page == page_cctv || cur_page == page_inter_ing || set_display_signal)
		{
			uart_mode = UART_POWER_ON_ACK;
			cmd_return = false;
		}
		break;
		
	case CMD_POWER_ON_ACK:
		power_ack_time_reinit();
		uart_signal_busy_ing = true;
		break;
	case CMD_SIGNAL_NOT_BUSY:
		uart_signal_busy_ing = false;
		event_uart = uart_cmd[3];
		break;
		
	case CMD_RELEASE:
		event_uart = uart_cmd[3];
		break;
	case CMD_CALL_1:
	case CMD_CALL_2:
		event_uart = uart_cmd[3];
		break;
	case CMD_CALL_JOIN:
		if(master_vdp)
		{
			if((format_ing ||delete_ing) && (!ID_LINK[0] && !ID_LINK[1]&& !ID_LINK[2]&& !ID_LINK[3]))	//第一个加入
			{
				ID_LINK[uart_cmd[1]-1]++;
				event_uart = uart_cmd[3];
			}
			else 															//后续加入
				ID_LINK[uart_cmd[1]-1]++;
		}
		break;
	case CMD_CALL_QUIT:
		if(master_vdp)
		{
			ID_LINK[uart_cmd[1]-1]--;
			if(uart_call_no_one())
				event_uart = uart_cmd[3];
		}
		break;
	case CMD_CALL_OVER:
		event_uart = uart_cmd[3];
		break;
	case CMD_CALL_ANSWER:
		other_talk_id = uart_cmd[1];
		event_uart = uart_cmd[3];
		break;
	case CMD_RETURN:
		break;
	case CMD_SIGNAL_BUSY:
		uart_signal_busy_ing = true;
		break;
	case CMD_SIGNAL_BUSY_30:
		busy_30_start();
		uart_signal_busy_ing = true;
		break;
	case CMD_SIGNAL_BUSY_60:
		busy_60_start();
		uart_signal_busy_ing = true;
		break;
		
	case CMD_INTER_CALL:
		inter_id = uart_cmd[1];
		if(uart_cmd[2] & (0x1 << theConfig.id))		//INTERPHONE 被呼叫
		{
			if(format_ing ||delete_ing)
				break;
			event_uart = uart_cmd[3];
			uart_mode = UART_INTER_JOIN;
			cmd_return = false;
		}
		else										//INTERPHONE 未被呼叫
		{
			inter_ing = true;
			if(cur_page  == page_inter)
				event_go_home = true;
		}
		break;
	case CMD_INTER_JOIN:
		if(!INT_LINK[0] && !INT_LINK[1]&& !INT_LINK[2]&& !INT_LINK[3])	//第一个加入
		{
			INT_LINK[uart_cmd[1]-1]++;
			event_uart = uart_cmd[3];
			uart_mode = UART_INTER_ING;
			cmd_return = false;
		}
		else 															//后续加入
			INT_LINK[uart_cmd[1]-1]++;
		break;
	case CMD_INTER_ING:
		uart_set_busy();
		inter_call_start();
		if(0)
		{
			if(montion_enable)      //退出移动侦测
				montion_end();
		}
		break;
	case CMD_INTER_QUIT:
		INT_LINK[uart_cmd[1]-1]--;
		if(uart_inter_no_one())
			event_uart = uart_cmd[3];
		break;
	case CMD_INTER_OVER:
		event_uart = uart_cmd[3];
		break;
	case CMD_INTER_ANSWER:
		anser_id = uart_cmd[1];
		event_uart = uart_cmd[3];
		break;
	case CMD_DOOR_1_OPEN:
		if(signal_insert[DOOR_1])
		{
			event_uart = uart_cmd[3];
		}
		break;
	case CMD_DOOR_2_OPEN:
		if(signal_insert[DOOR_2])
		{
			event_uart = uart_cmd[3];
		}
		break;
	}
	return cmd_return;
}


bool check_cmd_return(char uart_cmd[CMD_MAX])
{
	if(uart_cmd[2] == UART_ALL)
	{
		if(((uart_cmd[3] == CMD_DOOR_1) && signal_insert[DOOR_1])
		        ||((uart_cmd[3] == CMD_DOOR_2) && signal_insert[DOOR_2])
		        ||((uart_cmd[3] == CMD_CCTV_1) && signal_insert[CCTV_1])
		        ||((uart_cmd[3] == CMD_CCTV_2) && signal_insert[CCTV_2]) )
		{
			printf("check_cmd_return----------------------->return\n");
			event_uart = uart_cmd[3];
			return true;
		}
		else if((uart_cmd[3] == CMD_RELEASE) && offer_signal_channel)
		{
			printf("check_cmd_return----------------------->return\n");
			event_uart = uart_cmd[3];
			return true;
		}
		else if(uart_cmd[3] == CMD_CALL_1)
		{
			event_uart = uart_cmd[3];
		}


	}
	else if(uart_cmd[2] == theConfig.id)
	{

	}
	return false;
	//uart_mode = UART_START;
}
//#define uart_test
void _user_uart_init()
{
	char uart_cmd[CMD_MAX];
	char temp_val ;
	char temp_val_1 = 0x36;
	char temp_cmd = 0x36;
	int i;
	unsigned temp_rand = 0;

	while(1)
	{
#ifdef uart_test
		//user_uart_write(0x77);
		temp_val = user_uart_read();
		if(temp_val)
			printf("------------->%x\n",temp_val);
#else
		switch(uart_mode)
		{
		case UART_START:
			temp_val = user_uart_read();
			//if(temp_val)
			//	printf("------------->%x\n",temp_val);
			if(temp_val == 0x36)
			{
				uart_mode = UART_RECEIVE;
				uart_read_start();
				uart_cmd[0] = 0x36;
				i = 1;
			}
			break;
		case UART_RECEIVE:
			temp_val = user_uart_read();
			//if(temp_val)
			//printf("------------->%x\n",temp_val);
			if(temp_val == 0x36)
			{
				uart_cmd[0] = 0x36;
				i = 1;
			}
			else if(temp_val)
			{
				uart_cmd[i] = temp_val;
				i++;
			}
			if(i == CMD_MAX)
			{
				if(uart_cmd[4] == ((uart_cmd[1]+uart_cmd[2]+uart_cmd[3])&0xFF))
				{
					if(uart_cmd[3] == sent_ing_cmd)					//接收到自己发的命令不做处理
					{
						uart_mode = UART_START;
						sent_ing_cmd = 0;
					}
					else												//接收命令处理
					{
						if(uart_cmd[2] == UART_ALL || uart_cmd[2] == (theConfig.id+1) ||uart_cmd[3] == CMD_INTER_CALL)
						{
							printf("<<<<<<<<<<RECE_CMD<---ID: %d<---------CMD: 0x%x<-------ID: %d\n",uart_cmd[2],uart_cmd[3],uart_cmd[1]);
							for(i = 0; i<CMD_MAX; i++)
								prev_cmd[i] = uart_cmd[i];
							if(uart_cmd_process(uart_cmd))
								uart_mode = UART_START;
						}
						else
							uart_mode = UART_START;
					}
				}
				else
					uart_mode = UART_START;
			}
			break;
		case UART_MONITOR:
			uart_sent_cmd(UART_MASTER,CMD_MONITOR);
			break;
		case UART_MON_DOOR1:
			uart_sent_cmd(prev_cmd[1],CMD_MON_DOOR1);
			break;
		case UART_MON_DOOR2:
			uart_sent_cmd(prev_cmd[1],CMD_MON_DOOR2);
			break;
		case UART_CCTV:
			uart_sent_cmd(UART_MASTER,CMD_CCTV);
			break;
		case UART_MON_CCTV1:
			uart_sent_cmd(prev_cmd[1],CMD_MON_CCTV1);
			break;
		case UART_MON_CCTV2:
			uart_sent_cmd(prev_cmd[1],CMD_MON_CCTV2);
			break;
		case UART_MON_NOC:
			uart_sent_cmd(prev_cmd[1],CMD_MON_NOC);
			break;
			
		case UART_SWITCH_1:
			uart_sent_cmd(UART_MASTER,CMD_SWITCH_1);
			break;
		case UART_SWITCH_2:
			uart_sent_cmd(UART_MASTER,CMD_SWITCH_2);
			break;
		case UART_SWITCH_3:
			uart_sent_cmd(UART_MASTER,CMD_SWITCH_3);
			break;
		case UART_SWITCH_4:
			uart_sent_cmd(UART_MASTER,CMD_SWITCH_4);
			break;
		case UART_SWITCH_OK:
			uart_sent_cmd(prev_cmd[1],CMD_SWITCH_OK);
			break;
		case UART_POWER_ON:
			if(theConfig.id == 0)
				power_delay_start(5*100*1000);
			else
				power_delay_start((theConfig.id+1)*100*1000);
			uart_mode = UART_START;
			break;
		case UART_POWER_ON_D:
			uart_sent_cmd(UART_ALL,CMD_POWER_ON);
			power_ack_time_start();
			break;
		case UART_POWER_ON_ACK:
			uart_sent_cmd(prev_cmd[1],CMD_POWER_ON_ACK);
			break;
		case UART_SIGNAL_NOT_BUSY:
			uart_sent_cmd(UART_ALL,CMD_SIGNAL_NOT_BUSY);
			break;
			
		case UART_RELEASE:									//释放 命令
			uart_signal_busy_ing = false;
			uart_sent_cmd(UART_ALL,CMD_RELEASE);
			break;
		case UART_CALL_1:									//CALL  命令
			uart_sent_cmd(UART_ALL,CMD_CALL_1);
			break;
		case UART_CALL_2:									//CALL  命令
			uart_sent_cmd(UART_ALL,CMD_CALL_2);
			break;
		case UART_CALL_JOIN:									//CALL  加入
#if 0
			srand(SDL_GetTicks());
			temp_rand = (rand()%20+1)*100*1000;
			printf("temp_rand------------------>%ld\n",temp_rand);
			call_delay_start(temp_rand);
#else
			if(theConfig.id == 0)
				call_delay_start(5*100*1000);
			else
				call_delay_start((theConfig.id+1)*100*1000);
#endif
			uart_mode = UART_START;
			break;
		case UART_CALL_JOIN_D:									//CALL  加入
			uart_sent_cmd(UART_ALL,CMD_CALL_JOIN);
			break;
		case UART_CALL_QUIT:									//CALL  退出
			uart_sent_cmd(UART_ALL,CMD_CALL_QUIT);
			break;
		case UART_CALL_OVER:									//CALL  结束
			uart_sent_cmd(UART_ALL,CMD_CALL_OVER);
			break;
		case UART_CALL_ANSWER:								//CALL  接听命令
			uart_sent_cmd(UART_ALL,CMD_CALL_ANSWER);
			break;
		case UART_SIGNAL_BUSY:								//信号忙命令
			uart_sent_cmd(UART_ALL,CMD_SIGNAL_BUSY);
			break;
		case UART_SIGNAL_BUSY_30:								//信号忙命令
			uart_sent_cmd(UART_ALL,CMD_SIGNAL_BUSY_30);
			break;
		case UART_SIGNAL_BUSY_60:								//信号忙命令
			uart_sent_cmd(UART_ALL,CMD_SIGNAL_BUSY_60);
			break;
			
		case UART_INTER_CALL:								//CALL  接听命令
			uart_sent_cmd(interphone_number,CMD_INTER_CALL);
			break;
		case UART_INTER_JOIN:									//CALL  加入
#if 0
			srand((unsigned)time(NULL));
			temp_rand = (rand()%20+1)*100*1000;
			printf("temp_rand------------------>%ld\n",temp_rand);
			inter_delay_start(temp_rand);
#else
			if(theConfig.id == 0)
				inter_delay_start(5*100*1000);
			else
				inter_delay_start((theConfig.id+1)*100*1000);
#endif
			uart_mode = UART_START;
			break;
		case UART_INTER_JOIN_D:									//CALL  加入
			uart_sent_cmd(inter_id,CMD_INTER_JOIN);
			break;
		case UART_INTER_ING:									//CALL  加入
			uart_sent_cmd(UART_ALL,CMD_INTER_ING);
			break;
		case UART_INTER_QUIT:									//CALL  退出
			uart_sent_cmd(inter_id,CMD_INTER_QUIT);
			break;
		case UART_INTER_OVER:									//CALL  结束
			uart_sent_cmd(UART_ALL,CMD_INTER_OVER);
			break;
		case UART_INTER_ANSWER:								//CALL  接听命令
			uart_sent_cmd(UART_ALL,CMD_INTER_ANSWER);
			break;
		case UART_DOOR_1_OPEN:								//开锁
			uart_sent_cmd(UART_ALL,CMD_DOOR_1_OPEN);
			break;
		case UART_DOOR_2_OPEN:								//开锁
			uart_sent_cmd(UART_ALL,CMD_DOOR_2_OPEN);
			break;
		}
#endif
		usleep(1000);
	}
}
void user_uart_init()
{
	sem_init(&UartSem, 0, 0);
	itpRegisterDevice(ITP_DEVICE_UART0, &itpDeviceUart0);
	ioctl(ITP_DEVICE_UART0, ITP_IOCTL_INIT, NULL);
	ioctl(ITP_DEVICE_UART0, ITP_IOCTL_RESET, CFG_UART0_BAUDRATE);
	ioctl(ITP_DEVICE_UART0, ITP_IOCTL_REG_UART_CB, (void*)UartCallback);

	timer_create(CLOCK_REALTIME, NULL, &uart_read_TimerId);
	timer_connect(uart_read_TimerId, uart_read_end, 0);

	pthread_t user_uart_p;			//线程ID
	pthread_attr_t user_uart_pat;
	pthread_attr_init(&user_uart_p);
	pthread_attr_setdetachstate(&user_uart_pat, PTHREAD_CREATE_DETACHED);
	pthread_create(&user_uart_p,NULL, _user_uart_init, NULL);
}










