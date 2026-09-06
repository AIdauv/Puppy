#include "Window.h"
#include <iostream>

Window::Window(int w, int h) : width(w), height(h) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL初始化失败: " << SDL_GetError() << std::endl;
    }
}

Window::~Window() {
    if (texture) SDL_DestroyTexture(texture);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

bool Window::init() {
    window = SDL_CreateWindow("Puppy",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_SHOWN);
    if (!window) return false;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) return false;

    texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        width, height);
    if (!texture) return false;

    SDL_GetMouseState(&inputState.mouseX, &inputState.mouseY);

    return true;
}

void Window::handleEvents() {

    inputState.resetDelta();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            running = false;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_ESCAPE:
                running = false;
                break;
            case SDLK_w:
                inputState.keyW = true;
                break;
            case SDLK_a:
                inputState.keyA = true;
                break;
            case SDLK_s:
                inputState.keyS = true;
                break;
            case SDLK_d:
                inputState.keyD = true;
                break;
            case SDLK_q:
                inputState.keyQ = true;
                break;
            case SDLK_e:
                inputState.keyE = true;
                break;
            }
            break;

        case SDL_KEYUP:
            switch (event.key.keysym.sym) {
            case SDLK_w:
                inputState.keyW = false;
                break;
            case SDLK_a:
                inputState.keyA = false;
                break;
            case SDLK_s:
                inputState.keyS = false;
                break;
            case SDLK_d:
                inputState.keyD = false;
                break;
            case SDLK_q:
                inputState.keyQ = false;
                break;
            case SDLK_e:
                inputState.keyE = false;
                break;
            }
            break;

        case SDL_MOUSEMOTION:
            // 计算鼠标增量
            inputState.mouseDeltaX = event.motion.xrel;
            inputState.mouseDeltaY = event.motion.yrel;
            inputState.mouseX = event.motion.x;
            inputState.mouseY = event.motion.y;
            break;

        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT) {
                inputState.mouseLeft = true;
                // 按下鼠标左键时捕获鼠标
                SDL_SetRelativeMouseMode(SDL_TRUE);
            }
            else if (event.button.button == SDL_BUTTON_RIGHT) {
                inputState.mouseRight = true;
            }
            break;

        case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_LEFT) {
                inputState.mouseLeft = false;
                SDL_SetRelativeMouseMode(SDL_FALSE);
            }
            else if (event.button.button == SDL_BUTTON_RIGHT) {
                inputState.mouseRight = false;
            }
            break;
        }
    }
}

void Window::updateTexture(void* pixelData, int pitch) {
    SDL_UpdateTexture(texture, nullptr, pixelData, pitch);
}

void Window::render() {
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

void Window::captureMouse(bool capture) {
    SDL_SetRelativeMouseMode(capture ? SDL_TRUE : SDL_FALSE);
}