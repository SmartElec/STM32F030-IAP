#ifndef __SOFTTIMER_H_
#define __SOFTTIMER_H_ 

#include <stdint.h>
#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif

//#define IFS_STMR_FLAG_ONESHOT 0
//#define IFS_STMR_FLAG_PERIOD  1
typedef enum
{
	IFS_STMR_FLAG_ONESHOT,
	IFS_STMR_FLAG_PERIOD,
	IFS_STMR_FLAG_DELETE
}IFS_STMR_FLAG;

typedef signed char ifs_int8_t;
typedef unsigned char ifs_uint8_t;
typedef signed short ifs_int16_t;
typedef unsigned short ifs_uint16_t;
typedef signed long ifs_int32_t;
typedef unsigned long ifs_uint32_t;
typedef int ifs_stmr_handle;
typedef void (*timer_out_callback)(void*);

extern ifs_uint32_t stmr_ticks;

void ifs_stmr_init(void);
ifs_stmr_handle ifs_stmr_registered(ifs_uint32_t period, 
                                      timer_out_callback cb,
                                      void *cb_param,
                                      IFS_STMR_FLAG flag);
int ifs_stmr_delete(ifs_stmr_handle handle);
int ifs_stmr_start(ifs_stmr_handle handle);
int ifs_stmr_stop(ifs_stmr_handle handle);
void ifs_stmr_interrupt(void);
int ifs_stmr_task(void);									  
  
#ifdef __cplusplus
}
#endif


#endif
