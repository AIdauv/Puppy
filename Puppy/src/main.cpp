#include "Window.h"
#include "MultisampleFramebuffer.h"
#include "Rasterizer.h"
#include "Camera.h"
#include "Model.h"
#include "Light.h"
#include "NormalMappingShader.h"
#include "Texture.h"
#include <iostream>

int main(int argc, char* argv[]) {
    // ========== 控制台输出操作指南 ==========
    std::cout << "========================================" << std::endl;
    std::cout << "         软光栅渲染器 - 相机控制        " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "  W / S  : 前后移动（沿视线方向）" << std::endl;
    std::cout << "  A / D  : 左右移动（水平方向）" << std::endl;
    std::cout << "  Q / E  : 上下移动（垂直升降）" << std::endl;
    std::cout << "  鼠标左键拖拽 : 旋转视角" << std::endl;
    std::cout << "  ESC    : 退出程序" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;

    // 窗口
    Window window(1024, 768);
    if (!window.init()) {
        std::cerr << "窗口初始化失败！" << std::endl;
        return -1;
    }

    int width = window.getWidth();
    int height = window.getHeight();
    const int msaaSamples = 4;   // 可改为 1 对比锯齿

    MultisampleFramebuffer framebuffer(width, height, msaaSamples);

    // 摄像机
    Camera camera;
    camera.setPosition(glm::vec3(3.0f, 2.0f, 4.5f));
    camera.setTarget(glm::vec3(0.0f, 0.0f, 0.0f));
    camera.setPerspective(0.1f, 100.0f, 45.0f, (float)width / height);

    // 光源
    Light light(glm::vec3(2.0f, 4.0f, 3.0f), glm::vec3(1.0f, 0.9f, 0.8f), 1.0f);

    // 加载纹理
    Texture brickColor, brickNormal, dogTexture;
    if (!brickColor.loadFromFile("texture/Bricks102_1K-JPG_Color.jpg")) {
        std::cerr << "砖墙颜色纹理加载失败" << std::endl;
        return -1;
    }
    if (!brickNormal.loadFromFile("texture/Bricks102_1K-JPG_NormalGL.jpg")) {
        std::cerr << "砖墙法线纹理加载失败" << std::endl;
        return -1;
    }
    if (!dogTexture.loadFromFile("texture/2.jpg")) {
        std::cerr << "小狗纹理加载失败，将使用默认颜色" << std::endl;
    }

    brickColor.generateMipmaps();
    brickNormal.generateMipmaps();
    dogTexture.generateMipmaps();

    // 纯色纹理
    Texture goldTexture;
    goldTexture.createSolidColor(1, 1, glm::vec3(1.0f, 0.8f, 0.2f));
    Texture grayTexture;
    grayTexture.createSolidColor(1, 1, glm::vec3(0.6f, 0.6f, 0.6f));

    // ---------- 场景物体 ----------
    // 背景墙
    Model backWall;
    backWall.createPlane(8.0f, 6.0f, glm::vec3(0, 0, 1), glm::vec3(1, 0, 0), 4, 3);
    backWall.setPosition(glm::vec3(0.0f, 0.0f, -3.0f));

    // 左立方体（小狗纹理）
    Model cubeLeft;
    cubeLeft.createCube(1.0f);
    cubeLeft.setPosition(glm::vec3(-2.0f, 0.5f, 0.0f));
    cubeLeft.setRotation(glm::vec3(25.0f, 40.0f, 10.0f));

    // 中央球体（灰色）
    Model sphere;
    sphere.createSphere(0.9f, 32, 32);
    sphere.setPosition(glm::vec3(0.0f, 0.9f, 0.0f));

    // 右立方体（金色）
    Model cubeRight;
    cubeRight.createCube(1.0f);
    cubeRight.setPosition(glm::vec3(2.0f, 0.5f, 0.0f));
    cubeRight.setRotation(glm::vec3(-15.0f, 30.0f, 5.0f));

    // ---------- 着色器 ----------
    NormalMappingShader shader;
    shader.setAmbientStrength(0.2f);
    shader.setDiffuseStrength(0.8f);

    // ---------- 渲染上下文 ----------
    ShaderContext context;
    context.projectionMatrix = camera.getProjectionMatrix();
    context.light = light;
    context.cameraPos = camera.getPosition();
    context.useLight = true;
    context.useTexture = true;
    context.useMipmap = true;
    context.lodBias = 0.0f;

    Rasterizer rasterizer;

    // ---------- 主循环 ----------
    while (window.isRunning()) {
        window.handleEvents();
        const auto& input = window.getInputState();

        // ---- 相机移动 ----
        float speed = 0.05f;
        if (input.keyW) camera.moveForward(speed);
        if (input.keyS) camera.moveForward(-speed);
        if (input.keyA) camera.moveRight(-speed);
        if (input.keyD) camera.moveRight(speed);
        if (input.keyQ) camera.moveUp(speed);
        if (input.keyE) camera.moveUp(-speed);
        if (input.mouseLeft) {
            float sensitivity = 0.2f;
            camera.rotate(input.mouseDeltaX * sensitivity,
                -input.mouseDeltaY * sensitivity);
        }

        context.viewMatrix = camera.getViewMatrix();
        context.projectionMatrix = camera.getProjectionMatrix();
        context.cameraPos = camera.getPosition();

        // ---- 清屏 ----
        framebuffer.clear(0xff1a1a2e);
        framebuffer.clearDepth();

        // ---- 绘制所有物体 ----
        // 1. 背景墙（砖墙 + 法线贴图）
        context.modelMatrix = backWall.getModelMatrix();
        context.texture = &brickColor;
        context.normalMap = &brickNormal;
        context.useNormalMap = true;
        shader.setNormalMode(NORMAL_MAP);
        shader.setSpecularStrength(0.3f);
        shader.setShininess(16.0f);
        for (const auto& tri : backWall.getTriangles()) {
            rasterizer.drawTriangle3D(tri, shader, context, framebuffer, MathUtils::CullingMode::BACK);
        }

        // 2. 左立方体（小狗纹理）
        context.modelMatrix = cubeLeft.getModelMatrix();
        context.texture = &dogTexture;
        context.normalMap = nullptr;
        context.useNormalMap = false;
        shader.setNormalMode(VERTEX_NORMALS);
        shader.setSpecularStrength(0.5f);
        shader.setShininess(32.0f);
        for (const auto& tri : cubeLeft.getTriangles()) {
            rasterizer.drawTriangle3D(tri, shader, context, framebuffer, MathUtils::CullingMode::BACK);
        }

        // 3. 中央球体（灰色纹理，展示光照）
        context.modelMatrix = sphere.getModelMatrix();
        context.texture = &grayTexture;
        context.normalMap = nullptr;
        context.useNormalMap = false;
        shader.setNormalMode(VERTEX_NORMALS);
        shader.setSpecularStrength(0.8f);
        shader.setShininess(64.0f);
        for (const auto& tri : sphere.getTriangles()) {
            rasterizer.drawTriangle3D(tri, shader, context, framebuffer, MathUtils::CullingMode::BACK);
        }

        // 4. 右立方体（金色纹理，金属质感）
        context.modelMatrix = cubeRight.getModelMatrix();
        context.texture = &goldTexture;
        context.normalMap = nullptr;
        context.useNormalMap = false;
        shader.setNormalMode(VERTEX_NORMALS);
        shader.setSpecularStrength(1.0f);
        shader.setShininess(128.0f);
        for (const auto& tri : cubeRight.getTriangles()) {
            rasterizer.drawTriangle3D(tri, shader, context, framebuffer, MathUtils::CullingMode::BACK);
        }

        // ---- MSAA 解析并显示 ----
        framebuffer.resolve();
        window.updateTexture(framebuffer.getResolvedData(), framebuffer.getPitch());
        window.render();

        SDL_Delay(16);
    }

    return 0;
}