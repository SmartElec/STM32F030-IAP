#include "common.h"
#define DVR_PRT(format,...)  printf("[File:"__FILE__", Line:%d]  "format, __LINE__, ##__VA_ARGS__)
#define PRT(format,...)  printf("[File:%s, Line:%d] "format, __FILE__, __LINE__, ##__VA_ARGS__)

#define DEBUG(format,...) //printf 

#define REG_CONTROL_BASEADDR 0x2000
#define REG_PLAYCONTROL	0x0082//播放/暂停/停止
#define	REG_PLAYSPEED 	1//快进快退
#define	REG_FAST		2//快进
#define REG_FORWARD		3//快退
#define	REG_NEXT		4//下一曲
#define REG_PREVIEW		5//上一曲
#define REG_VOLUME		6//音量
#define REG_PLAYFILE	7//播放指定序号的文件
#define REG_FRAMES_ALL	8//当前文件总帧数
#define REG_NOWFRAME	9//当前正在播放第几帧
#define REG_FRAMERATE	10//当前文件帧率
#define REG_PLAYSTATUS	11//播放状态寄存器
#define REG_FILENUMS	12//文件个数寄存器
#define REG_RWTEST		13//可读可写测试寄存器

//硬件初始化
void Control_SpiDevInit(void)
{
	SPI1_Init();
}
//从addr地址读取一个字节
u32 fpgedev_readWord(u32 addr)
{
	u32 ret=0;
	
	SPI_CS_LOW();
	SPI1_ReadWriteWord((u16)((addr>>16)|0x8000));
	SPI1_ReadWriteWord((u16)addr);
	ret=SPI1_ReadWriteWord(0xffff)<<16;
	ret|=SPI1_ReadWriteWord(0xffff);
	SPI_CS_HIGH();
	return ret;
}
//在addr处写一个字节
void fpgedev_writeWord(u32 addr,u32 words)
{
	SPI_CS_LOW();
	SPI1_ReadWriteWord((u16)(addr>>16));
	SPI1_ReadWriteWord((u16)addr);
	SPI1_ReadWriteWord((u16)(words>>16));
	SPI1_ReadWriteWord((u16)words);
	SPI_CS_HIGH();
}

/***********************
设置播放速度
**********************/
void _SetPlaySpeed(u8 SpeedLevel)
{
	fpgedev_writeWord(REG_CONTROL_BASEADDR+0x0080,0);//快进寄存器
	fpgedev_writeWord(REG_CONTROL_BASEADDR+0x0081,0);//快退寄存器
	if(SpeedLevel==4)
	{
		fpgedev_writeWord(REG_CONTROL_BASEADDR+0x00A4,0);//倍率设置为0
		return;
	}
//	fpgedev_writeWord(REG_CONTROL_BASEADDR+0x00A4,1);
	if(SpeedLevel>4)
	{
		fpgedev_writeWord(REG_CONTROL_BASEADDR+0x00A4,SpeedLevel-4);//快进寄存器倍率
		fpgedev_writeWord(REG_CONTROL_BASEADDR+0x0080,1);//快进寄存器
	}	
	else
	{
		fpgedev_writeWord(REG_CONTROL_BASEADDR+0x00A4,4-SpeedLevel);//快退寄存器倍率
		fpgedev_writeWord(REG_CONTROL_BASEADDR+0x0081,1);//快退寄存器
	}
		
	
}
/***********************
设置播放/暂停/停止
**********************/
void _SetProgramStatus(u8 status)
{
	if(status)
		fpgedev_writeWord(REG_CONTROL_BASEADDR+REG_PLAYCONTROL,1);
	else
		fpgedev_writeWord(REG_CONTROL_BASEADDR+REG_PLAYCONTROL,0);
}
/***********************
设置播放下一曲
**********************/
void _SetNextProgram(void)
{
	
	fpgedev_writeWord(REG_NEXT,1);
}
/***********************
设置播放上一曲
**********************/
void _SetPrevProgram(void)
{
	fpgedev_writeWord(REG_PREVIEW,1);
}
/***********************
设置播放音量
**********************/
void _SetVolume(u8 value)
{

	fpgedev_writeWord(REG_VOLUME,value);
}
/***********************
设置播放第几个文件
**********************/
void _SetPlay_Index(u8 Index)
{
	fpgedev_writeWord(REG_PLAYFILE,Index);

}
/***********************
读取文件总帧数
**********************/
u32 _GetFileFrames(void)
{
	return fpgedev_readWord(REG_FRAMES_ALL);
}
/***********************
读取文件总帧数
**********************/
u32 _GetNow_Frames(void)
{
	return fpgedev_readWord(REG_NOWFRAME);
}
/***********************
读取文件帧率
**********************/
u32 _GetFileFrameRate(void)
{

	return fpgedev_readWord(REG_FRAMERATE);
}
/***********************
读取当前播放状态
**********************/
u32 _GetPlayStatues(void)
{
	return fpgedev_readWord(REG_PLAYSTATUS);
}
/***********************
读取将要播放的文件个数
**********************/
u32 _GetFileNums(void)
{
	return fpgedev_readWord(REG_FILENUMS);
}


void Test_ReadWrite(void* arg)
{
	u32 temp=0;
	temp=fpgedev_readWord(REG_RWTEST);
	DEBUG("read:0x%08lx\r\n",temp);
	if(temp!=0xAA)
	{
		fpgedev_writeWord(REG_RWTEST,0XAA);
	}
	else
	{
		LED_Flash(NULL);
	}
		
}

/*=================================================================================*/
#define XPAR_AXI_MEM_BASEADDR 0
#define NVME_RECORD    0x1
#define NVME_REPLAY    0x0
// address
#define CFG_128B_CDQ_2M_BASE  0x00000000ul
#define CFG_128B_PDQ0_2M_BASE 0x00000000ul
#define CFG_128B_PDQ1_2M_BASE 0x01CB0000ul
#define CFG_128B_CSQ_2M_BASE  0x01F80000ul
#define CFG_128B_CCQ_2M_BASE  0x01F90000ul
#define CFG_128B_PSQ_2M_BASE  0x01F80000ul
#define CFG_128B_PCQ_2M_BASE  0x01F90000ul
#define CFG_128B_AWDQ_2M_BASE 0x01F20000ul
#define CFG_128B_ARDQ_2M_BASE 0x01F30000ul
#define CFG_128B_ASQ_2M_BASE  0x01F00000ul
#define CFG_128B_ACQ_2M_BASE  0x01F10000ul
#define CFG_128B_IWDQ_2M_BASE 0x01F60000ul
#define CFG_128B_IRDQ_2M_BASE 0x01F70000ul
#define CFG_128B_ISQ_2M_BASE  0x01F40000ul
#define CFG_128B_ICQ_2M_BASE  0x01F50000ul

