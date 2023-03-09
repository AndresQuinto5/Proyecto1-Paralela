#include <SDL2/SDL.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <SDL2/SDL_ttf.h>
#include <cmath>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int BALL_RADIUS = 10;
const float GRAVITY = 500.0f; // pixels per second squared
const float BOUNCE_FACTOR = 0.8f;

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
    SDL_Window* window = SDL_CreateWindow("Screensaver", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // Initialize variables for FPS counter
    Uint32 start_time = SDL_GetTicks();
    int frame_count = 0;
    int fps = 0;

    // Ask user for the number of balls they want on the screen
    int num_balls;
    std::string input;
    bool valid_input = false;
    while (!valid_input) {
        std::cout << "Enter the number of balls you want on the screen (must be greater than 0): ";
        std::cin >> num_balls;
        if (std::cin.fail() || num_balls <= 0) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a positive integer greater than 0." << std::endl;
        } else {
            valid_input = true;
        }
    }

    // Set up an array to hold the positions and velocities of each ball
    struct Ball {
        int x, y, dx, dy;
    };
    Ball balls[num_balls];

    // Initialize the positions and velocities of each ball
    srand(time(0)); // Seed the random number generator with 0
    for (int i = 0; i < num_balls; i++) {
    balls[i].x = rand() % (SCREEN_WIDTH - BALL_RADIUS * 2) + BALL_RADIUS;
    balls[i].y = BALL_RADIUS;
    balls[i].dx = rand() % 400 - 200;
    balls[i].dy = rand() % 400 - 200;
    }

        // Main loop
    bool quit = false;
    while (!quit) {
        // Handle events
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }
        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw the balls
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (int i = 0; i < num_balls; i++) {
            // Update position
            balls[i].x += balls[i].dx / 60;
            balls[i].y += balls[i].dy / 60;

            // Apply gravity
            balls[i].dy += GRAVITY / 60;

            // Handle wall collisions
            if (balls[i].x - BALL_RADIUS < 0) {
                balls[i].x = BALL_RADIUS;
                balls[i].dx = -balls[i].dx * BOUNCE_FACTOR;
            } else if (balls[i].x + BALL_RADIUS > SCREEN_WIDTH) {
                balls[i].x = SCREEN_WIDTH - BALL_RADIUS;
                balls[i].dx = -balls[i].dx * BOUNCE_FACTOR;
            }
            if (balls[i].y - BALL_RADIUS < 0) {
                balls[i].y = BALL_RADIUS;
                balls[i].dy = -balls[i].dy * BOUNCE_FACTOR;
            } else if (balls[i].y + BALL_RADIUS > SCREEN_HEIGHT) {
                balls[i].y = SCREEN_HEIGHT - BALL_RADIUS;
                balls[i].dy = -balls[i].dy * BOUNCE_FACTOR;
            }

            // Draw the ball
            SDL_Rect rect = {balls[i].x - BALL_RADIUS, balls[i].y - BALL_RADIUS, BALL_RADIUS * 2, BALL_RADIUS * 2};
            SDL_RenderFillRect(renderer, &rect);
        }

        // Draw the FPS counter
        frame_count++;
        Uint32 current_time = SDL_GetTicks();
        if (current_time > start_time + 1000) {
            fps = frame_count;
            frame_count = 0;
            start_time = current_time;
        }
        SDL_Color color = {255, 255, 255};
        std::string fps_text = "FPS: " + std::to_string(fps);
        SDL_Surface* surface = TTF_RenderText_Solid(font, fps_text.c_str(), color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect text_rect = {0, 0, surface->w, surface->h};
        SDL_RenderCopy(renderer, texture, NULL, &text_rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);

        // Update the screen
        SDL_RenderPresent(renderer);
    }

    // Clean up
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;

}