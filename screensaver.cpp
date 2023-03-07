#include <SDL2/SDL.h>
#include <cstdlib>
#include <ctime>
#include <iostream>

int main(int argc, char* argv[]) {
    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);

    // Create a window and renderer
    SDL_Window* window = SDL_CreateWindow("Ball Bouncing", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // Initialize variables for FPS counter
    Uint32 start_time = SDL_GetTicks();
    int frame_count = 0;
    int fps = 0;

    // Initialize variables for ball movement
    int x = 320, y = 240;
    int x_speed = 5, y_speed = 5;
    int ball_size = 20;

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

        // Move the ball
        x += x_speed;
        y += y_speed;

        // Bounce the ball off the edges of the screen
        if (x < 0 || x > 640 - ball_size) {
            x_speed = -x_speed;
        }
        if (y < 0 || y > 480 - ball_size) {
            y_speed = -y_speed;
        }

        // Draw the ball
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        SDL_Rect ball_rect = {x, y, ball_size, ball_size};
        SDL_RenderFillRect(renderer, &ball_rect);

        // Add a 5ms delay to simulate 200 FPS
        SDL_Delay(5);

        // Update the screen
        SDL_RenderPresent(renderer);

        // Increment frame count
        frame_count++;
    }

    // Clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
