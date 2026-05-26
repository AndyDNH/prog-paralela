#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>
#include <cstring>
#include <string>
#include <SFML/Graphics.hpp>

#include "grayscale_simd.h"
#include "grayscale_openmp.h"

void print_menu()
{
    std::cout << "\n--- Control de la Ventana ---\n";
    std::cout << "Presione [1]: Mostrar imagen original\n";
    std::cout << "Presione [2]: Aplicar filtro SIMD y mostrar\n";
    std::cout << "Presione [3]: Aplicar filtro OpenMP y mostrar\n";
    std::cout << "Presione [S]: Guardar imagen resultante\n";
    std::cout << "Presione [Esc] o cierre la ventana para salir\n";
}

int main()
{

    int width, height, channels;
    const char *ruta_imagen = "imagen_entrada.png";
    unsigned char *img = stbi_load(ruta_imagen, &width, &height, &channels, STBI_rgb_alpha);

    if (!img)
    {
        std::cerr << "Error al cargar la imagen desde: " << ruta_imagen << "\n";
        return 1;
    }
    channels = 4;

    int img_size = width * height * channels;
    unsigned char *img_simd = new unsigned char[img_size];
    unsigned char *img_openmp = new unsigned char[img_size];

    unsigned char *img_result = img;
    std::string last_method = "original";

    sf::Vector2u window_size(static_cast<unsigned int>(width), static_cast<unsigned int>(height));
    sf::RenderWindow window(sf::VideoMode(window_size), "Filtro de Imagenes - Escala de Grises");

    sf::Texture texture;
    if (!texture.resize(window_size))
    {
        std::cerr << "Error al crear la textura de SFML\n";
        stbi_image_free(img);
        delete[] img_simd;
        delete[] img_openmp;
        return 1;
    }

    texture.update(img);
    sf::Sprite sprite(texture);

    print_menu();

    while (window.isOpen())
    {

        while (std::optional<sf::Event> event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->code == sf::Keyboard::Key::Num1)
                {
                    std::cout << "[1] Mostrando imagen original.\n";
                    img_result = img;
                    last_method = "original";
                    texture.update(img_result);
                }
                else if (keyPressed->code == sf::Keyboard::Key::Num2)
                {
                    std::cout << "[2] Aplicando filtro SIMD...\n";
                    grayscale_simd(img, img_simd, width, height, channels);
                    img_result = img_simd;
                    last_method = "simd";
                    texture.update(img_result);
                }
                else if (keyPressed->code == sf::Keyboard::Key::Num3)
                {
                    std::cout << "[3] Aplicando filtro OpenMP...\n";
                    grayscale_openmp(img, img_openmp, width, height, channels);
                    img_result = img_openmp;
                    last_method = "openmp";
                    texture.update(img_result);
                }
                else if (keyPressed->code == sf::Keyboard::Key::S)
                {
                    std::string nombre = (last_method == "simd") ? "salida_simd.png" : (last_method == "openmp") ? "salida_openmp.png"
                                                                                                                 : "salida_original.png";

                    stbi_write_png(nombre.c_str(), width, height, channels, img_result, width * channels);
                    std::cout << "[S] Imagen guardada : " << nombre << "\n";
                }
                else if (keyPressed->code == sf::Keyboard::Key::Escape)
                {
                    window.close();
                }
            }
        }

        window.clear();
        window.draw(sprite);
        window.display();
    }

    stbi_image_free(img);
    delete[] img_simd;
    delete[] img_openmp;

    std::cout << "Programa finalizado con exito.\n";
    return 0;
}