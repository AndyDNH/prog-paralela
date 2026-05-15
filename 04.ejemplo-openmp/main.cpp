#include <iostream>
#include <fmt/core.h>
#include <omp.h>

int main() {
    // Para que OpenMP funcione, el código debe estar dentro de una región paralela
    // #pragma omp parallel num_threads(4)
    // {
    //     // Ejecucion en el hilo master
    //     #pragma omp master 
    //     {
    //         int threads_count = omp_get_num_threads();
    //         fmt::println("Goodbye serial world, hello OpenMP!");
    //         fmt::println("I have {} thread(s) ", threads_count);
    //     }
        
    //     int thread_id = omp_get_thread_num();
    //     fmt::println("My thread id is {}", thread_id);
    // }
    // #pragma omp parallel {
    //     int thread_id = omp_get_thread_num();
    //         std::string msg = "";

    //     #pragma omp parallel for
    //     {


    //         for (int i = 0; i < thread_id; ++i) {

    //             msg += "* ";

    //         }
    //         fmt::println("Thread {}: {}", thread_id, msg);
    //     }
    // }

    int numero_elementos = 15;
    #pragma omp parallel for num_threads(4)
    for (int i = 0; i < 16; ++i) {
        // fmt::println("Thread {}: {}", omp_get_thread_num(), i);
    }

    #pragma omp parallel  num_threads(4)
    {
        int thread_id = omp_get_thread_num();
        int thread_num = omp_get_num_threads();
        
        int delta = std::ceil(numero_elementos*1.0/thread_num);
        int start = thread_id * delta;
        int end = (thread_id + 1) * delta;

        if(thread_id == thread_num - 1) {
            end = numero_elementos;
        }

        // fmt::println("Thread_{}: Start={}, End={}", omp_get_thread_num(), start, end);
        
        for(int i = start; i < end; ++i) {
            // fmt::println("Thread_{}: {}", omp_get_thread_num(), i);
        }

    
    }

    #pragma omp parallel 
    {
        int thread_id = omp_get_thread_num();
        int thread_num = omp_get_num_threads();

        for(int i = thread_id; i < numero_elementos; i+=24) {
            fmt::println("Thread_id: {}, index= {}", thread_id, i);
        }
    
    }

    #pragma omp parallel 
    {
        // while(true){

        // }
    
    }

    return 0;
}