#define CFG_128B_2M_OFFSET    0x00004000ul

// size
#define CFG_128B_CDQ_2M_SIZE  0x01F00000ul
#define CFG_128B_PDQ0_2M_SIZE 0x01CB0000ul
#define CFG_128B_PDQ1_2M_SIZE 0x00250000ul
#define CFG_128B_CSQ_SIZE     0x00000400ul
#define CFG_128B_CCQ_SIZE     0x00000400ul
#define CFG_128B_PSQ_SIZE     0x00000400ul
#define CFG_128B_PCQ_SIZE     0x00000400ul
#define CFG_128B_AWDQ_SIZE    0x00000400ul
#define CFG_128B_ARDQ_SIZE    0x00000400ul
#define CFG_128B_ASQ_SIZE     0x00000400ul
#define CFG_128B_ACQ_SIZE     0x00000400ul
#define CFG_128B_IWDQ_SIZE    0x00000400ul
#define CFG_128B_IRDQ_SIZE    0x00000400ul
#define CFG_128B_ISQ_SIZE     0x00000400ul
#define CFG_128B_ICQ_SIZE     0x00000400ul

#define ADMIN_SQ_DB_BASE      0x00001000ul
#define ADMIN_CQ_DB_BASE      0x00001004ul
#define NVM0_SQ_DB_BASE       0x00001008ul
#define NVM0_CQ_DB_BASE       0x0000100Cul
#define NVM1_SQ_DB_BASE       0x00001010ul
#define NVM1_CQ_DB_BASE       0x00001014ul

#define ONE_FRAME_PAGE_NUM    51ul
#define ONE_FRAME_LINE_NUM    0x000CA800ul
#define ONE_FRAME_DQ_NUM      0x00044000ul

u32 Cmd_id;
u32 Admin_sq_addr_array [3];
u32 Admin_cq_addr_array [3];
u32 Admin_sq_addr;
u32 Admin_cq_addr;


void write_reg_nvme(u32 addr,u32 wdata)
{
	fpgedev_writeWord((XPAR_AXI_MEM_BASEADDR+addr),wdata);
	//DEBUG(" [NVMe Register Write] --> addr=0x%lx, wdata=0x%lx\r\n", addr,wdata);
}

u32 read_reg_nvme(u32 addr)
{
    u32 rdata;
    rdata = fpgedev_readWord(XPAR_AXI_MEM_BASEADDR+addr);
	//DEBUG(" [NVMe Register Read]  --> addr=0x%lx, rdata=0x%lx\r\n", addr,rdata);
    return rdata;
}

void write_ddr_data (u32 addr, u32 data, u32 cmd_user_num, u32 ssd_num)
{
    write_reg_nvme((cmd_user_num << 9) + 0x0832, addr);
    for (int i = 0; i < 32; i++) {
        write_reg_nvme((cmd_user_num << 9) + 0x086F - i, i + data);
    }
    write_reg_nvme((cmd_user_num << 9) + 0x0830, 0x00000001);
    write_reg_nvme((cmd_user_num << 9) + 0x0830, 0x00000000);
}

void read_ddr_data (u32 ddr_addr, u32 cmd_user_num)
{
    u32 rd_data;

    write_reg_nvme((cmd_user_num << 9) + 0x0832, ddr_addr);     // cfg_user_addr
    write_reg_nvme((cmd_user_num << 9) + 0x0831, 0x00000001);     // cfg_user_read
    write_reg_nvme((cmd_user_num << 9) + 0x0831, 0x00000000);     // cfg_user_read
    DEBUG("Data in 0x%lx is:\r\n", ddr_addr);
    for (int i = 0; i < 32; i++) {
        rd_data = read_reg_nvme((cmd_user_num << 9) + 0x0870 + i);
       DEBUG("%lx\r\n", rd_data);
    }
    DEBUG("\r\n");
}

void read_ctrl_regs (u32 bar_raddr, u32 bar_rbytes, u32 cmd_user_num, u32 ssd_num)
{
    u32 rd_data;

    if (bar_rbytes > 16) {
        bar_rbytes = 16;
    }

    DEBUG("read addr = 0x%lx, read bytes = %ld\r\n", bar_raddr, bar_rbytes);
    write_reg_nvme((cmd_user_num << 9) + 0x0802, 1 << ssd_num);
    write_reg_nvme((cmd_user_num << 9) + 0x082A, bar_raddr);
    write_reg_nvme((cmd_user_num << 9) + 0x082B, bar_rbytes);
    write_reg_nvme((cmd_user_num << 9) + 0x0828, 0x00000001);
    write_reg_nvme((cmd_user_num << 9) + 0x0828, 0x00000000);
    delay_ms(1);
    for (int i = 0; i < 4; i++) {
        rd_data = read_reg_nvme((cmd_user_num << 9) + 0x082C + i);
        DEBUG("rd_data[%3d:%3d] = %lx\r\n", 127-i*32, 96-i*32, rd_data);
    }
}


void write_ctrl_regs (u32 bar_waddr, u32 bar_wbytes, u32 bar_wdata, u32 cmd_user_num, u32 ssd_num)
{
    if (bar_wbytes > 4) {
        DEBUG("bar_wbytes overflow!");
        bar_wbytes = 4;
    }

    write_reg_nvme((cmd_user_num << 9) + 0x0802, 1 << ssd_num);
    write_reg_nvme((cmd_user_num << 9) + 0x0822, bar_waddr);
    write_reg_nvme((cmd_user_num << 9) + 0x0823, bar_wbytes);
    write_reg_nvme((cmd_user_num << 9) + 0x0827, bar_wdata);
    write_reg_nvme((cmd_user_num << 9) + 0x0820, 0x00000001);
    write_reg_nvme((cmd_user_num << 9) + 0x0820, 0x00000000);
}

