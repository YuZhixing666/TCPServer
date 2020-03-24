#ifndef TIMER_H
#define TIMER_H

#define DEFAULT_TIMEOUT  10000    /* ms */

typedef time_t        sec_t;    /* Seconds */
typedef unsigned long msec_t;   /* Milliseconds */


void event_timer_init(void);
void event_time_update(void);
msec_t event_find_timer(void);
void event_expire_timers(void);
void event_add_timer(pthread_t tid, int fd, msec_t timer);

#endif