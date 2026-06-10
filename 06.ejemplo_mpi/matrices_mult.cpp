#include <iostream>
#include <fmt/core.h>
#include <mpi.h>
#include <vector>

#define MATRIZ_DIM 25 

void imprimir_matriz(const std::vector<double> &A, int rows, int cols)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            int index = i * cols + j;
            fmt::print("{:.2f} ", A[index]);
        }
        fmt::print("\n");
    }
}

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

void multiplicar_matriz_vector(const std::vector<double> &A, const std::vector<double> &b, std::vector<double> &x, int rows, int cols)
{
    for (int i = 0; i < rows; i++)
    {
        double sum = 0;
        x[i] = 0;
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

    int base_rows = MATRIZ_DIM / nprocs;
    int remainder = MATRIZ_DIM % nprocs;

    if (rank == 0)
    {
        std::vector<double> A(MATRIZ_DIM * MATRIZ_DIM); 
        std::vector<double> b(MATRIZ_DIM);              
        std::vector<double> x(MATRIZ_DIM);              

        for (int i = 0; i < MATRIZ_DIM; i++)
        {
            for (int j = 0; j < MATRIZ_DIM; j++)
            {
                int index = i * MATRIZ_DIM + j;
                A[index] = i;
            }
            b[i] = 1; 
        }

        fmt::print("MATRIZ_DIM: {}, NPROCS: {}, base_rows: {}, remainder: {}\n",
                   MATRIZ_DIM, nprocs, base_rows, remainder);

        for (int i = 1; i < nprocs; i++)
        {
            int filas = (i == nprocs - 1) ? base_rows + remainder : base_rows;

            std::vector<int> data = {MATRIZ_DIM, filas};
            MPI_Send(data.data(), 2, MPI_INT, i, 0, MPI_COMM_WORLD);

            const double *buffer = A.data();
            MPI_Send(&buffer[i * base_rows * MATRIZ_DIM], filas * MATRIZ_DIM, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);

            MPI_Send(b.data(), MATRIZ_DIM, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
        }

        int my_rows = (0 == nprocs - 1) ? base_rows + remainder : base_rows;
        fmt::print("RANK: 0 procesando {} x {}\n", my_rows, MATRIZ_DIM);
        
        multiplicar_matriz_vector(A, b, x, my_rows, MATRIZ_DIM);

        for (int i = 1; i < nprocs; i++)
        {
            int filas = (i == nprocs - 1) ? base_rows + remainder : base_rows;
            MPI_Recv(&x[i * base_rows], filas, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        fmt::print("\n--- Resultado Final del Vector X ---\n");
        imprimir_vector(x, MATRIZ_DIM, 1);
    }
    else
    {
        std::vector<int> data_rec(2);
        MPI_Recv(data_rec.data(), 2, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int matrix_dim = data_rec[0];
        int rows = data_rec[1];

        fmt::print("RANK: {}, procesando: {} x {}\n", rank, rows, matrix_dim);
        
        std::vector<double> A_local(rows * matrix_dim); 
        std::vector<double> b_local(matrix_dim);        

        MPI_Recv(A_local.data(), rows * matrix_dim, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(b_local.data(), matrix_dim, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        std::vector<double> x_local(rows);
        multiplicar_matriz_vector(A_local, b_local, x_local, rows, matrix_dim);

        MPI_Send(x_local.data(), rows, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}