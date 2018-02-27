#ifndef JIFFIES_H
#define JIFFIES_H


// extern volatile unsigned long jiffies;

extern unsigned long ticks;
extern u32 get_jiffies(void);

#define SYS_TICKS                   ticks//ms

#define time_after(a,b)			    ((long)(b) - (long)(a) < 0)
#define time_before(a,b)		    time_after(b, a)


#define msecs_to_jiffies(msec)      ((msec)/SYS_TICKS)
#define jiffies_to_msecs(j)         ((j)*SYS_TICKS)


#define AT_SYS_TIMER_CODE           AT(.sys_timer_code)


#endif

