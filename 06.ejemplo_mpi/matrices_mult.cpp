#include <iostream>
#include <fmt/core.h>
#include <mpi.h>
#include <vector>
#include <cmath>
#include <algorithm>

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

    int rows_per_rank = std::ceil((MATRIZ_DIM) * 1.0 / nprocs);

    auto filas_para_rank = [&](int proc)
    {
        int inicio = proc * rows_per_rank;
        if (inicio >= MATRIZ_DIM)
        {
            return 0;
        }
        return std::min(rows_per_rank, MATRIZ_DIM - inicio);
    };

    auto offset_para_rank = [&](int proc)
    {
        return proc * rows_per_rank;
    };

    // Todo lo que hace el proceso 0 debe ir dentro de este bloque
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
        }

        for (int i = 0; i < MATRIZ_DIM; i++)
        {
            b[i] = 1;
        }

        int padding = rows_per_rank * nprocs - MATRIZ_DIM;

        fmt::print("MATRIZ_DIM: {}, NPROCS: {}, rows_per_rank: {}, padding: {}\n",
                   MATRIZ_DIM, nprocs, rows_per_rank, padding);

        // enviar dimesiones y datos
        for (int i = 1; i < nprocs; i++)
        {
            int filas = filas_para_rank(i);
            int inicio = offset_para_rank(i);

            // enviar dimensiones
            std::vector<int> data = {MATRIZ_DIM, filas};

            MPI_Send(
                data.data(),   
                2,             
                MPI_INT,      
                i,      
                0,            
                MPI_COMM_WORLD 
            );

            const double *buffer = A.data();
            MPI_Send(
                filas > 0 ? &buffer[inicio * MATRIZ_DIM] : buffer, 
                filas * MATRIZ_DIM,                                
                MPI_DOUBLE,                                        
                i,                                                 
                0,                                                 
                MPI_COMM_WORLD                                     
            );
            MPI_Send(
                b.data(),      // buffer
                MATRIZ_DIM,    // count
                MPI_DOUBLE,    // Tipo de datos
                i,             // RANK destino
                0,             // TAG
                MPI_COMM_WORLD // Grupo
            );
        }

        // -----------------------------------ENVIO DE DATOS A LOS PROCESOS----------------------------------

        int filas_locales = filas_para_rank(0);
        std::vector<double> A_local(A.begin(), A.begin() + filas_locales * MATRIZ_DIM);
        std::vector<double> x_local(filas_locales);
        multiplicar_matriz_vector(A_local, b, x_local, filas_locales, MATRIZ_DIM);

        for (int i = 0; i < filas_locales; i++)
        {
            x[i] = x_local[i];
        }

        for (int i = 1; i < nprocs; i++)
        {
            int filas = filas_para_rank(i);
            std::vector<double> x_parcial(filas);

            MPI_Recv(
                x_parcial.data(),
                filas,
                MPI_DOUBLE,
                i,
                1,
                MPI_COMM_WORLD,
                MPI_STATUS_IGNORE);

            int inicio = offset_para_rank(i);
            for (int j = 0; j < filas; j++)
            {
                x[inicio + j] = x_parcial[j];
            }
        }

        fmt::print("Resultado global de la multiplicacion:\n");
        imprimir_vector(x, MATRIZ_DIM, 1);
        fmt::print("RANK: {}, {} x {}\n", rank, rows_per_rank, MATRIZ_DIM);
    }
    else
    {
        std::vector<int> data_rec(2);

        MPI_Recv(
            data_rec.data(), 
            2,               
            MPI_INT,        
            0,               
            0,
            MPI_COMM_WORLD,
            MPI_STATUS_IGNORE);

        int matrix_dim = data_rec[0];
        int rows = data_rec[1];

        fmt::print("RANK: {}, {} x {}\n", rank, rows, matrix_dim);

        std::vector<double> A_local(rows * matrix_dim);
        std::vector<double> b_local(matrix_dim);
        MPI_Recv(
            A_local.data(),
            rows * matrix_dim,
            MPI_DOUBLE,
            0,
            0,              
            MPI_COMM_WORLD, 
            MPI_STATUS_IGNORE);

        MPI_Recv(
            b_local.data(), 
            matrix_dim,     
            MPI_DOUBLE,     
            0,              
            0,              
            MPI_COMM_WORLD, 
            MPI_STATUS_IGNORE);

        if (rank == 1)
        {
            imprimir_vector(A_local, rows, matrix_dim);
        }

        std::vector<double> x_local(rows); 
        multiplicar_matriz_vector(A_local, b_local, x_local, rows, matrix_dim);

        MPI_Send(
            x_local.data(),
            rows,
            MPI_DOUBLE,
            0,
            1,
            MPI_COMM_WORLD);

        if (rank == 1)
        {
            fmt::print("Resultado de la multiplicacion:\n");
            imprimir_vector(x_local, rows, 1);
        }
    }

    MPI_Finalize();
    return 0;
}