void read_cq (u32 cq_addr, u32 cmd_user_num)
{
    u32 rd_data;
    u32 cq_status_field;

    write_reg_nvme((cmd_user_num << 9) + 0x0832, cq_addr);     // cfg_user_addr
    write_reg_nvme((cmd_user_num << 9) + 0x0831, 0x00000001);     // cfg_user_read
    write_reg_nvme((cmd_user_num << 9) + 0x0831, 0x00000000);     // cfg_user_read
    delay_ms(10);
    rd_data = read_reg_nvme((cmd_user_num << 9) + 0x088C);
    cq_status_field = rd_data >> 17;
    if (cq_status_field != 0) {
        DEBUG("CQ error! The status field is %8lx\r\n", cq_status_field);
    }
    Admin_cq_addr++;
}
void config_SSD_initControllerRegisters(u32 ssd_num)
{
    u32 asq_base_addr;
    u32 acq_base_addr;

    asq_base_addr = (CFG_128B_ASQ_2M_BASE + ssd_num * CFG_128B_2M_OFFSET) << 7;
    acq_base_addr = (CFG_128B_ACQ_2M_BASE + ssd_num * CFG_128B_2M_OFFSET) << 7;

    write_ctrl_regs(0x00000014, 1, 0x00000000,            0, ssd_num);        // write CC.EN
    write_ctrl_regs(0x00000024, 2, CFG_128B_ASQ_SIZE - 1, 0, ssd_num);        // AQA.ASQS ~ 0x400
    write_ctrl_regs(0x00000026, 2, CFG_128B_ACQ_SIZE - 1, 0, ssd_num);        // AQA.ACQS ~ 0x400

    write_ctrl_regs(0x00000028, 4, asq_base_addr,         0, ssd_num);        // ASQ.ASQB
    write_ctrl_regs(0x0000002c, 4, 0x00000000,            0, ssd_num);        // ASQ.ASQB ~ 0x3E000000
    write_ctrl_regs(0x00000030, 4, acq_base_addr,         0, ssd_num);        // ASQ.ACQB
    write_ctrl_regs(0x00000034, 4, 0x00000000,            0, ssd_num);        // ASQ.ACQB ~ 0x3E200000

    write_ctrl_regs(0x00000014, 3, 0x00460481,            0, ssd_num);        // write CC.EN

    //usleep(100);
    //read_ctrl_regs(0x00000000, 0x10, 0, ssd_num);
    //read_ctrl_regs(0x00000010, 0x10, 0, ssd_num);
    //read_ctrl_regs(0x00000020, 0x10, 0, ssd_num);
    //read_ctrl_regs(0x00000030, 0x10, 0, ssd_num);
}

void write_sq_db_tail (u32 cmd_user_num, u32 db_addr)
{
    write_reg_nvme((cmd_user_num << 9) + 0x080A, db_addr);     // cfg_sq_db_waddr[31:0]
    write_reg_nvme((cmd_user_num << 9) + 0x0807, 0x00000001);     // cfg_sq_db_write
    write_reg_nvme((cmd_user_num << 9) + 0x0807, 0x00000000);     // cfg_sq_db_write
}


void write_cq_db_head (u32 cmd_user_num, u32 db_addr)
{
    write_reg_nvme((cmd_user_num << 9) + 0x080C, db_addr);     // cfg_cq_db_waddr[31:0]
    write_reg_nvme((cmd_user_num << 9) + 0x0808, 0x00000001);     // cfg_cq_db_write
    write_reg_nvme((cmd_user_num << 9) + 0x0808, 0x00000000);     // cfg_cq_db_write
}


