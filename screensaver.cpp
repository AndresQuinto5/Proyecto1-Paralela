#include <SDL2/SDL.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <SDL2/SDL_ttf.h>
#include <cmath>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int BALL_RADIUS = 10;

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
    srand(time(NULL));
    for (int i = 0; i < num_balls; i++) {
        balls[i].x = rand() % (SCREEN_WIDTH - BALL_RADIUS*2) + BALL_RADIUS;
        balls[i].y = rand() % (SCREEN_HEIGHT - BALL_RADIUS*2) + BALL_RADIUS;
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
        Uint8 b = (1 + sin(time_elapsed * 1.1f)) * 128;

        // Set the color of the renderer to the rainbow color
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        // Clear the renderer
        SDL_RenderClear(renderer);

        // Update the positions of the balls
        for (int i = 0; i < num_balls; i++) {
            balls[i].x += balls[i].dx;
            balls[i].y += balls[i].dy;

            // Check for collisions with the walls
            if (balls[i].x - BALL_RADIUS < 0) {
                balls[i].x = BALL_RADIUS;
                balls[i].dx *= -1;
            }
            if (balls[i].x + BALL_RADIUS > SCREEN_WIDTH) {
                balls[i].x = SCREEN_WIDTH - BALL_RADIUS;
                balls[i].dx *= -1;
            }
            if (balls[i].y - BALL_RADIUS < 0) {
                balls[i].y = BALL_RADIUS;
                balls[i].dy *= -1;
            }
            if (balls[i].y + BALL_RADIUS > SCREEN_HEIGHT) {
                balls[i].y = SCREEN_HEIGHT - BALL_RADIUS;
                balls[i].dy *= -1;
            }

            // Check for collisions with other balls
            for (int j = 0; j < num_balls; j++) {
                if (i != j) {
                    int dx = balls[i].x - balls[j].x;
                    int dy = balls[i].y - balls[j].y;
                    int distance_squared = dx*dx + dy*dy;
                    int radius_sum = BALL_RADIUS*2;
                    if (distance_squared < radius_sum*radius_sum) {
                        // Collision detected
                        balls[i].dx *= -1;
                        balls[i].dy *= -1;
                        balls[j].dx *= -1;
                        balls[j].dy *= -1;
                    }
                }
            }

            // Draw the ball
            SDL_SetRenderDrawColor(renderer, r, g, b, 255);
            SDL_Rect ball_rect = { balls[i].x - BALL_RADIUS, balls[i].y - BALL_RADIUS, BALL_RADIUS*2, BALL_RADIUS*2 };
            SDL_RenderFillRect(renderer, &ball_rect);
        }

        // Draw the FPS counter
        std::string fps_str = "FPS: " + std::to_string(fps);
        SDL_Color color = { r, g, b, 255 };
        SDL_Surface* surface = TTF_RenderText_Solid(font, fps_str.c_str(), color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect text_rect = { 10, 10, surface->w, surface->h };
        SDL_RenderCopy(renderer, texture, NULL, &text_rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);

        // Present the renderer
        SDL_RenderPresent(renderer);

        // Increment the frame count
        frame_count++;
    }

// Clean up
SDL_DestroyRenderer(renderer);
SDL_DestroyWindow(window);
TTF_CloseFont(font);
TTF_Quit();
SDL_Quit();

return 0;
}
