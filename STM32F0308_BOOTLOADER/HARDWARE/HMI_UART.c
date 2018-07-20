
#include "common.h"
/*
触摸屏相关数据
获取触摸屏的按钮按下状态
*/

#define SAVE_ADDR 0x08007C00
#define INIT_VOL *(vu16*)SAVE_ADDR
//#define USE_STMFLASH

static u16 g_Volume=50;//音量大小
static u8 mutex_status=0;//全局 获取显示音量和静音需要使用到
static u8 paly_status=0;//播放状态切换
static ifs_stmr_handle store_handle;
static ifs_stmr_handle play_handle;
static u8 speed=4;
const char* PLAYSPEED[9]={"<<X16","<<X8 ","<<X4 ","<<X2 ","",">>X2 ",">>X4 ",">>X8 ",">>X16"};

struct _myfile
{
	u32 nframe;
	u32 frameall;
	u8 frame_rate;
	u8 time_minutes;
	u8 time_second;

}MY_FILE_STRUCT;

typedef struct
{
	uint8_t ID;//在HMI中自定义数据
	uint8_t *objname;
	uint8_t event;/*触发它的事件是按下还是弹起*/
}KEY_OP_TYPE;

//播放[暂停]/停止/快进+/快退-/上一曲/下一曲/静音/音量+/音量-/

//串口屏发送指令
void HMI_SendData(u8 *str)
{
	Send_String(str);
	Send_Byte(0xff);
	Send_Byte(0xff);
	Send_Byte(0xff);
}
/**************************************
存储音量值到flash,用定时器10s后触发一次
**************************************/
void Store_Volume(void* value)
{
	#ifdef USE_STMFLASH
	u16 Volume_t=*(u16*)value;
	if(Volume_t!=INIT_VOL)
	{
		STMFLASH_Write(SAVE_ADDR,&Volume_t,1);
	}
	#endif
}
/************************
从flash恢复中恢复音量值
*************************/
void Resume_Volume(void* arg)
{
	#ifdef USE_STMFLASH
	if(INIT_VOL>90)//读取上一次断电前保存的值
	{
		u16 Init_Volume=50;
		STMFLASH_Write(SAVE_ADDR,&Init_Volume,1);
	}
	g_Volume=INIT_VOL;
	_SetVolume(g_Volume);
	#endif
}
/*****************************
UI显示部分
*****************************/

