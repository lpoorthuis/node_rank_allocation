#ifndef __NODE_RANK_ALLOCATION_H__
#define __NODE_RANK_ALLOCATION_H__

#include <mpi.h>

#include <stdlib.h>
#include <errno.h>

#include <stddef.h>
#include <stdint.h>

#include <unistd.h>

#ifndef HOST_NAME_MAX
    #define HOST_NAME_MAX 64
#endif

#define LOCAL_HOSTNAME_MAX 256

#ifndef MAX
    #define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

typedef int bool;
#define true 1
#define false 0

static int NodeRankByHash(MPI_Comm comm, int mpiRank);
static uint32_t Adler32(const void *buf, size_t buflength);

int MpiNodeRank(MPI_Comm comm, int mpiRank);
int GetHostName(char **hostnamePtr, size_t *hostnameLength);

#endif
