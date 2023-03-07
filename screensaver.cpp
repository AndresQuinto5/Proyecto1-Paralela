#include <SDL2/SDL.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <SDL2/SDL_ttf.h>

int main(int argc, char* argv[]) {
    // Initialize TTF
    if (TTF_Init() == -1) {
        std::cerr << "Error initializing TTF: " << TTF_GetError() << std::endl;
        return 1;
    }

    // Create font object
    TTF_Font* font = TTF_OpenFont("arial.ttf", 24);
    if (!font) {
        std::cerr << "Error loading font: " << TTF_GetError() << std::endl;
        return 1;
    }

    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);

    // Create a window and renderer
    SDL_Window* window = SDL_CreateWindow("Screensaver", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // Initialize variables for FPS counter
    Uint32 start_time = SDL_GetTicks();
    int frame_count = 0;
    int fps = 0;

    // Ask user for the number of balls they want on the screen
    int num_balls;
    std::cout << "Enter the number of balls you want on the screen: ";
    std::cin >> num_balls;

    // Set up an array to hold the positions and velocities of each ball
    struct Ball {
        int x, y, dx, dy;
    };
    Ball balls[num_balls];

    // Initialize the positions and velocities of each ball
    srand(time(NULL));
    for (int i = 0; i < num_balls; i++) {
        balls[i].x = rand() % (640 - 20) + 20;
        balls[i].y = rand() % (480 - 20) + 20;
        balls[i].dx = rand() % 10 - 5;
        balls[i].dy = rand() % 10 - 5;
    }

    // Run the event loop
    bool quit = false;
    while (!quit) {
        // Calculate FPS
        Uint32 current_time = SDL_GetTicks();
        if (current_time - start_time >= 1000) {
            fps = frame_count;
            frame_count = 0;
            start_time = current_time;
            std::cout << "FPS: " << fps << std::endl;
        }

        // Handle events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Calculate time elapsed in milliseconds
        float time_elapsed = current_time / 1000.0f;

        // Calculate rainbow color values using time elapsed
        Uint8 r = (1 + sin(time_elapsed * 0.5f)) * 128;
        Uint8 g = (1 + sin(time_elapsed * 0.7f)) * 128;
        Uint8 b = (1 + sin(time_elapsed * 0.9f)) * 128;

        // Set background color to rainbow color
        SDL_SetRenderDrawColor(renderer, r, g, b, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        // Update ball positions and draw them
        SDL_SetRenderDrawColor(renderer, 255, 255, 255,SDL_ALPHA_OPAQUE);
        for (int i = 0; i < num_balls; i++) {
        // Update ball position
        balls[i].x += balls[i].dx;
        balls[i].y += balls[i].dy;
                // Handle ball-wall collisions
        if (balls[i].x < 0 || balls[i].x > 640 - 20) {
            balls[i].dx *= -1;
        }
        if (balls[i].y < 0 || balls[i].y > 480 - 20) {
            balls[i].dy *= -1;
        }

        // Draw ball
        SDL_Rect ball_rect = { balls[i].x, balls[i].y, 20, 20 };
        SDL_RenderFillRect(renderer, &ball_rect);
    }

        // Draw FPS counter
        SDL_Color text_color = { 255, 255, 255, SDL_ALPHA_OPAQUE };
        std::string fps_text = "FPS: " + std::to_string(fps);
        SDL_Surface* text_surface = TTF_RenderText_Solid(font, fps_text.c_str(), text_color);
        SDL_Texture* text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
        SDL_Rect text_rect = { 10, 10, text_surface->w, text_surface->h };
        SDL_RenderCopy(renderer, text_texture, NULL, &text_rect);
        SDL_FreeSurface(text_surface);
        SDL_DestroyTexture(text_texture);

        // Update the screen
        SDL_RenderPresent(renderer);

        // Increase frame count
        frame_count++;

        // Delay to achieve desired FPS
        Uint32 end_time = SDL_GetTicks();
        Uint32 frame_time = end_time - current_time;
        if (frame_time < 1000 / 60) {
            SDL_Delay(1000 / 60 - frame_time);
        }
    }

    // Clean up resources and quit
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