/***********************
控制播放暂停
**********************/
static void UI_Control_PlayPause(u8 PlayingStatues)
{
	u8 str[32];
	if(PlayingStatues)//全局
	{
		//HMI_SendData("tm0.en=1");//启动HMI时间计数器 会自动获取当前状态
		snprintf((char*)str,31,"b3.picc=1");
		//ifs_stmr_start(play_handle);
		_SetProgramStatus(0);
	}
	else
	{
		HMI_SendData("tm0.en=0");
		snprintf((char*)str,31,"b3.picc=0");
		ifs_stmr_stop(play_handle);
		_SetProgramStatus(1);
	}
	HMI_SendData(str);//切换图标
	speed=4;//播放速度恢复到初始正常状态
	_SetPlaySpeed(speed);
}
/***********************
控制播放快进
**********************/
static void UI_Control_FastPreview(void)
{
	u8 str[32];
	
	speed++;
	if(speed>7)
		speed=4;
	snprintf((char*)str,31,"t0.txt=\"%s\"",PLAYSPEED[speed]);
	HMI_SendData(str);
	HMI_SendData("click t0,1");
	_SetPlaySpeed(speed);
}
/***********************
控制播放快退
**********************/
static void UI_Control_ForwordPreview(void)
{
	u8 str[32];	
	if(speed>1)
		speed--;
	else
		speed=4;
	
	snprintf((char*)str,31,"t0.txt=\"%s\"",PLAYSPEED[speed]);
	HMI_SendData(str);
	HMI_SendData("click t0,1");
	_SetPlaySpeed(speed);
}
static void UI_Disp_TimeDetail(void)//获取文件播放信息
{
	u8 str[32];

	u32 time_all=MY_FILE_STRUCT.frameall/MY_FILE_STRUCT.frame_rate;
	if(time_all>=100*60) time_all=99*60+59;

	MY_FILE_STRUCT.time_minutes=time_all/60;
	MY_FILE_STRUCT.time_second=time_all%60;

	snprintf((char*)str,31,"time_all.val=%ld",time_all);
	HMI_SendData(str);
	snprintf((char*)str,31,"n2.val=%d",MY_FILE_STRUCT.time_minutes);
	HMI_SendData(str);
	snprintf((char*)str,31,"n3.val=%d",MY_FILE_STRUCT.time_second);
	HMI_SendData(str);
	snprintf((char*)str,31,"framerate.val=%d",MY_FILE_STRUCT.frame_rate);
	HMI_SendData(str);

}
/***********************
显示总播放时间
**********************/
static void UI_PlayerRun(void* arg)
{
	u8 str[32];
	struct _myfile *movie=(struct _myfile *)arg;
	
	movie->nframe+=30;
	if(movie->nframe>movie->frameall)
	{
		movie->nframe=0;
	}
	snprintf((char*)str,31,"n_frame.val=%ld",movie->nframe);
	HMI_SendData(str);
}
/***********************
显示音量图标
**********************/
static void UI_Disp_Volume(u8 num)
{
	u8 str[32];
	if(mutex_status)
			num=0;
	snprintf((char*)str,31,"volume.val=%d",num);
	HMI_SendData(str);
	HMI_SendData("click volume,1");
}
/***********************
控制播放音量
输入参数 voup：0 静音  1 音量+ 2音量减
**********************/
static void UI_Control_Volume(u8 voup)
{
	if(voup>0)
	{
		ifs_stmr_stop(store_handle);
		if(mutex_status)
		{
			mutex_status=0;//取消静音状态
		}
		else
		{
			switch(voup)
			{
				case 1:
					if(g_Volume<90)//10-90
						g_Volume+=16;
					else
						g_Volume=90;
					break;
					
				case 2:
					if(g_Volume>10)//10-90
						g_Volume-=16;
					else
						g_Volume=10;
					break;
				default:
					break;
			}
			if(g_Volume!=INIT_VOL)
			{
				ifs_stmr_start(store_handle);
			}
		}
	}
	else
	{
		mutex_status=!mutex_status;
	}
	/*显示并将控制音量信息发送给FPGA*/
	UI_Disp_Volume(g_Volume);
	_SetVolume(mutex_status>0?g_Volume:0);
}
/***********************
控制停止 复位其他状态
**********************/
static void UI_Control_StopProgram(void)
{
	MY_FILE_STRUCT.nframe=0;
	paly_status=0;
	UI_Control_PlayPause(paly_status);
	_SetProgramStatus(1);
}
/*****************************
控制部分
*****************************/
/***********************
控制下一曲
**********************/
static void Control_NextProgram(void)
{
	_SetNextProgram();
}
/***********************
控制上一曲
**********************/
static void Control_PrevProgram(void)
{
	_SetPrevProgram();
}
/**************************************
在HMI软件中自定义每个按键发送的帧格式
head+id+event
**************************************/
void HMI_Analysis(u8* datas)
{
	u8 key_id=0;
	if(datas[0]==0xa5)
	{
		key_id=datas[1];
		switch(key_id)
		{
			case CMD_PLAY_PAUSE:
				paly_status=!paly_status;
				UI_Control_PlayPause(paly_status);
				break;
			case CMD_STOP:
				UI_Control_StopProgram();
				break;
			
			case CMD_FAST:
				UI_Control_FastPreview();
				break;
			
			case CMD_FORWARD:
				UI_Control_ForwordPreview();
				break;
			
			case CMD_PREV:
				Control_PrevProgram();
				break;
			
			case CMD_NEXT:
				Control_NextProgram();
				break;
			
			case CMD_VOL_U:
				UI_Control_Volume(1);
				break;
			
			case CMD_VOL_D:
				UI_Control_Volume(2);
				break;
			
			case CMD_GET_VOL:
				UI_Disp_Volume(g_Volume);
				break;
			
			case CMD_GET_DETAIL:
				UI_Disp_TimeDetail();
				break;
			
			default://静音等 没有在屏幕触摸中实现
				key_id=0;
				break;
		}
		/*do something*/
	}
}
/*
click obj,event
obj:控件ID或控件名称
event:事件序号:0为弹起，1为按下
click b0,1 (激活名称为b0的控件的按下事件)
click 2,0 (激活ID为2的控件的弹起事件)
*/
//播放[暂停]/停止/快进+/快退-/上一曲/下一曲/静音/音量+/音量-/
void OPERATE_IR_KEY(uint8_t id,uint8_t event)
{
	UNUSED(event);
	switch(id)
	{
	case CMD_PLAY_PAUSE:
			paly_status=!paly_status;
			UI_Control_PlayPause(paly_status);
		break;
	
	case CMD_PREV:
			Control_PrevProgram();
		break;
	
	case CMD_NEXT:
			Control_NextProgram();
		break;
	
	case CMD_FAST:
			UI_Control_FastPreview();
			break;
	
	case CMD_FORWARD:
			UI_Control_ForwordPreview();
		break;
	
	case CMD_STOP:
			UI_Control_StopProgram();
		break;
	
	case CMD_MUTE:
			UI_Control_Volume(0);
		break;
	
	case CMD_VOL_U:
			UI_Control_Volume(1);
		break;
	
	case CMD_VOL_D:
			UI_Control_Volume(2);
		break;
	
	default:
		break;
	}
}
/***********************
界面相关初始化
**********************/
void HMI_init(void)
{
	MY_FILE_STRUCT.frameall=2000;
	MY_FILE_STRUCT.frame_rate=30;
	MY_FILE_STRUCT.nframe=0;
	
	UI_Control_PlayPause(paly_status);
	store_handle=ifs_stmr_registered(10000,Store_Volume,&g_Volume,IFS_STMR_FLAG_ONESHOT);
	play_handle=ifs_stmr_registered(1000,UI_PlayerRun,&MY_FILE_STRUCT,IFS_STMR_FLAG_PERIOD);
	//ifs_stmr_start(ifs_stmr_registered(1000,Disp_TimeToPlay,&MY_FILE_STRUCT,IFS_STMR_FLAG_DELETE));//注册运行后删除自身 IFS_STMR_FLAG_DELETE
	Resume_Volume(NULL);
}


//音量控制
/*
volume.val=xx
vis volume,1
tm1.en=1
*/
