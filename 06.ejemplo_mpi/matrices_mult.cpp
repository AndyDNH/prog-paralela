#include <iostream>
#include <fmt/core.h>
#include <mpi.h>
#include <vector>
#include <cmath>

#define MATRIZ_DIM 25

void imprimir_vector(const std::vector<double> &b, int rows, int cols)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            int index = i * cols + j;
            fmt::print("{:.2f} ", b[index]);
        }
        fmt::print("\n");
    }
}

void multiplicar_matriz_vector(
    const std::vector<double> &A,
    const std::vector<double> &b,
    std::vector<double> &x,
    int rows,
    int cols)
{
    for (int i = 0; i < rows; i++)
    {
        double sum = 0.0;

        for (int j = 0; j < cols; j++)
        {
            int index = i * cols + j;
            sum += A[index] * b[j];
        }

        x[i] = sum;
    }
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int nprocs, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int rows_per_rank = std::ceil(MATRIZ_DIM * 1.0 / nprocs);
    int total_rows = rows_per_rank * nprocs;
    int padding = total_rows - MATRIZ_DIM;

    std::vector<double> A;
    std::vector<double> b(MATRIZ_DIM);
    std::vector<double> x;

    if (rank == 0)
    {
        A.resize(total_rows * MATRIZ_DIM, 0.0);
        x.resize(total_rows, 0.0);

        for (int i = 0; i < MATRIZ_DIM; i++)
        {
            for (int j = 0; j < MATRIZ_DIM; j++)
            {
                A[i * MATRIZ_DIM + j] = i;
            }

            b[i] = 1.0;
        }

        fmt::print("MATRIZ_DIM: {}, NPROCS: {}, rows_per_rank: {}, padding: {}\n",
                   MATRIZ_DIM, nprocs, rows_per_rank, padding);
    }

    MPI_Bcast(
        b.data(),
        MATRIZ_DIM,
        MPI_DOUBLE,
        0,
        MPI_COMM_WORLD);

    std::vector<double> A_local(rows_per_rank * MATRIZ_DIM);

    MPI_Scatter(
        rank == 0 ? A.data() : nullptr,
        rows_per_rank * MATRIZ_DIM,
        MPI_DOUBLE,
        A_local.data(),
        rows_per_rank * MATRIZ_DIM,
        MPI_DOUBLE,
        0,
        MPI_COMM_WORLD);

    fmt::print("RANK: {} procesando {} x {}\n", rank, rows_per_rank, MATRIZ_DIM);

    std::vector<double> x_local(rows_per_rank);

    multiplicar_matriz_vector(
        A_local,
        b,
        x_local,
        rows_per_rank,
        MATRIZ_DIM);

    MPI_Gather(
        x_local.data(),
        rows_per_rank,
        MPI_DOUBLE,
        rank == 0 ? x.data() : nullptr,
        rows_per_rank,
        MPI_DOUBLE,
        0,
        MPI_COMM_WORLD);

    if (rank == 0)
    {
        fmt::print("\n--- Resultados ---\n");
        imprimir_vector(x, MATRIZ_DIM, 1);
    }

    MPI_Finalize();
    return 0;
}