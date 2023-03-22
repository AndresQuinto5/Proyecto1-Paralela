#include <SDL2/SDL.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <SDL2/SDL_ttf.h>
#include <cmath>
#include <omp.h>


const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int BALL_RADIUS = 30;
const float GRAVITY = 200.0f;
const float BOUNCE_FACTOR = 0.6f;
const float BALL_COLLISION_FACTOR = 0.8f;

Uint32 start_time_collision = SDL_GetTicks();
struct Ball {
    int x, y, dx, dy, bounces, radius;
    Uint8 r, g, b;
    Uint32 start_time_collision;

};

bool useBallCollision(Ball &ball1, Ball &ball2, bool ignoreCollision) {
    if (ignoreCollision) {
        return false;
    }
    int dx = ball1.x - ball2.x;
    int dy = ball1.y - ball2.y;
    int distanceSquared = dx * dx + dy * dy;
    return distanceSquared <= (ball1.radius + ball2.radius) * (ball1.radius + ball2.radius);
}

void drawStar(SDL_Renderer* renderer, int x, int y, int radius) {
    const int num_points = 5;
    const double angle = 2 * M_PI / num_points;
    SDL_Point points[num_points * 2 + 1];

    for (int i = 0; i < num_points * 2; i += 2) {
        points[i].x = x + radius * cos(i / 2 * angle);
        points[i].y = y + radius * sin(i / 2 * angle);
        points[i + 1].x = x + (radius / 2) * cos((i / 2) * angle + angle / 2);
        points[i + 1].y = y + (radius / 2) * sin((i / 2) * angle + angle / 2);
    }

    points[num_points * 2] = points[0];
    SDL_RenderDrawLines(renderer, points, num_points * 2 + 1);
}

struct Collision {
    int id1, id2;
    Uint32 timestamp;
};

void ballCollisionManager(Ball &ball1, Ball &ball2) {
    int dx = ball1.x - ball2.x;
    int dy = ball1.y - ball2.y;
    float distance = hypot(dx, dy);

    if (distance == 0.0f) {
        return;
    }
    if (std::isnan(distance)) {
        std::cerr << "Error: invalid distance value: dx = " << dx << ", dy = " << dy << std::endl;
        return;
    }

    float overlap = ball1.radius + ball2.radius - distance;
    float nx = dx / distance;
    float ny = dy / distance;

    ball1.x += nx * overlap / 2;
    ball1.y += ny * overlap / 2;
    ball2.x -= nx * overlap / 2;
    ball2.y -= ny * overlap / 2;

    float relativeVelocityX = ball1.dx - ball2.dx;
    float relativeVelocityY = ball1.dy - ball2.dy;
    float impulse = (relativeVelocityX * nx + relativeVelocityY * ny) * BALL_COLLISION_FACTOR;

    ball1.dx -= nx * impulse;
    ball1.dy -= ny * impulse;
    ball2.dx += nx * impulse;
    ball2.dy += ny * impulse;

    // Add the size reduction feature for the two balls colliding
    if (ball1.radius > BALL_RADIUS/2 && ball2.radius > BALL_RADIUS/2) {
        ball1.radius *= BOUNCE_FACTOR;
        ball2.radius *= BOUNCE_FACTOR;
        // Cambiar el color de las bolas cuando colisionan
        ball1.r = rand() % 256;
        ball1.g = rand() % 256;
        ball1.b = rand() % 256;
        ball2.r = rand() % 256;
        ball2.g = rand() % 256;
        ball2.b = rand() % 256;
    }
    ball1.start_time_collision = SDL_GetTicks();
    ball2.start_time_collision = SDL_GetTicks();
}

