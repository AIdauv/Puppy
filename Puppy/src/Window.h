#pragma once

#include <SDL.h>

class Window {
private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* texture = nullptr;
    int width = 800;
    int height = 600;
    bool running = true;

public:
    Window(int w = 800, int h = 600);
    ~Window();

    bool init();
    void handleEvents();
    bool isRunning() const { return running; }
    void updateTexture(void* pixelData, int pitch);
    void render();
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    void close() { running = false; }
};
