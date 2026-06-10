#include <iostream>
#include <fmt/core.h>
#include <mpi.h>

int main(int argc, char** argv){
    MPI_Init(&argc, &argv);

    int nprocs, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int version,subversion;
    MPI_Get_version(&version,&subversion);

    if(rank == 0){
        fmt::print("MPI version: {}.{}\n", version, subversion);
        fmt::print("Number of processes: {}\n", nprocs);
    }

    fmt::println("RANK_{} de {} procesos", rank, nprocs);


    MPI_Finalize();
    return 0;

}