int main(int argc, char* argv[]) {
    if (TTF_Init() == -1) {
        std::cerr << "Error initializing TTF: " << TTF_GetError() << std::endl;
        return 1;
    }

    TTF_Font* font = TTF_OpenFont("arial.ttf", 24);
    if (!font) {
        std::cerr << "Error loading font: " << TTF_GetError() << std::endl;
        return 1;
    }

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Screensaver", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    Uint32 start_time = SDL_GetTicks();
    int frame_count = 0;
    int fps = 0;
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

    Ball* balls = new Ball[num_balls];
    srand(time(0)); // Seed the random number generator with 0
    for (int i = 0; i < num_balls; i++) {
        balls[i].x = rand() % (SCREEN_WIDTH - BALL_RADIUS * 2) + BALL_RADIUS;
        balls[i].y = BALL_RADIUS;
        balls[i].dx = rand() % 400 - 200;
        balls[i].dy = rand() % 400 - 200;
        balls[i].bounces = 0;
        balls[i].radius = BALL_RADIUS;
        balls[i].r = rand() % 256;
        balls[i].g = rand() % 256;
        balls[i].b = rand() % 256;
        balls[i].start_time_collision = SDL_GetTicks();

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
    Uint32 elapsed_time_collision = SDL_GetTicks() - start_time_collision;
    bool ignoreCollision = elapsed_time_collision < 5000;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    int num_threads = 4; // Cambia este valor para ajustar la cantidad de hilos utilizados

    #pragma omp parallel num_threads(num_threads)
    {
        int thread_id = omp_get_thread_num();
        int start = thread_id * num_balls / num_threads;
        int end = (thread_id + 1) * num_balls / num_threads;
    
        for (int i = start; i < end; i++) {
            balls[i].x += balls[i].dx / 60;
            balls[i].y += balls[i].dy / 60;

            balls[i].dy += GRAVITY / 60;

            // Handle wall collisions
            if (balls[i].x - balls[i].radius < 0) {
                balls[i].x = balls[i].radius;
                balls[i].dx = -balls[i].dx * BOUNCE_FACTOR;
            } else if (balls[i].x + balls[i].radius > SCREEN_WIDTH) {
                balls[i].x = SCREEN_WIDTH - balls[i].radius;
                balls[i].dx = -balls[i].dx * BOUNCE_FACTOR;
            }
            // Handle ball collisions
            for (int j = i + 1; j < num_balls; j++) {
                Uint32 elapsed_time_collision_i = SDL_GetTicks() - balls[i].start_time_collision;
                Uint32 elapsed_time_collision_j = SDL_GetTicks() - balls[j].start_time_collision;
                bool ignoreCollision_i = elapsed_time_collision_i < 5000;
                bool ignoreCollision_j = elapsed_time_collision_j < 5000;
                bool ignoreCollision = ignoreCollision_i || ignoreCollision_j;
                
                if (useBallCollision(balls[i], balls[j], ignoreCollision)) {
                    ballCollisionManager(balls[i], balls[j]);
                }
            }
            if (balls[i].y - balls[i].radius < 0) {
                balls[i].y = balls[i].radius;
                balls[i].dy = -balls[i].dy * BOUNCE_FACTOR;

            } else if (balls[i].y + balls[i].radius > SCREEN_HEIGHT) {
                balls[i].y = SCREEN_HEIGHT - balls[i].radius;
                balls[i].dy = -balls[i].dy * BOUNCE_FACTOR;
                balls[i].bounces++;
                balls[i].radius -= 5;
                balls[i].r -= 25;
                balls[i].g -= 10;
                balls[i].b -= 5;
                balls[i].dx *= 1.2;
                balls[i].dy *= 1.2;
            }
            // Check if the ball has collided with the edges of the screen
            if (balls[i].x < balls[i].radius || balls[i].x > SCREEN_WIDTH - balls[i].radius) {
                balls[i].dx = -balls[i].dx * BOUNCE_FACTOR;
                if (fps != 0){
                    balls[i].x += balls[i].dx / fps;
                }
                balls[i].bounces++;
            }
            if (balls[i].y < balls[i].radius) {
                balls[i].dy = -balls[i].dy * BOUNCE_FACTOR;
                if (fps != 0){
                    balls[i].y += balls[i].dy / fps;
                }
                balls[i].bounces++;
            }
            if (balls[i].y > SCREEN_HEIGHT - balls[i].radius) {
                balls[i].dy = -balls[i].dy * BOUNCE_FACTOR;
                if (fps != 0){
                    balls[i].y += balls[i].dy / fps;
                }
                balls[i].bounces++;
            }
            float friction = 0.98f;
            if (balls[i].y + balls[i].radius >= SCREEN_HEIGHT) {
                balls[i].dx *= friction;
            }

            if (balls[i].bounces >= 3) {
                // Reset the position and velocity of the ball
                balls[i].x = rand() % (SCREEN_WIDTH / 2) + SCREEN_WIDTH / 4;
                balls[i].y = rand() % (SCREEN_HEIGHT / 2) + SCREEN_HEIGHT / 4;
                balls[i].dx = rand() % 400 - 200;
                balls[i].dy = rand() % 400 - 200;
                balls[i].bounces = 0;
            }
            if (balls[i].bounces > 10 || balls[i].radius <= 0) {
                // Guardar la bola eliminada para reutilizarla en el futuro
                Ball removed_ball = balls[i];
                for (int j = i; j < num_balls - 1; j++) {
                    balls[j] = balls[j + 1];
                }
                num_balls--;
                
                // Reutilizar la bola eliminada
                removed_ball.x = rand() % (SCREEN_WIDTH - BALL_RADIUS * 2) + BALL_RADIUS;
                removed_ball.y = BALL_RADIUS;
                removed_ball.dx = rand() % 400 - 200;
                removed_ball.dy = rand() % 400 - 200;
                removed_ball.bounces = 0;
                removed_ball.radius = BALL_RADIUS;
                removed_ball.r = rand() % 256;
                removed_ball.g = rand() % 256;
                removed_ball.b = rand() % 256;
                balls[num_balls] = removed_ball;
                num_balls++;
            }
            
            SDL_SetRenderDrawColor(renderer, balls[i].r, balls[i].g, balls[i].b, 255);
            drawStar(renderer, balls[i].x, balls[i].y, balls[i].radius);

            for (int j = i + 1; j < num_balls; j++) {
                if (useBallCollision(balls[i], balls[j], ignoreCollision)) {
                    ballCollisionManager(balls[i], balls[j]);
                }
            }
        }
    }
    // Draw the FPS counter
    frame_count++;
    Uint32 elapsed_time = SDL_GetTicks() - start_time;
    if (elapsed_time >= 1000) {
        fps = frame_count;
        frame_count = 0;
        start_time = SDL_GetTicks();
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

    delete[] balls; // Libera la memoria dinámica asignada al array de bolas
    // Clean up
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;

}