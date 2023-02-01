/*
t gcc-8 futex.c -o futex  -lpthread  &&  t ./futex
t gcc-8 clt.c   -o clt    -lpthread  &&  t ./clt
t gcc-8 srv.c   -o srv    -lpthread  &&  t ./srv
*/
#include <mathisart4.h>

// ----------------------------------------------------------------------------------------------------------------------------# @blk1  libfutex
/* The main problem with this particular @futex_wait()/futex_wake() pair is that it's a BUSY LOOP (to avoid so-called "race conditions"), which I don't know how to avoid, which DEFEATS THE PURPOSE OF THE FUTEX.
*/
#include <linux/futex.h>

int futex(int* uaddr, int futex_op, int val, const struct timespec* timeout, int* uaddr2, int val3){
	return syscall(SYS_futex, uaddr, futex_op, val, timeout, uaddr, val3);
}

void futex_wait(int* ftx, int val){  // busy loop until @*ftx becomes @val, then wait until FUTEX_WAKE. this is a busy loop that consumes 80% of a cpu if unchecked... what's the point of futexes then?
	// struct timespec ts = {tv_sec:0, tv_nsec:4*1000l*1000l};
	for(;;){  // WARN! the braces are REQUIRED, else the `else if(st==0)` gets bound `if(errno!=EAGAIN)`, when we want it to bind to `if(st==-1)`
		int st=futex(ftx, FUTEX_WAIT, val, NULL, NULL, 0);  // if @*ftx is @val, wait until a FUTEX_WAKE, else ret immediately. (if it rets immediately, we must try again... WHAT'S THE POINT, THEN?) rets 0 if the caller received a FUTEX_WAKE (ie. if the caller was woken up)
		if(     st==-1){  if(errno!=EAGAIN) fail();  }  // chk not vals over 1 to save 1 line of code (is it even possible for @futex(FUTEX_WAIT) to ret over 1?
		else if(st== 0){  if(*ftx==val)     return;  }  // must chk @*ftx==val in case @st==0 is a spurious wake up?
		// printf("\x1b[34m%d \x1b[35m%s \x1b[32m%d\x1b[0m\n", getpid(),__func__,st);
	}
}

void futex_wake(int* ftx){  // busy loop until the caller successfully sends (and someone receives) a FUTEX_WAKE. this is a busy loop that consumes 80% of a cpu if unchecked... what's the point of futexes then?
	for(;;){  // this for-loop seems needed
		int st=futex(ftx, FUTEX_WAKE, 1, NULL, NULL, 0);  chks(st);  // wake up at most @val waiters (ie. processes that called FUTEX_WAIT) on @ftx. (if it wakes up no one, we must try again... WHAT'S THE POINT, THEN?) (often @val is 1 (wake up 1 waiter) or INT_MAX (wake up all waiters). which waiters are awoken is undefined.) rets the number of waiters that were woken up. calling @futex(FUTEX_WAKE) w/ val==1 is a SIGNIFICANT OPTIMIZATION (https://akkadia.org/drepper/futex.pdf)
		if(0<st) return;
		// printf("\x1b[34m%d \x1b[35m%s \x1b[32m%d\x1b[0m\n", getpid(),__func__,st);
	}
}

// ----------------------------------------------------------------------------------------------------------------------------# @blk1  libshm
#include <sys/mman.h>

#define SHM_PATH "/dev/shm/smem00"
#define SHM_PDIM 1                  // page dimension

#define ST_NULL 0x00  // state: null
#define ST_R2RD 0x10  // state: ready to read
#define ST_R2WR 0x11  // state: ready to write

tdef{
	char* path;
	i64   bdim;  // total size of the @mmap()-ed buf. useful for @munmap()
	int   fd;
	u32*  base;
	i32   running;
}shm_t;

#define shm_futex_pget(shm)   (((i32*)(shm).base)+0x00)
#define shm_futex1_pget(shm)  (((i32*)(shm).base)+0x01)
#define shm_bdim_pget(shm)    (((i64*)(shm).base)+0x01)
#define shm_data_pget(shm)    (((i64*)(shm).base)+0x02)

void shm_cmd_handle(shm_t* shm){
	if(memcmp(shm_data_pget(*shm), "cmd exit\n", ((i64*)shm->base)[0x01])==0)              shm->running = 0;
	if(memcmp(shm_data_pget(*shm), "Jesus is God of gods\n", ((i64*)shm->base)[0x01])==0)  puts("Yes He is!!!");
	else                                                                                   return;
}
