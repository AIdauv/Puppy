#include "Window.h"
#include "Framebuffer.h"
#include "MultisampleFramebuffer.h"
#include "Rasterizer.h"
#include "Camera.h"
#include "Model.h"
#include "Light.h"
#include "Texture.h"
#include "Shader.h"
#include "SimpleShader.h"
#include "PhongShader.h"
#include "NormalMappingShader.h"
#include <iostream>
#include <chrono>
#include <glm/gtc/matrix_transform.hpp> 

int main(int argc, char* argv[]) {
    // 1. 创建窗口
    Window window(800, 600);
    if (!window.init()) {
        std::cerr << "窗口初始化失败" << std::endl;
        return -1;
    }

    // 2. 创建帧缓冲（画布）
    Framebuffer framebuffer(window.getWidth(), window.getHeight());
    MultisampleFramebuffer framebufferMSAA(window.getWidth(), window.getHeight(), 4);  // MSAA帧缓冲

    // 3. 创建光栅化器
    Rasterizer rasterizer;

    // 4. 创建相机
    Camera camera;
    camera.setPosition({ 0.0f, 0.0f, 5.0f });
    camera.setTarget({ 0.0f, 0.0f, 0.0f });
    camera.setPerspective(0.1f, 
        100.0f, 
        45.0f, 
        static_cast<float>(window.getWidth()) / window.getHeight());

    // 5. 创建光源
    Light light;
    light.position = glm::vec3(1, 1, 1);
    light.color = glm::vec3(1.0f, 0.9f, 0.8f);  // 暖白色
    light.intensity = 1.2f;

    // 6. 创建纹理
    Texture texture;
    //texture.loadFromFile("texture/1.jpg");
    texture.loadFromFile("texture/Bricks102_1K-JPG_Color.jpg");
    texture.setSampleMode(SampleMode::Bilinear);
    texture.generateMipmaps();
    //texture.createSolidColor(256, 256, glm::vec3(1.0f, 0.5f, 0.0f));
    //texture.createCheckerboard(256, 256, 32, 
    //    glm::vec3(0.8f, 0.2f, 0.2f), 
    //    glm::vec3(0.2f, 0.2f, 0.8f));

    // 7. 创建法线贴图与凹凸贴图
    Texture normalMap;
    normalMap.loadFromFile("texture/Bricks102_1K-JPG_NormalGL.jpg");
    normalMap.setSampleMode(SampleMode::Bilinear);
    normalMap.generateMipmaps();

    Texture heightMap;
    heightMap.createCheckerboard(256, 256, 32,
        glm::vec3(0.3f, 0.3f, 0.3f),  // 模拟低洼处
        glm::vec3(0.7f, 0.7f, 0.7f)); // 模拟高处
    heightMap.setSampleMode(SampleMode::Bilinear);


    // 8. 创建一些3D三角形进行测试
     
    // 逆时针三角形（正面朝镜头）
    Triangle3D tri1;
    tri1.v0 = Vertex3D(glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    tri1.v1 = Vertex3D(glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    tri1.v2 = Vertex3D(glm::vec3(0.0f, 1.0f, -2.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    // 顺时针三角形（背面朝镜头）
    Triangle3D tri2;
    tri2.v0 = Vertex3D(glm::vec3(-0.5f, -0.5f, -0.75f), glm::vec3(1.0f, 1.0f, 0.0f));
    tri2.v1 = Vertex3D(glm::vec3(0.0f, 0.5f, -1.5f), glm::vec3(1.0f, 0.0f, 1.0f));  // 注意：交换了顶点顺序
    tri2.v2 = Vertex3D(glm::vec3(0.5f, -0.5f, -0.75f), glm::vec3(0.0f, 1.0f, 1.0f));
    
    // 9. 模型测试
    Model model;
    model.createCube(2);
    model.setPosition(glm::vec3(-1, 0, -3));
    float rotationAngle = 0.0f;
    model.setRotation(glm::vec3(0.0f, 60.0f, 0.0f));
    model.setScale(glm::vec3(1.0, 1.0, 1.0));
    glm::mat4 modelMat = model.getModelMatrix();


    // 10. 创建着色器
    std::unique_ptr<NormalMappingShader> shader = std::make_unique<NormalMappingShader>();

    // 设置着色器参数
    shader->setNormalMode(NormalMode::NORMAL_MAP);  // 初始模式：法线贴图
    shader->setNormalMapStrength(1.0f);
    shader->setBumpMapStrength(1.0f);
    shader->setBumpScale(1.0f);
    shader->setBumpStrength(15.0f);
    shader->setHybridWeight(0.5f);


    // 着色器上下文
    ShaderContext context;
    context.modelMatrix = model.getModelMatrix();
    context.viewMatrix = camera.getViewMatrix();
    context.projectionMatrix = camera.getProjectionMatrix();
    context.light = light;
    context.cameraPos = camera.getPosition();
    context.texture = &texture;
    context.normalMap = &normalMap;
    context.heightMap = &heightMap;
    context.usePerspective = true;
    context.useLight = true;
    context.useTexture = true;
    context.useNormalMap = true;
    context.useMipmap = true;
    context.useMipmapForNormalMap = true;
    context.lodBias = 0.0f;


    std::cout << "软光栅化器 v0.3 - 着色与纹理版本" << std::endl;
    std::cout << "控制说明：" << std::endl;
    std::cout << "  W/S - 前后移动" << std::endl;
    std::cout << "  A/D - 左右移动" << std::endl;
    std::cout << "  Q/E - 上下移动" << std::endl;
    std::cout << "  鼠标左键 + 拖动 - 旋转视角" << std::endl;
    std::cout << "  ESC - 退出程序" << std::endl;

    // 用于帧时间计算
    auto lastTime = std::chrono::high_resolution_clock::now();

    // 主循环
    while (window.isRunning()) {
        // 计算帧时间
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        // 处理事件
        window.handleEvents();

        // 获取输入状态
        const auto& input = window.getInputState();

        // 相机控制
        float moveSpeed = 3.0f * deltaTime;
        float rotateSpeed = 50.0f * deltaTime;

        // 移动相机
        if (input.keyW) camera.moveForward(moveSpeed);
        if (input.keyS) camera.moveForward(-moveSpeed);
        if (input.keyA) camera.moveRight(-moveSpeed);
        if (input.keyD) camera.moveRight(moveSpeed);
        if (input.keyQ) camera.moveUp(moveSpeed);
        if (input.keyE) camera.moveUp(-moveSpeed);

        // 旋转相机（使用鼠标）
        if (input.mouseLeft) {
            float mouseSensitivity = 0.1f;
            float yaw = -input.mouseDeltaX * mouseSensitivity;
            float pitch = -input.mouseDeltaY * mouseSensitivity;
            camera.rotate(yaw, pitch);
        }

        // 更新旋转
        rotationAngle += 60.0f * deltaTime;  // 60度/秒
        model.setRotation(glm::vec3(0, rotationAngle, 0));

        // 更新着色器上下文
        context.modelMatrix = model.getModelMatrix();
        context.viewMatrix = camera.getViewMatrix();
        context.cameraPos = camera.getPosition();

        // 清屏（黑色）
        framebuffer.clear(0xFF000000);  // ARGB: 黑色
        framebuffer.clearDepth();

        framebufferMSAA.clear(0xFF000000);  // ARGB: 黑色
        framebufferMSAA.clearDepth();

        
        

        // 绘制所有三角形
        
        for (const Triangle3D& tri : model.getTriangles()) {
            rasterizer.drawTriangle3D(tri, *shader, context, framebufferMSAA,
                MathUtils::CullingMode::BACK);
        }

        //rasterizer.drawTriangle3D(tri1, glm::mat4(1.0f), camera.getViewMatrix(), camera.getProjectionMatrix(), framebuffer,
        //    MathUtils::CullingMode::NONE);
        //rasterizer.drawTriangle3D(tri2, glm::mat4(1.0f), camera.getViewMatrix(), camera.getProjectionMatrix(), framebuffer,
        //    MathUtils::CullingMode::NONE);


        // 绘制一些调试信息
        framebuffer.setPixel(10, 10, glm::vec3(1, 1, 1));  // 左上角白点
        framebuffer.setPixel(window.getWidth() - 10, 10, glm::vec3(1, 1, 1));  // 右上角白点

        // 更新到窗口
        //window.updateTexture(framebuffer.getData(), framebufferMSAA.getPitch());
        
        framebufferMSAA.resolve();
        window.updateTexture(framebufferMSAA.getResolvedData(), framebufferMSAA.getPitch());
        window.render();

    }

    std::cout << "程序正常退出" << std::endl;
    return 0;
}