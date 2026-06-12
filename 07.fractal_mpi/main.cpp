#include <iostream>
#include <fmt/core.h>
#include <complex>
#include <mpi.h>
#include <SFML/Graphics.hpp>

#include "fractal_serial_mpi.h"

#ifdef _WIN32
#include <windows.h>
#endif

#define WIDTH 1600
#define HEIGHT 900

// -- parametros
int max_iteraciones = 10;

double x_min = -1.5;
double x_max = 1.5;
double y_min = -1.0;
double y_max = 1.0;

uint32_t *pixel_buffer = nullptr;
uint32_t *texture_buffer = nullptr;
int running = 1;

std::complex<double> c(-0.7, 0.27015);

void setup_ui()
{
    texture_buffer = new uint32_t[WIDTH * HEIGHT];
    std::memset(texture_buffer, 0, WIDTH * HEIGHT * sizeof(uint32_t));

    // -- dibujar
    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "Julia Set - SFML");

#ifdef _WIN32
    HWND hwnh = window.getNativeHandle();
    ShowWindow(hwnh, SW_MAXIMIZE);
#endif
    sf::Texture texture({WIDTH, HEIGHT});
    sf::Sprite sprite(texture);

    int frames = 0;
    int fps = 0;
    sf::Clock clock;

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                running = 0;
                window.close();
            }
            else if (event->is<sf::Event::KeyReleased>())
            {
                auto evt = event->getIf<sf::Event::KeyReleased>();

                switch (evt->scancode)
                {
                case sf::Keyboard::Scan::Up:
                    max_iteraciones += 10;
                    break;
                case sf::Keyboard::Scan::Down:
                    max_iteraciones -= 10;
                    if (max_iteraciones < 10)
                        max_iteraciones = 10;
                    break;
                }
            }

            std::memset(texture_buffer, 0, WIDTH * HEIGHT * sizeof(uint32_t));
        }

        std::vector<int> dummy = {max_iteraciones, running};
        MPI_Bcast(dummy.data(), 2, MPI_INT, 0, MPI_COMM_WORLD);
        // texture.update((const uint8_t *)pixel_buffer);
        frames++;

        if (clock.getElapsedTime().asSeconds() >= 1.0f)
        {
            fps = frames;
            frames = 0;
            clock.restart();
        }
        // acttualziar el titulo
        // auto msg = fmt::format("Julia Set: Iteraciones:{} , FPS:{} , Mode:{}", max_iteraciones, fps, mode);
        // text.setString(msg);

        // dibujar

        window.clear();
        {
            // window.draw(sprite);
            // window.draw(text);
            // window.draw(textOptions);
        }
        window.display();
    }
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int nprocs, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int delta = std::ceil(HEIGHT * 1.0 / nprocs); // 1600/4 = 400
    int row_start = rank * delta;
    int row_end = row_start + delta;

    // ro: start= 0 *400, end = 0+400 = 400
    // ro: start= 1 *400, end = 400+400 = 800
    // ro: start= 2 *400, end = 800+400 = 1200
    // ro: start= 3 *400, end = 1200+400 = 1600
    int padding = delta * nprocs - HEIGHT; // 400 * 4 - 1600 = 0

    if (row_end > HEIGHT)
    {
        row_end = HEIGHT;
    }

    pixel_buffer = new uint32_t[WIDTH * delta];
    std::memset(pixel_buffer, 0, WIDTH * delta * sizeof(uint32_t));

    fmt::println("rank: {}, rows: {} to: {}", rank, row_start, row_end);

    if (rank == 0)
    {
        setup_ui();
    }
    else
    {
        // dibujar
        while (true)
        {
            std::vector<int> dummy = {max_iteraciones, 0};
            MPI_Bcast(dummy.data(), 2, MPI_INT, 0, MPI_COMM_WORLD);
            
            max_iteraciones = dummy[0];
            running = dummy[1];
            if (running == 0)
            {
                fmt::println("rank: {} exiting...", rank);
                break;
            }
            julia_mpi(x_min, y_min, x_max, y_max, WIDTH, HEIGHT, row_start, row_end, pixel_buffer);
            // comprobacion del rango
            if(rank == 1){
                fmt::println("rank: {} : max_iteraciones = {}", rank, max_iteraciones);
                std::cout.flush();
            }




        }

        // {
        //
        // }
    }

    MPI_Finalize();
    return 0;
}