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

	// ----------------------------------------------------------------------------------------------------------------------------#
	while(shm.running){
		*shm_futex_pget(shm) = ST_R2WR;                                                                                                                                                 // srv: put ST_R2WR on smem and wake up the clt
		printf("\x1b[34m%7d \x1b[31m%-3s \x1b[35m%02x  \x1b[33mput  \x1b[32m%02x\x1b[0m\n", getpid(),"srv", *shm_futex_pget(shm), ST_R2WR);  futex_wake(shm_futex_pget(shm));           // srv: busy loop until the clt receives the srv's FUTEX_WAKE
		printf("\x1b[34m%7d \x1b[31m%-3s \x1b[35m%02x  \x1b[33mwait \x1b[32m%02x\x1b[0m\n", getpid(),"srv", *shm_futex_pget(shm), ST_R2RD);  futex_wait(shm_futex_pget(shm), ST_R2RD);  // srv: busy loop NOT until smem becomes ST_R2RD, but until the clt himself signals ST_R2RD. then read
		printf("\x1b[34m%7d \x1b[31m%-3s \x1b[35m%02x  \x1b[0m%.*s", getpid(),"srv", *shm_futex_pget(shm), *shm_bdim_pget(shm), shm_data_pget(shm));                                    // srv: read

		shm_cmd_handle(&shm);
	}

	printf("\x1b[34m%7d \x1b[31m%-3s \x1b[35m%02x  \x1b[33mbye\n", getpid(),"srv", *shm_futex_pget(shm));
	chks(munmap(shm.base, shm.bdim));
	chks(close(shm.fd));
	chks(unlink(shm.path));
	return 0;
}
