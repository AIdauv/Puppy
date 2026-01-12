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

    struct InputState {
        // 键盘状态
        bool keyW = false;
        bool keyA = false;
        bool keyS = false;
        bool keyD = false;
        bool keyQ = false;
        bool keyE = false;

        // 鼠标状态
        bool mouseLeft = false;
        bool mouseRight = false;
        int mouseX = 0;
        int mouseY = 0;
        int mouseDeltaX = 0;
        int mouseDeltaY = 0;

        // 重置每帧的增量
        void resetDelta() {
            mouseDeltaX = 0;
            mouseDeltaY = 0;
        }
    } inputState;

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

    const InputState& getInputState() const { return inputState; }
    void captureMouse(bool capture);
};