void nvm_admin_create_io_cq(u32 cq_addr,
                            u32 ccq_qid,
                            u32 cmd_user_num,
                            u32 ssd_num)
{
    u32 acq_base_addr;

    acq_base_addr = (cq_addr + ssd_num * CFG_128B_2M_OFFSET) << 7;

    DEBUG("[nvm_admin_create_io_cq] \r\n");

    write_reg_nvme((cmd_user_num << 9) + 0x0832, Admin_sq_addr);      // cfg_user_addr
    write_reg_nvme((cmd_user_num << 9) + 0x086F, 0x00000005 + (Cmd_id << 16));        // Dword0
    write_reg_nvme((cmd_user_num << 9) + 0x086E, 0x00000000);                         // Dword1
    write_reg_nvme((cmd_user_num << 9) + 0x086D, 0x00000000);                         // Dword2
    write_reg_nvme((cmd_user_num << 9) + 0x086C, 0x00000000);                         // Dword3
    write_reg_nvme((cmd_user_num << 9) + 0x086B, 0x00000000);                         // Dword4
    write_reg_nvme((cmd_user_num << 9) + 0x086A, 0x00000000);                         // Dword5
    write_reg_nvme((cmd_user_num << 9) + 0x0869, acq_base_addr);                      // Dword6
    write_reg_nvme((cmd_user_num << 9) + 0x0868, 0x00000000);                         // Dword7
    write_reg_nvme((cmd_user_num << 9) + 0x0867, 0x00000000);                         // Dword8
    write_reg_nvme((cmd_user_num << 9) + 0x0866, 0x00000000);                         // Dword9
    write_reg_nvme((cmd_user_num << 9) + 0x0865, 0x03FF0000 + ccq_qid);               // Dword10
    write_reg_nvme((cmd_user_num << 9) + 0x0864, 0x00000001);                         // Dword11
    write_reg_nvme((cmd_user_num << 9) + 0x0863, 0x00000000);                         // Dword12
    write_reg_nvme((cmd_user_num << 9) + 0x0862, 0x00000000);                         // Dword13
    write_reg_nvme((cmd_user_num << 9) + 0x0861, 0x00000000);                         // Dword14
    write_reg_nvme((cmd_user_num << 9) + 0x0860, 0x00000000);                         // Dword15
    write_reg_nvme((cmd_user_num << 9) + 0x0830, 0x00000001);                         // cfg_user_write
    write_reg_nvme((cmd_user_num << 9) + 0x0830, 0x00000000);                         // cfg_user_write

    // Write sq tail
    write_sq_db_tail(0, ADMIN_SQ_DB_BASE);

    Admin_sq_addr++;
    Cmd_id++;
    delay_ms(10);

    // Check cq
    read_cq(Admin_cq_addr, 0);
    write_cq_db_head(0, ADMIN_CQ_DB_BASE);
}
void nvm_admin_create_io_sq(u32 sq_addr,
                            u32 csq_cqid,
                            u32 csq_qid,
                            u32 cmd_user_num,
                            u32 ssd_num)
{

    u32 asq_base_addr;

    asq_base_addr = (sq_addr + ssd_num * CFG_128B_2M_OFFSET) << 7;

    DEBUG("[nvm_admin_create_io_sq] \r\n");

    write_reg_nvme((cmd_user_num << 9) + 0x0832, Admin_sq_addr);
    write_reg_nvme((cmd_user_num << 9) + 0x086F, 0x00000001 + (Cmd_id << 16));        // Dword0
    write_reg_nvme((cmd_user_num << 9) + 0x086E, 0x00000000);                         // Dword1
    write_reg_nvme((cmd_user_num << 9) + 0x086D, 0x00000000);                         // Dword2
    write_reg_nvme((cmd_user_num << 9) + 0x086C, 0x00000000);                         // Dword3
    write_reg_nvme((cmd_user_num << 9) + 0x086B, 0x00000000);                         // Dword4
    write_reg_nvme((cmd_user_num << 9) + 0x086A, 0x00000000);                         // Dword5
    write_reg_nvme((cmd_user_num << 9) + 0x0869, asq_base_addr);                      // Dword6
    write_reg_nvme((cmd_user_num << 9) + 0x0868, 0x00000000);                         // Dword7
    write_reg_nvme((cmd_user_num << 9) + 0x0867, 0x00000000);                        // Dword8
    write_reg_nvme((cmd_user_num << 9) + 0x0866, 0x00000000);                         // Dword9
    write_reg_nvme((cmd_user_num << 9) + 0x0865, 0x03FF0000 + csq_qid);               // Dword10
    write_reg_nvme((cmd_user_num << 9) + 0x0864, 0x00000001 + (csq_cqid << 16));      // Dword11
    write_reg_nvme((cmd_user_num << 9) + 0x0863, 0x00000000);                         // Dword12
    write_reg_nvme((cmd_user_num << 9) + 0x0862, 0x00000000);                         // Dword13
    write_reg_nvme((cmd_user_num << 9) + 0x0861, 0x00000000);                         // Dword14
    write_reg_nvme((cmd_user_num << 9) + 0x0860, 0x00000000);                         // Dword15
    write_reg_nvme((cmd_user_num << 9) + 0x0830, 0x00000001);                         // cfg_user_write
    write_reg_nvme((cmd_user_num << 9) + 0x0830, 0x00000000);                         // cfg_user_write

    // Write sq tail
    write_sq_db_tail(0, ADMIN_SQ_DB_BASE);

    Admin_sq_addr++;
    Cmd_id++;
    delay_ms(10);

    // Check cq
    read_cq(Admin_cq_addr, 0);
    write_cq_db_head(0, ADMIN_CQ_DB_BASE);
}
void nvme_data_set (void)
{

    write_reg_nvme(0x0120, 0x00000000);     // {f31_f2_r,dset_psdt}
    write_reg_nvme(0x0121, 0x00000000);     // {f31_f2_r,dset_fuse}
    write_reg_nvme(0x0122, 0x00000009);     // {f31_f8_r,dset_opc}
    write_reg_nvme(0x0123, 0x00000000);     // {dset_nsid}
    write_reg_nvme(0x0124, 0x00000000);     // {dset_mptr[63:32]}
    write_reg_nvme(0x0125, 0x00000000);     // {dset_mptr[31:0]}
    write_reg_nvme(0x0126, 0x00000000);     // {dset_dptr[127:96]}
    write_reg_nvme(0x0127, 0x00000000);     // {dset_dptr[95:64]}
    write_reg_nvme(0x0128, 0x00000000);     // {dset_dptr[63:32]}
//    write_reg_nvme(0x0129, 0x0F920000);     // {dset_dptr[31:0]}    ~ CFG_1B_IWDQ_BASE
    write_reg_nvme(0x0129, 0x3F920000);     // {dset_dptr[31:0]}    ~ CFG_1B_IWDQ_BASE

    write_reg_nvme(0x012A, 0x00000000);     // {f31_f8_r,dset_nr}   ~ Number of Ranges (NR)
    write_reg_nvme(0x012B, 0x00000000);     // {f31_f1_r,dset_ad}   ~ Attribute 鈥? Deallocate (AD)
    write_reg_nvme(0x012C, 0x00000000);     // {f31_f1_r,dset_idw}  ~ Attribute 鈥? Integral Dataset for Write (IDW)
    write_reg_nvme(0x012D, 0x00000001);     // {f31_f1_r,dset_idr}  ~ Attribute 鈥? Integral Dataset for Read (IDR)
}


void nvme_cmd_addr_set ()
{
    write_reg_nvme(0x0810, CFG_128B_ASQ_2M_BASE                            << 7); // cfg_sq_addr_start
    write_reg_nvme(0x0811, (CFG_128B_ASQ_2M_BASE + CFG_128B_2M_OFFSET * 4) << 7); // cfg_sq_addr_end
    write_reg_nvme(0x0812, CFG_128B_ACQ_2M_BASE                            << 7); // cfg_cq_addr_start
    write_reg_nvme(0x0813, (CFG_128B_ACQ_2M_BASE + CFG_128B_2M_OFFSET * 4) << 7); // cfg_cq_addr_end
    write_reg_nvme(0x0814, CFG_128B_PDQ0_2M_BASE                           << 7); // cfg_dq_addr_start
    write_reg_nvme(0x0815, CFG_128B_PDQ1_2M_BASE                           << 7); // cfg_dq_addr_end
}


void nvme_rec_addr_set ()
{
    write_reg_nvme(0x1010, CFG_128B_CSQ_2M_BASE                            << 7); // cfg_sq_addr_start
    write_reg_nvme(0x1011, (CFG_128B_CSQ_2M_BASE + CFG_128B_2M_OFFSET * 4) << 7); // cfg_sq_addr_end
    write_reg_nvme(0x1012, CFG_128B_CCQ_2M_BASE                            << 7); // cfg_cq_addr_start
    write_reg_nvme(0x1013, (CFG_128B_CCQ_2M_BASE + CFG_128B_2M_OFFSET * 4) << 7); // cfg_cq_addr_end
    write_reg_nvme(0x1014, CFG_128B_CDQ_2M_BASE                            << 7); // cfg_dq_addr_start
    write_reg_nvme(0x1015, CFG_128B_ASQ_2M_BASE                            << 7); // cfg_dq_addr_end
}

