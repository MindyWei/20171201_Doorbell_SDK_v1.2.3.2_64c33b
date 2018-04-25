#ifndef USER_CAPTURE_H
#define USER_CAPTURE_H

//#define TARGET_BOARD_S
#define TARGET_BOARD_G
//#define TARGET_BOARD_G_V03

typedef enum SEND_STATE_TAG
{
    SEND_BEGIN,
    SEND_START,
    SEND_STOP,
    SEND_TERMINATE,
    SEND_IDLE,
} SEND_STATE;

typedef enum CAP_YUV_MODE_TAG
{
    CAP_YUV_NONE,
    CAP_YUV_420,
    CAP_YUV_422,
} CAP_YUV_MODE;

#if defined(TARGET_BOARD_S)
#define DETECT_TYPE			0		//call����ⷽʽ 0:�ж�	1:�߳�
#define DOOR_MAX			2		//���������

#define DOOR_1_ON			31		//�����1 ����GPIO
#define DOOR_2_ON			30		//�����2 ����GPIO
#define DOOR_3_ON			0		//�����3 ����GPIO
#define DOOR_4_ON			0		//�����4 ����GPIO

#define DOOR_1_CALL			27		//�����1 ���GPIO
#define DOOR_2_CALL			28		//�����2 ���GPIO
#define DOOR_3_CALL			0		//�����3 ���GPIO
#define DOOR_4_CALL			0		//�����4���GPIO

#define CD4051_A				91
#define CD4051_B				92

#define DOOR_OPEN			100		//���������GPIO	
#define DOOR_1_OPEN		71	
#define DOOR_2_OPEN		70
#define DOOR_HOOK			90

#define ON		0
#define CALL		1

#endif
#endif
