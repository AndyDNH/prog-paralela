// #include <iostream>
// #include <fmt/core.h>
// #include <mpi.h>
// #include <vector>
// #include <cmath> 

// #define MATRIZ_DIM 25 

// void imprimir_matriz(const std::vector<double> &A, int rows, int cols)
// {
//     for (int i = 0; i < rows; i++)
//     {
//         for (int j = 0; j < cols; j++)
//         {
//             int index = i * cols + j;
//             fmt::print("{:.2f} ", A[index]);
//         }
//         fmt::print("\n");
//     }
// }

// void imprimir_vector(const std::vector<double> &b, int rows, int cols)
// {
//     for (int i = 0; i < rows; i++)
//     {
//         for (int j = 0; j < cols; j++)
//         {
//             int index = i * cols + j;
//             fmt::print("{:.2f} ", b[index]);
//         }
//         fmt::print("\n");
//     }
// }

// void multiplicar_matriz_vector(const std::vector<double> &A, const std::vector<double> &b, std::vector<double> &x, int rows, int cols)
// {
//     for (int i = 0; i < rows; i++)
//     {
//         double sum = 0;
//         x[i] = 0;
//         for (int j = 0; j < cols; j++)
//         {
//             int index = i * cols + j;
//             sum += A[index] * b[j];
//         }
//         x[i] = sum;
//     }
// }

// int main(int argc, char **argv)
// {
//     MPI_Init(&argc, &argv);

//     int nprocs, rank;
//     MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
//     MPI_Comm_rank(MPI_COMM_WORLD, &rank);

//     int rows_per_rank = std::ceil((MATRIZ_DIM) * 1.0 / nprocs);
//     int padding = rows_per_rank * nprocs - MATRIZ_DIM;

//     if (rank == 0)
//     {
//         std::vector<double> A(MATRIZ_DIM * MATRIZ_DIM); 
//         std::vector<double> b(MATRIZ_DIM);              
//         std::vector<double> x(MATRIZ_DIM);              

//         for (int i = 0; i < MATRIZ_DIM; i++)
//         {
//             for (int j = 0; j < MATRIZ_DIM; j++)
//             {
//                 int index = i * MATRIZ_DIM + j;
//                 A[index] = i;
//             }
//             b[i] = i; 
//         }

//         fmt::print("MATRIZ_DIM: {}, NPROCS: {}, rows_per_rank: {}, padding: {}\n",
//                    MATRIZ_DIM, nprocs, rows_per_rank, padding);

//         for (int i = 1; i < nprocs; i++)
//         {
//             int filas = rows_per_rank;
//             if (i == nprocs - 1)
//             {
//                 filas = rows_per_rank - padding;
//             }

//             std::vector<int> data = {MATRIZ_DIM, filas};
//             MPI_Send(data.data(), 2, MPI_INT, i, 0, MPI_COMM_WORLD);

//             const double *buffer = A.data();
//             MPI_Send(&buffer[i * rows_per_rank * MATRIZ_DIM], filas * MATRIZ_DIM, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);

//             MPI_Send(b.data(), MATRIZ_DIM, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
//         }

//         fmt::print("RANK: 0 procesando {} x {}\n", rows_per_rank, MATRIZ_DIM);
//         multiplicar_matriz_vector(A, b, x, rows_per_rank, MATRIZ_DIM);

//         for (int i = 1; i < nprocs; i++)
//         {
//             int filas = rows_per_rank;
//             if (i == nprocs - 1)
//             {
//                 filas = rows_per_rank - padding;
//             }
//             MPI_Recv(&x[i * rows_per_rank], filas, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//         }

//         fmt::print("\n--- Resultado Final del Vector X ---\n");
//         imprimir_vector(x, MATRIZ_DIM, 1);
//     }
//     else
//     {
//         std::vector<int> data_rec(2);

//         MPI_Recv(data_rec.data(), 2, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

//         int matrix_dim = data_rec[0];
//         int rows = data_rec[1];

//         fmt::print("RANK: {}, inicializando std::vector con tamaño: {} x {}\n", rank, rows, matrix_dim);
        
//         std::vector<double> A_local(rows * matrix_dim); 
//         std::vector<double> b_local(MATRIZ_DIM);        

//         MPI_Recv(A_local.data(), rows * matrix_dim, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
//         MPI_Recv(b_local.data(), MATRIZ_DIM, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

//         if (rank == 1)
//         {
//             fmt::print("RANK 1 recibió su porción de la matriz correctamente.\n");
//         }

//         std::vector<double> x_local(rows);
//         multiplicar_matriz_vector(A_local, b_local, x_local, rows, matrix_dim);

//         MPI_Send(x_local.data(), rows, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
//     }

//     MPI_Finalize();
//     return 0;
// }