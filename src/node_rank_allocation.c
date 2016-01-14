#include "node_rank_allocation.h"

#include <errno.h>

#include <stddef.h>
#include <stdint.h>

#include <unistd.h>

#ifndef HOST_NAME_MAX
    // see the man page of 'gethostname'
    #define HOST_NAME_MAX 64
#endif

#define LOCAL_HOSTNAME_MAX 256

#ifndef MAX
    #define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

static int NodeRankByHash(MPI_Comm comm, int mpiRank);
static uint32_t Adler32(const void *buf, size_t buflength);

typedef int bool;
#define true 1
#define false 0

int MpiNodeRank(MPI_Comm comm, int mpiRank) {
    int nodeRank = -1;

    #ifdef NODE_RANK_BY_SHM
        returnValue = NodeRankByShm(comm, mpiRank, nodeRank);
    #else
        nodeRank = NodeRankByHash(comm, mpiRank);
    #endif

    return nodeRank;
}

static int NodeRankByHash(MPI_Comm comm, int mpiRank) {
    int error;

    char*  hostname       = NULL;
    size_t hostnameLength = 0;

    error = getHostName(&hostname, &hostnameLength);
    if (error != 0) {
        return -1;
    }

    uint32_t checkSum = Adler32(hostname, hostnameLength);

    int commRank = -1;
    MPI_Comm_rank(comm, &commRank);

    MPI_Comm nodeComm = MPI_COMM_NULL;

    MPI_Comm_split(comm, checkSum, mpiRank, &nodeComm);

    int nodeRank;
    MPI_Comm_rank(nodeComm, &nodeRank);

    int nodeSize;
    MPI_Comm_size(nodeComm, &nodeSize);

    printf("%d/%d\n", nodeSize, nodeRank);

    // now check for collision with hashed hostnames
    int nSend = MAX(HOST_NAME_MAX, LOCAL_HOSTNAME_MAX);
    char *send = (char *) malloc(sizeof(char)*nSend);

    strncpy(send, hostname, nSend);

    send[nSend-1] = 0x00;

    char *recv = (char *) malloc(sizeof(char)*nSend*nodeSize);
    MPI_Allgather(send, nSend, MPI_CHAR, recv, nSend, MPI_CHAR, nodeComm);

    char *neighbor = recv;
    int localNodeRank = 0;

    #define STREQ(a, b) (strcmp((a), (b)) == 0)

    for(int i=0; i<nodeSize; i++) {
        if(STREQ(send, neighbor)) {
            if (i < nodeRank) {
                ++localNodeRank;
            } else {
                // collision
            }

            neighbor += nSend;
        }
    }

    #undef streq

    if (nodeRank != localNodeRank) {
         nodeRank = localNodeRank;
    }

    // Clean up
    free(send); send = NULL;
    free(recv); recv = NULL;

    MPI_Comm_free(&nodeComm);

    free(hostname), hostname = NULL;

    return nodeRank;
}

int getHostName(char** hostnamePtr, size_t* hostnameLength) {
    char*  hostname  = NULL;
    size_t nHostname = 0;

    bool allocateMore = false;

    *hostnamePtr    = NULL;
    *hostnameLength = 0;

    do {
        nHostname += MAX(HOST_NAME_MAX, LOCAL_HOSTNAME_MAX);

        hostname = (char*) malloc(sizeof(char)*nHostname);

        if (hostname == NULL) {
            return 1;
        }

        int error;

        error  = gethostname(hostname, nHostname);

        if (error == -1) {
            if (error == ENAMETOOLONG) {
                allocateMore = true;
                free(hostname); hostname = NULL;
            } else {
                free(hostname);
                hostname = NULL;

                return 1;
            }
        } else {
            allocateMore = false;
        }
    } while(allocateMore);

    hostname[nHostname - 1] = 0x00;

    *hostnameLength = strnlen(hostname, nHostname) + 1;
    *hostnamePtr = hostname;

    return 0;
}

static uint32_t Adler32(const void *buf, size_t buflength) {
    const uint8_t *buffer = (const uint8_t *) buf;

    uint32_t s1 = 1;
    uint32_t s2 = 0;

    for (size_t n=0; n<buflength; n++) {
        s1 = (s1 + buffer[n]) % 65521;
        s2 = (s2 + s1) % 65221;
    }

    return (s2 << 16) | s1;
}

int main(int argc, char** argv) {
    int world_rank, world_size;
    int node_rank, node_size;
    int type;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    type=2;

    switch(type) {
        case 1:
        {
            char name[MPI_MAX_PROCESSOR_NAME];
            int  resultlen;

            MPI_Get_processor_name(name, &resultlen);

            printf("%s %d\n", name, resultlen);
            break;
        }
        case 2:
        {
            node_rank = MpiNodeRank(MPI_COMM_WORLD, world_rank);
        }
        default:
        {
            break;
        }
    }    

    printf("world:%d node:%d\n", world_rank, node_rank);

    MPI_Finalize();
}
