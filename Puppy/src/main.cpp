#include "Window.h"
#include "Framebuffer.h"
#include "Rasterizer.h"
#include "Camera.h"
#include <iostream>

int main(int argc, char* argv[]) {
    // 1. 创建窗口
    Window window(800, 600);
    if (!window.init()) {
        std::cerr << "窗口初始化失败" << std::endl;
        return -1;
    }

    // 2. 创建帧缓冲（画布）
    Framebuffer framebuffer(window.getWidth(), window.getHeight());

    // 3. 创建光栅化器
    Rasterizer rasterizer(framebuffer);

    std::cout << "软光栅化器 v0.1 启动" << std::endl;
    std::cout << "按ESC退出程序" << std::endl;

    // 主循环
    while (window.isRunning()) {
        // 处理事件
        window.handleEvents();

        // 清屏（黑色）
        framebuffer.clear(0xFF000000);  // ARGB: 黑色

        // 绘制所有三角形
            rasterizer.drawTriangle2D(glm::vec2(200, 200),
                glm::vec2(150, 100),
                glm::vec2(250, 100),
                glm::vec3(1.0f, 0.0f, 0.0f));

            rasterizer.drawTriangle2D(glm::vec2(600, 200),
                glm::vec2(550, 100),
                glm::vec2(650, 100),
                glm::vec3(0.0f, 1.0f, 0.0f));

            rasterizer.drawTriangle2D(glm::vec2(300, 200),
                glm::vec2(500, 200),
                glm::vec2(400, 300),
                glm::vec3(0.0f, 0.0f, 1.0f));

        // 绘制一些调试信息
        framebuffer.setPixel(10, 10, glm::vec3(1, 1, 1));  // 左上角白点
        framebuffer.setPixel(window.getWidth() - 10, 10, glm::vec3(1, 1, 1));  // 右上角白点

        // 更新到窗口
        window.updateTexture(framebuffer.getData(), framebuffer.getPitch());
        window.render();

        // 简单帧率控制（约60FPS）
        SDL_Delay(16);
    }

    std::cout << "程序正常退出" << std::endl;
    return 0;
}