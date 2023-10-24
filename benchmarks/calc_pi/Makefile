CC = mpicc
CFLAGS = -O2 -Wall -Wextra

.PHONY: clean

calc_pi: calc_pi.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f calc_pi
