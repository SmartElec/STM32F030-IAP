/*
 * !brief include header
 */

#include "softtimer.h"

#define SOFT_TIMERS_MAX 3
ifs_uint32_t stmr_ticks=0;


struct ifs_stmr_t {
    ifs_uint32_t timer_count;
    ifs_uint32_t time_out;
    void (*time_out_callback)(void *param);
    void *param;
    ifs_uint8_t timer_flag;
    struct option {
        ifs_uint8_t is_active : 1;
        ifs_uint8_t is_alive  : 1;
        ifs_uint8_t nc        : 6;    
    }opt;
} timer_list[SOFT_TIMERS_MAX];


void TMR_CALLBACK(struct ifs_stmr_t timer)
{
	timer.time_out_callback(timer.param);
}

void ifs_stmr_init(void)
{
    ifs_uint32_t i;
    for (i = 0; i < SOFT_TIMERS_MAX; i++) {
        timer_list[i].timer_count = 0;
        timer_list[i].timer_flag = 0;
        timer_list[i].time_out = 0;
        timer_list[i].time_out_callback = NULL;
        timer_list[i].param = NULL;
        timer_list[i].opt.is_active = 0;
        timer_list[i].opt.is_alive = 0;
    }
    stmr_ticks = 0;
}

ifs_stmr_handle ifs_stmr_registered(ifs_uint32_t period, 
                                      timer_out_callback cb,
                                      void *cb_param,
                                      IFS_STMR_FLAG flag)
{
    int i;
    
    if (cb == NULL) return -1;
    
    for (i = 0; i < SOFT_TIMERS_MAX; i++) {
        if (timer_list[i].opt.is_alive) continue;
        timer_list[i].opt.is_alive = 1;
        timer_list[i].opt.is_active = 0;
        timer_list[i].time_out = period;
        timer_list[i].time_out_callback = cb;
        timer_list[i].param = cb_param;
        timer_list[i].timer_flag = flag;
        return i;
    }
    
    return -1;
}
int ifs_stmr_delete(ifs_stmr_handle handle)
{
    int i = (int)handle;
    
    if (i >= SOFT_TIMERS_MAX || i < 0) return -1;
    timer_list[i].opt.is_alive = 0;
    timer_list[i].opt.is_active = 0;
    timer_list[i].timer_count = 0;
    timer_list[i].time_out = 0;
    timer_list[i].time_out_callback = NULL;
    timer_list[i].param = NULL;
    timer_list[i].timer_flag = 0;
    return 0;
}
int ifs_stmr_start(ifs_stmr_handle handle)
{
    int i = (int)handle;
    
    if (i >= SOFT_TIMERS_MAX || i < 0) return -1;
    if (!timer_list[i].opt.is_alive) return -1;
    
    //if (timer_list[i].opt.is_active != 1) //去掉判断 运行一次start就会重新计时
		{
        timer_list[i].timer_count = stmr_ticks;
        timer_list[i].opt.is_active = 1;
    }
    
    return 0;
}
int ifs_stmr_stop(ifs_stmr_handle handle)
{
    int i = (int)handle;
    
    if (i >= SOFT_TIMERS_MAX || i < 0) return -1;
    if (!timer_list[i].opt.is_alive) return -1;
    
    if (timer_list[i].opt.is_active != 0)
        timer_list[i].opt.is_active = 0;
    
    return 0;
}
void ifs_stmr_interrupt(void)
{
    stmr_ticks ++;
}
/*
* !brief soft timer task
*/
int ifs_stmr_task(void)
{
    ifs_uint8_t i, ret = 0;
    
    for (i = 0; i < SOFT_TIMERS_MAX; i ++) 
	{
        if ((timer_list[i].opt.is_alive == 0) ||
            (timer_list[i].opt.is_active == 0)) {
            continue;
        }
        
        if ((stmr_ticks - timer_list[i].timer_count) >= timer_list[i].time_out) 
		{
            TMR_CALLBACK(timer_list[i]);
            if (timer_list[i].timer_flag == IFS_STMR_FLAG_PERIOD)//period
			{
                timer_list[i].timer_count = stmr_ticks;
            } 
			else if (timer_list[i].timer_flag == IFS_STMR_FLAG_ONESHOT) //one shot
			{   
                timer_list[i].opt.is_active = 0;
            } 
			else//one shot and delete
			{
                timer_list[i].opt.is_alive = 0;
                timer_list[i].opt.is_active = 0;
            }
			
        }
        ret++;
    }
    return ret;
}
