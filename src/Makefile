PROGRAM    = node_rank_allocation
OBJ         = ${PROGRAM:=.o}

ARCH        = _LINUX
CC          = mpicc
USER_CFLAGS = -D${ARCH}

$(PROGRAM): $(OBJ)
	$(CC) $(USER_CFLAGS) -o $(PROGRAM).out $(OBJ)

.c.o:
	$(CC) $(USER_CFLAGS) -c $<

clean:
	rm -f *.o $(PROGRAM).out