void nvme_rep_addr_set ()
{
    write_reg_nvme(0x2010, CFG_128B_CSQ_2M_BASE                            << 7); // cfg_sq_addr_start
    write_reg_nvme(0x2011, (CFG_128B_CSQ_2M_BASE + CFG_128B_2M_OFFSET * 4) << 7); // cfg_sq_addr_end
    write_reg_nvme(0x2012, CFG_128B_CCQ_2M_BASE                            << 7); // cfg_cq_addr_start
    write_reg_nvme(0x2013, (CFG_128B_CCQ_2M_BASE + CFG_128B_2M_OFFSET * 4) << 7); // cfg_cq_addr_end
    write_reg_nvme(0x2014, CFG_128B_PDQ0_2M_BASE                           << 7); // cfg_dq_addr_start
    write_reg_nvme(0x2015, CFG_128B_PDQ1_2M_BASE                           << 7); // cfg_dq_addr_end
}

void nvme_size_set ()
{
    write_reg_nvme(0x1271, CFG_128B_CDQ_2M_SIZE); // cfg_cdq_sz
    write_reg_nvme(0x1272, CFG_128B_CSQ_SIZE); // cfg_csq_sz
    write_reg_nvme(0x1273, CFG_128B_CCQ_SIZE); // cfg_ccq_sz
    write_reg_nvme(0x1275, CFG_128B_PSQ_SIZE); // cfg_psq_sz
    write_reg_nvme(0x1276, CFG_128B_PCQ_SIZE); // cfg_pcq_sz
    write_reg_nvme(0x1277, CFG_128B_AWDQ_SIZE); // cfg_awdq_sz
    write_reg_nvme(0x1278, CFG_128B_ARDQ_SIZE); // cfg_ardq_sz
    write_reg_nvme(0x1279, CFG_128B_ASQ_SIZE); // cfg_asq_sz
    write_reg_nvme(0x127A, CFG_128B_ACQ_SIZE); // cfg_acq_sz
    write_reg_nvme(0x127B, CFG_128B_IWDQ_SIZE); // cfg_iwdq_sz
    write_reg_nvme(0x127C, CFG_128B_IRDQ_SIZE); // cfg_irdq_sz
    write_reg_nvme(0x127D, CFG_128B_ISQ_SIZE); // cfg_isq_sz
    write_reg_nvme(0x127E, CFG_128B_ICQ_SIZE); // cfg_icq_sz
    write_reg_nvme(0x1274, CFG_128B_PDQ0_2M_SIZE); // cfg_pdq0_sz
    write_reg_nvme(0x127F, CFG_128B_PDQ1_2M_SIZE); // cfg_pdq1_sz
    write_reg_nvme(0x1270, 0x0000FFFF);
    write_reg_nvme(0x1270, 0x00000000);
}
void record_configure(void)
/*=================================================================================*/
{
    DEBUG("-------------------------------------------- \r\n");
    DEBUG(" START of record_configure()             \r\n");
    DEBUG("-------------------------------------------- \r\n");
    nvme_rec_addr_set();
    write_reg_nvme(0x1004, 0x00000002        );   // cfg_ssd_num, 0~1SSD
    write_reg_nvme(0x1008, 0x00000000        );   // cfg_sq_db_waddr[63:32]
    write_reg_nvme(0x1009, NVM1_SQ_DB_BASE);   // cfg_sq_db_waddr[31:0]
    write_reg_nvme(0x100A, 0x00000000        );   // cfg_cq_db_waddr[63:32]
    write_reg_nvme(0x100B, NVM1_CQ_DB_BASE);   // cfg_cq_db_waddr[31:0]
    write_reg_nvme(0x1020, 0x00000000          );   // cfg_rec_dq_start
    write_reg_nvme(0x1021, 0x01F80000          );   // cfg_rec_sq_start
    write_reg_nvme(0x1022, 0x01F90000          );   // cfg_rec_cq_start
    write_reg_nvme(0x1023, 0x01F00000          );   // cfg_rec_dq_size
    write_reg_nvme(0x1025, 0x00000100          );   // cfg_rec_dq_burst_len
    write_reg_nvme(0x1026, 0x00000000          );   // cfg_rec_start_slba[63:32]
    write_reg_nvme(0x1027, 0x00000000          );   // cfg_rec_start_slba[31:0]
    write_reg_nvme(0x1030, 0xFFFFFFFF          );   // cfg_rec_dq_file_len(step)
    write_reg_nvme(0x1031, 0xFFFFFFFF          );   // cfg_rec_dq_file_len(step)
    write_reg_nvme(0x1050, 0x01E00000          );   // cfg_rec_dq_afull
    write_reg_nvme(0x1051, 0x00000020          );   // cfg_rec_dq_aempty
    write_reg_nvme(0x1052, 0x000003C0          );   // cfg_rec_sq_afull
    write_reg_nvme(0x1053, 0x00000005          );   // cfg_rec_sq_aempty
    write_reg_nvme(0x1054, 0x000003C0          );   // cfg_rec_cq_afull
    write_reg_nvme(0x1055, 0x00000005          );   // cfg_rec_cq_aempty
    write_reg_nvme(0x1062, 0x00000001          );   // write_opc
    write_reg_nvme(0x1063, 0x00000001          );   // write_nsid
    write_reg_nvme(0x1067, 0x00000001          );   // write_fua
    write_reg_nvme(0x1069, 0x00000FFF          );   // write_nlb = 4096
    write_reg_nvme(0x1080, 0x00000100          );   // cfg_rready_thrh
    write_reg_nvme(0x1081, 0x00000000          );   // cfg_record_intf
    write_reg_nvme(0x1082, 0x00000000          );   // cfg_test_proc
    write_reg_nvme(0x1083, 0x30000000          );   // cfg_rec_wait_cycle
    write_reg_nvme(0x1084, 0x00000000          );   // cfg_rep_dq_mode
    write_reg_nvme(0x1085, 0x00000000          );   // cfg_info_data_slba[63:32]
    write_reg_nvme(0x1086, 0x37000000          );   // cfg_info_data_slba[31:0]
    write_reg_nvme(0x1087, 0x00000028          );   // cfg_pixel_num  
    write_reg_nvme(0x1088, ONE_FRAME_LINE_NUM);   // cfg_frame_line_vld_len 
    write_reg_nvme(0x1089, 0x00000001          );   // cfg_rec_dio_en 
    write_reg_nvme(0x3003, 0x00E00080          );   // pcie_slv_rd_time_out
    write_reg_nvme(0x3004, 0x00040010          );   // pcie_slv_wr_time_out
    DEBUG("-------------------------------------------- \r\n");
    DEBUG(" END of record_configure()               \r\n");
    DEBUG("-------------------------------------------- \r\n");
}
void ssd_config(u32 ssd_num)
{
    DEBUG("Info:======================[SSD%ld_config]===================\r\n", ssd_num);
    // select SSD
    write_reg_nvme(0x0802, 0x00000001 << ssd_num);     //cfg_ssd_select

    // initial SSD
    config_SSD_initControllerRegisters(ssd_num);
    delay_ms(1);

    // Create io CQ and SQ
    nvm_admin_create_io_cq(CFG_128B_ICQ_2M_BASE, 1, 0, ssd_num);
    delay_ms(1);
    nvm_admin_create_io_sq(CFG_128B_ISQ_2M_BASE, 1, 1, 0, ssd_num);
    delay_ms(1);

    // Create function CQ and SQ
    nvm_admin_create_io_cq(CFG_128B_CCQ_2M_BASE, 2, 0, ssd_num);
    delay_ms(1);
    nvm_admin_create_io_sq(CFG_128B_CSQ_2M_BASE, 2, 2, 0, ssd_num);
	delay_ms(1);

}

