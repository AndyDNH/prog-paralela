#include <iostream>
#include <fmt/core.h>
#include <complex>
#include <mpi.h>
#include <SFML/Graphics.hpp>
// #include "arial.ttf.h"

#include "draw_text.h"

#include "fractal_serial_mpi.h"

#ifdef _WIN32
#include <windows.h>
#endif

#define WIDTH 1600
#define HEIGHT 900

namespace arial_ttf
{
    extern size_t data_len;
    extern unsigned char data[];
}

// -- parametros globales y de MPI
int max_iteraciones = 10;

double x_min = -1.5;
double x_max = 1.5;
double y_min = -1.0;
double y_max = 1.0;
int frames;

uint32_t *pixel_buffer = nullptr;
uint32_t *texture_buffer = nullptr;
int running = 1;

int nprocs, rank;
int delta;
int row_start;
int row_end;
int padding;

void dibujar_texto(int rank)
{
    char host_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    
    MPI_Get_processor_name(host_name, &name_len);

    auto texto = fmt::format("RANK_{} ({})", rank, host_name);

    draw_text_to_texture(
        (unsigned char *)pixel_buffer,
        WIDTH, delta,
        texto.c_str(),
        10, 25, 20); 
}

std::complex<double> c(-0.7, 0.27015);

void setup_ui()
{
    // El Rank 0 necesita el buffer completo para la textura
    texture_buffer = new uint32_t[WIDTH * HEIGHT];
    std::memset(texture_buffer, 0, WIDTH * HEIGHT * sizeof(uint32_t));

    // -- dibujar
    sf::RenderWindow window(sf::VideoMode({WIDTH, HEIGHT}), "Julia Set - SFML");

#ifdef _WIN32
    HWND hwnh = window.getNativeHandle();
    ShowWindow(hwnh, SW_MAXIMIZE);
#endif
    sf::Texture texture({WIDTH, HEIGHT});
    texture.update((const uint8_t *)texture_buffer);
    sf::Sprite sprite(texture);

    const sf::Font font(arial_ttf::data, arial_ttf::data_len);

    sf::Text text(font, "Fractal", 24);
    text.setFillColor(sf::Color::White);
    text.setPosition({10, 10});
    text.setStyle(sf::Text::Bold);

    std::string options = " UP/DOWN : Change Iterations";
    sf::Text textOptions(font, options, 14);
    textOptions.setFillColor(sf::Color::White);
    textOptions.setStyle(sf::Text::Bold);
    textOptions.setPosition({10, window.getView().getSize().y - 40});

    frames = 0;
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
        }

        std::vector<int> dummy = {max_iteraciones, running};
        MPI_Bcast(dummy.data(), 2, MPI_INT, 0, MPI_COMM_WORLD);

        if (running == 0)
        {
            break;
        }

        julia_mpi(x_min, y_min, x_max, y_max, WIDTH, HEIGHT, row_start, row_end, pixel_buffer);
        std::memcpy(texture_buffer, pixel_buffer, WIDTH * delta * sizeof(uint32_t));
        dibujar_texto(0);

        for (int i = 1; i < nprocs; i++)
        {
            int new_delta = delta;
            if (i == nprocs - 1)
            {
                new_delta = delta - padding;
            }

            MPI_Status status;
            MPI_Recv(pixel_buffer,
                     WIDTH * delta,
                     MPI_UNSIGNED,
                     i,
                     0,
                     MPI_COMM_WORLD,
                     &status);

            // if (i < nprocs - 1)
            {
                std::memcpy(
                    texture_buffer + (i * delta * WIDTH),
                    pixel_buffer,
                    WIDTH * new_delta * sizeof(uint32_t));
            }
        }

        texture.update((const uint8_t *)texture_buffer);
        frames++;

        if (clock.getElapsedTime().asSeconds() >= 1.0f)
        {
            fps = frames;
            frames = 0;
            clock.restart();
        }

        auto msg = fmt::format("Julia Set: Iteraciones:{} , FPS:{} , Mode:MPI", max_iteraciones, fps);
        text.setString(msg);

        // dibujar
        window.clear();
        {
            window.draw(sprite);
            window.draw(text);
            window.draw(textOptions);
        }
        window.display();
    }

    delete[] texture_buffer;
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    init_freetype();

    delta = std::ceil(HEIGHT * 1.0 / nprocs);
    padding = (delta * nprocs) - HEIGHT;
    row_start = rank * delta;
    row_end = row_start + delta;

    if (row_end > HEIGHT)
    {
        row_end = HEIGHT;
    }

    // Cada proceso (Master y Esclavos) reserva su buffer local de procesamiento
    pixel_buffer = new uint32_t[WIDTH * delta];
    std::memset(pixel_buffer, 0, WIDTH * delta * sizeof(uint32_t));

    fmt::println("rank: {}, rows: {} to: {}", rank, row_start, row_end);

    if (rank == 0)
    {
        setup_ui();
    }
    else
    {
        // Lógica de procesamiento de los procesos n>0
        while (true)
        {
            std::vector<int> dummy = {max_iteraciones, 0};
            // Recibir órdenes del Rank 0
            MPI_Bcast(dummy.data(), 2, MPI_INT, 0, MPI_COMM_WORLD);

            max_iteraciones = dummy[0];
            running = dummy[1];

            if (running == 0)
            {
                break;
            }

            // Calcular la porción correspondiente del fractal
            julia_mpi(x_min, y_min, x_max, y_max, WIDTH, HEIGHT, row_start, row_end, pixel_buffer);

            // Dibujar el texto con el número de rank
            dibujar_texto(rank);

            MPI_Send(pixel_buffer,
                     WIDTH * delta,
                     MPI_UNSIGNED,
                     0,
                     0,
                     MPI_COMM_WORLD);
        }
    }

    delete[] pixel_buffer;

    MPI_Finalize();
    return 0;
}