#ifndef __FPGADEV_H
#define	__FPGADEV_H

#include "stm32f0xx.h"
typedef struct
{
	u32 rep_video_addr;
	u32 video_page_num;
	u32 rep_audio_addr;
	u32 audio_page_num;
	u32 rep_dq1_file_len;
	u32 audio_valid_bytes;
	u32 audio_total_bytes;
    u32 rep_dq1_aempty_num;
}REPLAY_CONFIGURE_STRUCT;

void Test_ReadWrite(void* arg);
void nvme_init(void);
/***********************
设置播放速度
**********************/
void _SetPlaySpeed(u8 SpeedLevel);
/***********************
设置播放/暂停/停止
**********************/
void _SetProgramStatus(u8 status);
/***********************
设置播放下一曲
**********************/
void _SetNextProgram(void);
/***********************
设置播放上一曲
**********************/
void _SetPrevProgram(void);
/***********************
设置播放音量
**********************/
void _SetVolume(u8 value);
/***********************
设置播放第几个文件
**********************/
void _SetPlay_Index(u8 Index);
/***********************
读取文件总帧数
**********************/
u32 _GetFileFrames(void);
/***********************
读取文件总帧数
**********************/
u32 _GetNow_Frames(void);
/***********************
读取文件帧率
**********************/
u32 _GetFileFrameRate(void);
/***********************
读取当前播放状态
**********************/
u32 _GetPlayStatues(void);
/***********************
读取将要播放的文件个数
**********************/
u32 _GetFileNums(void);
#endif /* __GPIO_H */