void ssd_initial (u32 ssd_num)
{
    for (int i = 0; i < ssd_num; i++) {
        Cmd_id = 0;
        Admin_sq_addr = CFG_128B_ASQ_2M_BASE + CFG_128B_2M_OFFSET * i;
        Admin_cq_addr = CFG_128B_ACQ_2M_BASE + CFG_128B_2M_OFFSET * i;
        ssd_config(i);
        Admin_sq_addr_array[i] = Admin_sq_addr;
        Admin_cq_addr_array[i] = Admin_cq_addr;
    }
}
void replay_configure(REPLAY_CONFIGURE_STRUCT* configure)
/*=================================================================================*/
{
    u32 rep_dq0_file_len;
//    u32 cfg_rep_sq0_file_len;
    //u32 cfg_rep_sq1_file_len;
    u32 video_frame_num;

    video_frame_num    = configure->video_page_num / ONE_FRAME_PAGE_NUM;
    rep_dq0_file_len    = configure->video_page_num * ONE_FRAME_LINE_NUM;
    //cfg_rep_sq1_file_len    = audio_page_num;

    write_reg_nvme(0x2002, 0x00000000                  );       // cfg_force_sm


    write_reg_nvme(0x0085, 0x00000001                  );       // {f31_f1_r, cfg_sch_fifo_overtime_en}
    write_reg_nvme(0x0082, 0x01000020                 );       // cfg_rr_burst_len
    write_reg_nvme(0x0083, 0x01000020                 );       // cfg_user_burst_len
    write_reg_nvme(0x0084, 0xFFFF0002                 );       // cfg_rsch_fifo_overtime
    write_reg_nvme(0x0009, 0x000000FF                 );       // cfg_oc_cq_en,cfg_oc_sq_en
    write_reg_nvme(0x3003, 0x00FFFFFF                  );       // pcie_slv_rd_time_out
    write_reg_nvme(0x3004, 0x00FFFFFF                  );       // pcie_slv_wr_time_out

    nvme_rep_addr_set();
    write_reg_nvme(0x2004, 0x00000002                  );       // cfg_ssd_num, 0~ 1SSD, 1~ 2SSD, 2~ 3SSD, 3~ 4SSD
    write_reg_nvme(0x2005, 0x00000000                  );       // user_axi4_wbp_en
    write_reg_nvme(0x2008, 0x00000000                  );       // cfg_sq_db_waddr[63:32]
    write_reg_nvme(0x2009, NVM1_SQ_DB_BASE             );       // cfg_sq_db_waddr[31:0]
    write_reg_nvme(0x200A, 0x00000000                  );       // cfg_cq_db_waddr[63:32]
    write_reg_nvme(0x200B, NVM1_CQ_DB_BASE             );       // cfg_cq_db_waddr[31:0]
    write_reg_nvme(0x2026, 0x00000000               );       // {cfg_rep0_start_slba[63:32]}
    write_reg_nvme(0x2027, configure->rep_video_addr   );       // {cfg_rep0_start_slba[31:0]}
    write_reg_nvme(0x2030, 0x00000000                 );       // cfg_rep_dq_file_len[35:32]
    write_reg_nvme(0x2031, rep_dq0_file_len);       // cfg_rep_dq_file_len[31:0]
    write_reg_nvme(0x2032, configure->video_page_num );       // cfg_rep_sq_file_len
    write_reg_nvme(0x2033, 0xFFFFFFFF                  );       // cfg_rep_cycle_len

    //
//  write_reg_nvme(0x2050, ONE_FRAME_LINE_NUM * 15 / 3,  );       // cfg_rep_dq0_afull     max: 36
//  write_reg_nvme(0x2051, ONE_FRAME_LINE_NUM *  6 / 3,  );       // cfg_rep_dq0_aempty
//  write_reg_nvme(0x2052, ONE_FRAME_LINE_NUM * 30 / 3,  );       // cfg_rep_dq1_afull
//  write_reg_nvme(0x2053, ONE_FRAME_LINE_NUM *  3 / 3,  );       // cfg_rep_dq1_aempty

    //
    write_reg_nvme(0x2050, ONE_FRAME_DQ_NUM * 33       );       // cfg_rep_dq0_afull     max: 36
    write_reg_nvme(0x2051, ONE_FRAME_DQ_NUM * 30        );       // cfg_rep_dq0_aempty
    write_reg_nvme(0x2052, ONE_FRAME_DQ_NUM * 35        );       // cfg_rep_dq1_afull
    write_reg_nvme(0x2053, ONE_FRAME_DQ_NUM * 1         );       // cfg_rep_dq1_aempty

    write_reg_nvme(0x2054, 0x00000100                   );       // cfg_rep_sq_afull
    write_reg_nvme(0x2055, 0x00000001                   );       // cfg_rep_sq_aempty
    write_reg_nvme(0x2056, 0x000000E0                   );       // cfg_cq_afull_num
    write_reg_nvme(0x2057, 0x00000020                   );       // cfg_cq_aempty_num
    write_reg_nvme(0x2062, 0x00000002                   );       // read_opc
    write_reg_nvme(0x2063, 0x00000001                   );       // read_nsid
//  write_reg_nvme(0x206F, 0x09FF0FFF                   );       // read_nlb_1, read_nlb_0
    write_reg_nvme(0x206F, 0x0FFF0FFF                   );       // read_nlb_1, read_nlb_0
    write_reg_nvme(0x20A0, 0x00000028                   );       // cfg_pixel_num
    write_reg_nvme(0x20A1, 0x00000130                   );       // cfg_video_prog_full_thresh
    write_reg_nvme(0x20A2, 0x01950000                   );       // cfg_video_valid_bytes
//  write_reg_nvme(0x20A3, 0x01950000                   );       // cfg_video_total_bytes
    write_reg_nvme(0x20A3, 0x01980000                   );       // cfg_video_total_bytes
    write_reg_nvme(0x20A4, 0x00000004                   );       // cfg_jump_block_num
    write_reg_nvme(0x20A5, video_frame_num             );       // cfg_rep_dq_block_total
    write_reg_nvme(0x20A6, 0x00000000                   );       // cfg_rep_dq_chn_offset_0
    write_reg_nvme(0x20A7, 0x00000060                   );       // cfg_rep_dq_chn_offset_1
    write_reg_nvme(0x20A8, 0x00065400                   );       // cfg_rep_dq_chn_offset_2
    write_reg_nvme(0x20A9, 0x00065460                   );       // cfg_rep_dq_chn_offset_3
    write_reg_nvme(0x20AA, 0x00000060                   );       // cfg_width_size
    write_reg_nvme(0x20AB, 0x00065400                   );       // cfg_height_size

    //write_reg_nvme(0x103D, 0x00000000                   );       // {cfg_rep1_start_slba[63:32]}
    //write_reg_nvme(0x103E, rep_audio_addr               );       // {cfg_rep1_start_slba[31:0]}
    //write_reg_nvme(0x1061, rep_dq1_file_len / 2         );       // cfg_pdq1_rd_sz
    //write_reg_nvme(0x12A0, 0x00030000                   );       // cfg_pdf0_sz
    //write_reg_nvme(0x12A1, 0x00030000                   );       // cfg_pdf1_sz
    //write_reg_nvme(0x0405, 0x00000010                   );       // cfg_audio_prog_empty_thresh
    //write_reg_nvme(0x0406, 0x00000100                   );       // cfg_audio_prog_full_thresh
    //write_reg_nvme(0x105C, 0x00000000                   );  
    //write_reg_nvme(0x105D, rep_dq1_file_len             );       // cfg_rep_dq1_file_len = 4096  *********

    write_reg_nvme(0x2003, 0x00000001                   );       // cfg_rep_start
    write_reg_nvme(0x2003, 0x00000000                   );       // cfg_rep_start

    DEBUG(" _____________________________________________ \r\n");
    DEBUG("|                                             |\r\n");
    DEBUG("|                 Replay Start                |\r\n");
    DEBUG("|_____________________________________________|\r\n");
}
void get_DFX_registers(u32 registerType)
/*=================================================================================*/
{
    if(registerType == 0x0)
    {
        DEBUG("// -------------------------------------- \r\n");
        DEBUG("// DFX for NVMe_IP \r\n");
        DEBUG("// -------------------------------------- \r\n");
        read_reg_nvme(0x3014 ); // {c0_master_debug_cnt[127:96]}    // 
        read_reg_nvme(0x3114 ); // {c1_master_debug_cnt[127:96]}
        read_reg_nvme(0x3214 ); // {c2_master_debug_cnt[127:96]}
        read_reg_nvme(0x3015 ); // {c0_master_debug_cnt[95:64]}     // 
        read_reg_nvme(0x3115 ); // {c1_master_debug_cnt[95:64]}
        read_reg_nvme(0x3215 ); // {c2_master_debug_cnt[95:64]}
        read_reg_nvme(0x3040 ); // {c0_cq_sq_cmd_debug_cnt[63:32]}  // cq_cmd cnt
        read_reg_nvme(0x3041 ); // {c0_cq_sq_cmd_debug_cnt[31:0]}   // sq_cmd cnt
        read_reg_nvme(0x3140 ); // {c1_cq_sq_cmd_debug_cnt[63:32]}  // cq_cmd cnt
        read_reg_nvme(0x3141 ); // {c1_cq_sq_cmd_debug_cnt[31:0]}   // sq_cmd cnt
        read_reg_nvme(0x3240 ); // {c2_cq_sq_cmd_debug_cnt[63:32]}  // cq_cmd cnt
        read_reg_nvme(0x3241 ); // {c2_cq_sq_cmd_debug_cnt[31:0]}   // sq_cmd cnt

        DEBUG("// -------------------------------------- \r\n");
        read_reg_nvme(0x301D ); // pcie0 slave write:(axi4_slave_wr.v) (axi4_slave_wr.v) awsize[2:0],awlen[7:0],wr_bresp_cnt[15:0]
        read_reg_nvme(0x301E ); // pcie0 slave write:(axi4_slave_wr.v) wr_byte_out,wr_to_ddr_wdata_cnt
        read_reg_nvme(0x301F ); // pcie0 slave write:(axi4_slave_wr.v) wr_to_ddr_addr_cnt,wr_to_ddr_wlast_cnt
        read_reg_nvme(0x3022 ); // pcie0 slave write:(slave_wr_data_align.v) data_out_cnt
        read_reg_nvme(0x3023 ); // pcie0 slave write:(slave_wr_data_align.v) data_in_cnt
        read_reg_nvme(0x3024 ); // pcie0 (rd_addr_pcie_out,rd_last_to_pcie_cnt)
        read_reg_nvme(0x3025 ); // pcie0 (rd_addr_to_ddr_cnt,rd_data_to_pcie_cnt)
        read_reg_nvme(0x3026 ); // pcie0 (low 16bit is rd_data_in_cnt)
        read_reg_nvme(0x3021 ); // pcie0  slave write:(slave_wr_width_transform.v) full write cnt

        DEBUG("// -------------------------------------- \r\n");
        read_reg_nvme(0x311D ); // pcie1 slave write:(axi4_slave_wr.v) (axi4_slave_wr.v) awsize[2:0],awlen[7:0],wr_bresp_cnt[15:0]
        read_reg_nvme(0x311E ); // pcie1 slave write:(axi4_slave_wr.v) wr_byte_out,wr_to_ddr_wdata_cnt
        read_reg_nvme(0x311F ); // pcie1 slave write:(axi4_slave_wr.v) wr_to_ddr_addr_cnt,wr_to_ddr_wlast_cnt
        read_reg_nvme(0x3122 ); // pcie1 slave write:(slave_wr_data_align.v) data_out_cnt
        read_reg_nvme(0x3123 ); // pcie1 slave write:(slave_wr_data_align.v) data_in_cnt
        read_reg_nvme(0x3124 ); // pcie1 (rd_addr_pcie_out,rd_last_to_pcie_cnt)
        read_reg_nvme(0x3125 ); // pcie1 (rd_addr_to_ddr_cnt,rd_data_to_pcie_cnt)
        read_reg_nvme(0x3126 ); // pcie1 (low 16bit is rd_data_in_cnt)
        read_reg_nvme(0x3121 ); // pcie1  slave write:(slave_wr_width_transform.v) full write cnt

        DEBUG("// -------------------------------------- \r\n");
        read_reg_nvme(0x321D ); // pcie2 slave write:(axi4_slave_wr.v) (axi4_slave_wr.v) awsize[2:0],awlen[7:0],wr_bresp_cnt[15:0]
        read_reg_nvme(0x321E ); // pcie2 slave write:(axi4_slave_wr.v) wr_byte_out,wr_to_ddr_wdata_cnt
        read_reg_nvme(0x321F ); // pcie2 slave write:(axi4_slave_wr.v) wr_to_ddr_addr_cnt,wr_to_ddr_wlast_cnt
        read_reg_nvme(0x3222 ); // pcie2 slave write:(slave_wr_data_align.v) data_out_cnt
        read_reg_nvme(0x3223 ); // pcie2 slave write:(slave_wr_data_align.v) data_in_cnt
        read_reg_nvme(0x3224 ); // pcie2 (rd_addr_pcie_out,rd_last_to_pcie_cnt)
        read_reg_nvme(0x3225 ); // pcie2 (rd_addr_to_ddr_cnt,rd_data_to_pcie_cnt)
        read_reg_nvme(0x3226 ); // pcie2 (low 16bit is rd_data_in_cnt)
        read_reg_nvme(0x3221 ); // pcie2  slave write:(slave_wr_width_transform.v) full write cnt


        DEBUG("// -------------------------------------- \r\n");
        read_reg_nvme(0x3050 ); // {c0_cq_err_data[31:0]}
        read_reg_nvme(0x3051 ); // {c0_cq_err_data[63:32]}
        read_reg_nvme(0x3052 ); // {c0_cq_err_data[95:64]}
        read_reg_nvme(0x3053 ); // {c0_cq_err_data[127:96]}
        read_reg_nvme(0x3150 ); // {c1_cq_err_data[31:0]}
        read_reg_nvme(0x3151 ); // {c1_cq_err_data[63:32]}
        read_reg_nvme(0x3152 ); // {c1_cq_err_data[95:64]}
        read_reg_nvme(0x3153 ); // {c1_cq_err_data[127:96]}
        read_reg_nvme(0x3250 ); // {c2_cq_err_data[31:0]}
        read_reg_nvme(0x3251 ); // {c2_cq_err_data[63:32]}
        read_reg_nvme(0x3252 ); // {c2_cq_err_data[95:64]}
        read_reg_nvme(0x3253 ); // {c2_cq_err_data[127:96]}

        DEBUG("// -------------------------------------- \r\n");
        read_reg_nvme(0x2070 ); // {rep_dq_wcnt[0]} 
        read_reg_nvme(0x2071 ); // {rep_dq_wcnt[1]} 
        read_reg_nvme(0x2072 ); // {rep_dq_wcnt[2]} 
        read_reg_nvme(0x2073 ); // {rep_dq_wcnt[3]} 
        read_reg_nvme(0x2074 ); // {rep_dq_rcnt[0]} 
        read_reg_nvme(0x2075 ); // {rep_dq_rcnt[1]} 
        read_reg_nvme(0x2076 ); // {rep_dq_rcnt[2]} 
        read_reg_nvme(0x2077 ); // {rep_dq_rcnt[3]} 

        DEBUG("// END of 'get_DFXregisters' \r\n");

    }
    else
    {

    }
}
void nvme_init(void)
{
	u16 i=0;
	u32 nvme_process;
	
    nvme_cmd_addr_set();
    ssd_initial(3);
    for (i = 0; i < 3; i++) {
        DEBUG("Admin_sq_addr_array[%d] == 0x%lx\r\n", i, Admin_sq_addr_array[i]);
        DEBUG("Admin_cq_addr_array[%d] == 0x%lx\r\n", i, Admin_cq_addr_array[i]);
    }
	if(nvme_process == NVME_RECORD) 
    {
        record_configure();
    }
	else if(nvme_process == NVME_REPLAY) 
	{
		REPLAY_CONFIGURE_STRUCT REPLAY_CONFIGURE;
		REPLAY_CONFIGURE.rep_audio_addr=0;
		REPLAY_CONFIGURE.video_page_num= 51 * 540;
		REPLAY_CONFIGURE.rep_audio_addr=0;
		REPLAY_CONFIGURE.audio_page_num=0;
		REPLAY_CONFIGURE.rep_dq1_file_len=0;
		REPLAY_CONFIGURE.audio_valid_bytes=0;
		REPLAY_CONFIGURE.audio_total_bytes=0;
		REPLAY_CONFIGURE.rep_dq1_aempty_num=0;
		
        replay_configure(&REPLAY_CONFIGURE);
        //sleep(60);
        get_DFX_registers(0x0);
	}
}
