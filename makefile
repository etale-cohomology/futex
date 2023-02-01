CC = gcc-8
T  = t

all: futex  srv clt

clean:
	rm -f  futex  srv clt

futex: futex.c futex.h
	$(T) $(CC) $< -o $@  -lpthread

srv: srv.c futex.h
	$(T) $(CC) $< -o $@  -lpthread
clt: clt.c futex.h
	$(T) $(CC) $< -o $@  -lpthread
