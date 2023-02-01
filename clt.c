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
	shm.base = mmap(NULL,shm.bdim, PROT_READ|PROT_WRITE, MAP_SHARED, shm.fd,0x00);  chks(shm.base);

	// ----------------------------------------------------------------------------------------------------------------------------#
	u8  buf_data[0x100];
	i32 buf_bdim;

	while(shm.running){
		fgets(buf_data,Bsize(buf_data)-1, stdin);

		printf("\x1b[34m%7d \x1b[32m%-3s \x1b[35m%02x  \x1b[33mwait \x1b[32m%02x\x1b[0m\n", getpid(),"clt", *shm_futex_pget(shm), ST_R2WR);  futex_wait(shm_futex_pget(shm), ST_R2WR);  // clt: busy loop until smem becomes ST_R2WR, then write
		*shm_bdim_pget(shm) = strlen(buf_data);                                                                                                                                         // clt: write
		memcpy(shm_data_pget(shm), buf_data, mmin(shm.bdim,*shm_bdim_pget(shm)));                                                                                                       // clt: write
		printf("\x1b[34m%7d \x1b[32m%-3s \x1b[35m%02x  \x1b[0m%.*s", getpid(),"clt", *shm_futex_pget(shm), *shm_bdim_pget(shm), shm_data_pget(shm));                                    // clt: write
		*shm_futex_pget(shm) = ST_R2RD;                                                                                                                                                 // clt: put ST_R2RD on smem to wake up the srv
		printf("\x1b[34m%7d \x1b[32m%-3s \x1b[35m%02x  \x1b[33mput  \x1b[32m%02x\x1b[0m\n", getpid(),"clt", *shm_futex_pget(shm), ST_R2RD);  futex_wake(shm_futex_pget(shm));           // clt: busy loop until the srv receives the clt's FUTEX_WAKE

		shm_cmd_handle(&shm);
	}

	printf("\x1b[34m%7d \x1b[31m%-3s \x1b[35m%02x  \x1b[33mbye\n", getpid(),"clt", *shm_futex_pget(shm));
	chks(munmap(shm.base, shm.bdim));
	chks(close(shm.fd));
	return 0;
}
