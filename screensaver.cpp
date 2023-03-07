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

        // Clear screen with green color
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        // Draw ball
        static int x = 50;
        static int y = 50;
        static int dx = 5;
        static int dy = 5;
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
        SDL_Rect ball = {x, y, 20, 20};
        SDL_RenderFillRect(renderer, &ball);

        // Update ball position
        x += dx;
        y += dy;
        if (x <= 0 || x + ball.w >= 640) {
            dx = -dx;
        }
        if (y <= 0 || y + ball.h >= 480) {
            dy = -dy;
        }

        // Draw FPS to top-left corner of screen
        SDL_Color textColor = {255, 255, 255};
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, ("FPS: " + std::to_string(fps)).c_str(), textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect textRect = {0, 0, textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);

        // Draw the ball
        static int starting_x = 100, starting_y = 100; // starting position of the ball
        static int velocity_x = 5, velocity_y = 5; // velocity of the ball
        static int radius = 20; // radius of the ball

        // update the position of the ball
        starting_x += velocity_x;
        starting_y += velocity_y;

        // check if the ball hits the walls and change direction if so
        if (x - radius < 0 || x + radius > 640) {
            dx = -dx;
        }
        if (y - radius < 0 || y + radius > 480) {
            dy = -dy;
        }

        // draw the ball
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); // black color for the ball
        SDL_Rect rect = {x - radius, y - radius, radius * 2, radius * 2};
        SDL_RenderFillRect(renderer, &rect);


        // Add a 5-second delay
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

// Clean up font and TTF
TTF_CloseFont(font);
TTF_Quit();

return 0;

}