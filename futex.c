#include "futex.h"

// ----------------------------------------------------------------------------------------------------------------------------# @blk1
int main(int nargs, char* args[]){
	shm_t shm = {
		path:    "/dev/shm/smem00",
		bdim:    SHM_PDIM*sysconf(_SC_PAGE_SIZE),
		running: 1,
	};

	shm.fd = open(SHM_PATH, O_CREAT|O_RDWR, 0b110000000);  chks(shm.fd);
	chks(ftruncate(shm.fd, shm.bdim));
	shm.base             = mmap(NULL,shm.bdim, PROT_READ|PROT_WRITE, MAP_SHARED, shm.fd,0x00);  chks(shm.base);
	*shm_futex_pget(shm) = ST_R2RD;

	pid_t pid=fork();  chks(pid);  // father: gets the son's pid; son: gets not the father's pid

	// ----------------------------------------------------------------------------------------------------------------------------#
	if(0<pid){  // father
		while(shm.running){
			*shm_futex_pget(shm) = ST_R2WR;                                                                                                                                                    // father: put ST_R2WR on smem and wake up the son
			printf("\x1b[34m%7d \x1b[31m%-6s \x1b[35m%02x  \x1b[33mput  \x1b[32m%02x\x1b[0m\n", getpid(),"father", *shm_futex_pget(shm), ST_R2WR);  futex_wake(shm_futex_pget(shm));           // father: busy loop until the son receives the father's FUTEX_WAKE
			printf("\x1b[34m%7d \x1b[31m%-6s \x1b[35m%02x  \x1b[33mwait \x1b[32m%02x\x1b[0m\n", getpid(),"father", *shm_futex_pget(shm), ST_R2RD);  futex_wait(shm_futex_pget(shm), ST_R2RD);  // father: busy loop NOT until smem becomes ST_R2RD, but until the son himself signals ST_R2RD. then read
			printf("\x1b[34m%7d \x1b[31m%-6s \x1b[35m%02x  \x1b[0m%.*s", getpid(),"father", *shm_futex_pget(shm), *shm_bdim_pget(shm), ((i64*)shm.base)+0x02);                              // father: read

			shm_cmd_handle(&shm);
		}

		printf("\x1b[34m%7d \x1b[31m%-6s \x1b[35m%02x  \x1b[33mbye\n", getpid(),"father", *shm_futex_pget(shm));
		chks(munmap(shm.base, shm.bdim));
		chks(close(shm.fd));
		return 0;

	// ----------------------------------------------------------------------------------------------------------------------------#
	}else if(pid==0){  // son
		u8  buf_data[0x100];
		i32 buf_bdim;

		while(shm.running){
			fgets(buf_data,Bsize(buf_data)-1, stdin);

			printf("\x1b[34m%7d \x1b[32m%-6s \x1b[35m%02x  \x1b[33mwait \x1b[32m%02x\x1b[0m\n", getpid(),"son", *shm_futex_pget(shm), ST_R2WR);  futex_wait(shm_futex_pget(shm), ST_R2WR);  // son: busy loop until smem becomes ST_R2WR, then write
			*shm_bdim_pget(shm) = strlen(buf_data);                                                                                                                                      // son: write
			memcpy(((i64*)shm.base)+0x02, buf_data, mmin(shm.bdim,*shm_bdim_pget(shm)));                                                                                                 // son: write
			printf("\x1b[34m%7d \x1b[32m%-6s \x1b[35m%02x  \x1b[0m%.*s", getpid(),"son", *shm_futex_pget(shm), *shm_bdim_pget(shm), ((i64*)shm.base)+0x02);                              // son: write
			*shm_futex_pget(shm) = ST_R2RD;                                                                                                                                                 // son: put ST_R2RD on smem to wake up the father
			printf("\x1b[34m%7d \x1b[32m%-6s \x1b[35m%02x  \x1b[33mput  \x1b[32m%02x\x1b[0m\n", getpid(),"son", *shm_futex_pget(shm), ST_R2RD);  futex_wake(shm_futex_pget(shm));           // son: busy loop until the father receives the son's FUTEX_WAKE

			shm_cmd_handle(&shm);
		}

		printf("\x1b[34m%7d \x1b[31m%-6s \x1b[35m%02x  \x1b[33mbye\n", getpid(),"son", *shm_futex_pget(shm));
		chks(munmap(shm.base, shm.bdim));
		chks(close(shm.fd));
		return 0;
	}
}
