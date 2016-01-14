#ifndef __NODE_RANK_ALLOCATION_H__
#define __NODE_RANK_ALLOCATION_H__

#include <mpi.h>
#include <stdlib.h>

int MpiNodeRank(MPI_Comm comm, int mpiRank);
int GetHostName(char **hostnamePtr, size_t *hostnameLength);

#endif
