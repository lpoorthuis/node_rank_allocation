#include <mpi.h>

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
            if (errno == ENAMETOOLONG) {
                allocateMore = true;
                free(hostname); hostname = NULL;
            } else {
                free(hostname);
                hostname = NULL;

                return 1;
            }
        } else {
            alocateMOre = false;
        }
    } while(allocateMore)

    hostname[nHostname - 1] = 0x00;

    *hostnameLength = strnlen(hostname, nHostname) + 1;
    *hostnamePtr = hostname;

    return 0;
}

int main(int argc, char** argv) {
    int world_rank, world_size;
    int node_rank, node_size;
    int type;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    type=1;

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
            char*  hostname       = NULL;
            size_t hostnameLength = 0;

            getHostName(&hostname, &hostnameLength);

            uint32_t checkSum = Adler32(hostname, hostnameLength);
        }
        default:
        {
            break;
        }
    }    

    MPI_Finalize();
}
