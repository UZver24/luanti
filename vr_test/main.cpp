#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <string>
#include <cstdlib>
#include <vector>
#include <cmath>

#include <openvr.h>

// Простые математические структуры
struct Vector3 {
    float x, y, z;
    Vector3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
};

struct Matrix4 {
    float m[4][4];
    Matrix4() {
        for(int i = 0; i < 4; i++)
            for(int j = 0; j < 4; j++)
                m[i][j] = (i == j) ? 1.0f : 0.0f;
    }
};

// Глобальные переменные
vr::IVRSystem* g_pHMD = nullptr;
vr::IVRCompositor* g_pCompositor = nullptr;
SDL_Window* g_pWindow = nullptr;
SDL_GLContext g_pContext = nullptr;

// Размеры окна
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// Функции для работы с матрицами
Matrix4 ConvertSteamVRMatrixToMatrix4(const vr::HmdMatrix34_t& matPose) {
    Matrix4 matrixObj;
    matrixObj.m[0][0] = matPose.m[0][0];
    matrixObj.m[0][1] = matPose.m[1][0];
    matrixObj.m[0][2] = matPose.m[2][0];
    matrixObj.m[0][3] = 0.0f;
    matrixObj.m[1][0] = matPose.m[0][1];
    matrixObj.m[1][1] = matPose.m[1][1];
    matrixObj.m[1][2] = matPose.m[2][1];
    matrixObj.m[1][3] = 0.0f;
    matrixObj.m[2][0] = matPose.m[0][2];
    matrixObj.m[2][1] = matPose.m[1][2];
    matrixObj.m[2][2] = matPose.m[2][2];
    matrixObj.m[2][3] = 0.0f;
    matrixObj.m[3][0] = matPose.m[0][3];
    matrixObj.m[3][1] = matPose.m[1][3];
    matrixObj.m[3][2] = matPose.m[2][3];
    matrixObj.m[3][3] = 1.0f;
    return matrixObj;
}

// Функция для получения информации об устройстве
std::string GetTrackedDeviceString(vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError* peError = nullptr) {
    uint32_t unRequiredBufferLen = vr::VRSystem()->GetStringTrackedDeviceProperty(unDevice, prop, nullptr, 0, peError);
    if (unRequiredBufferLen == 0)
        return "";

    char* pchBuffer = new char[unRequiredBufferLen];
    unRequiredBufferLen = vr::VRSystem()->GetStringTrackedDeviceProperty(unDevice, prop, pchBuffer, unRequiredBufferLen, peError);
    std::string sResult = pchBuffer;
    delete[] pchBuffer;
    return sResult;
}

// Инициализация OpenVR
bool InitVR() {
    printf("Initializing OpenVR...\n");
    
    vr::EVRInitError eError = vr::VRInitError_None;
    g_pHMD = vr::VR_Init(&eError, vr::VRApplication_Scene);
    
    if (eError != vr::VRInitError_None) {
        g_pHMD = nullptr;
        printf("Unable to init VR runtime: %s\n", vr::VR_GetVRInitErrorAsEnglishDescription(eError));
        return false;
    }
    
    g_pCompositor = vr::VRCompositor();
    if (!g_pCompositor) {
        printf("Unable to initialize VR compositor!\n");
        vr::VR_Shutdown();
        g_pHMD = nullptr;
        return false;
    }
    
    printf("OpenVR initialized successfully!\n");
    
    // Выводим информацию о VR устройстве
    std::string deviceName = GetTrackedDeviceString(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_TrackingSystemName_String);
    std::string serialNumber = GetTrackedDeviceString(vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_SerialNumber_String);
    
    printf("Device: %s\n", deviceName.c_str());
    printf("Serial: %s\n", serialNumber.c_str());
    
    return true;
}

// Инициализация SDL и OpenGL
bool InitSDL() {
    printf("Initializing SDL...\n");
    
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    
    g_pWindow = SDL_CreateWindow("VR Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!g_pWindow) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    
    g_pContext = SDL_GL_CreateContext(g_pWindow);
    if (!g_pContext) {
        printf("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    
    // Инициализация GLEW
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        printf("GLEW initialization failed: %s\n", glewGetErrorString(err));
        return false;
    }
    
    printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
    printf("OpenGL Renderer: %s\n", glGetString(GL_RENDERER));
    
    // Настройки OpenGL
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    
    return true;
}

// Рендеринг простого куба
void RenderCube() {
    glBegin(GL_QUADS);
    
    // Передняя грань (красная)
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    
    // Задняя грань (зеленая)
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    
    // Верхняя грань (синяя)
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    
    // Нижняя грань (желтая)
    glColor3f(1.0f, 1.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    
    // Правая грань (пурпурная)
    glColor3f(1.0f, 0.0f, 1.0f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, -0.5f);
    glVertex3f(0.5f, 0.5f, 0.5f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    
    // Левая грань (голубая)
    glColor3f(0.0f, 1.0f, 1.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, 0.5f);
    glVertex3f(-0.5f, 0.5f, -0.5f);
    
    glEnd();
    glColor3f(1.0f, 1.0f, 1.0f); // Возвращаем белый цвет
}

// Основной цикл рендеринга
void RenderFrame() {
    if (!g_pHMD) return;
    
    // Очищаем экран
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Устанавливаем матрицу проекции
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f);
    
    // Устанавливаем матрицу модели
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0f, 0.0f, 5.0f,  // Позиция камеры
              0.0f, 0.0f, 0.0f,  // Точка, на которую смотрим
              0.0f, 1.0f, 0.0f); // Вектор "вверх"
    
    // Рендерим куб
    glTranslatef(0.0f, 0.0f, -2.0f);
    glRotatef(SDL_GetTicks() * 0.1f, 0.0f, 1.0f, 0.0f); // Вращение
    RenderCube();
    
    // Обновляем экран
    SDL_GL_SwapWindow(g_pWindow);
}

// Обработка событий
bool HandleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                return false;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    return false;
                }
                break;
        }
    }
    return true;
}

// Очистка ресурсов
void Cleanup() {
    printf("Cleaning up...\n");
    
    if (g_pCompositor) {
        g_pCompositor = nullptr;
    }
    
    if (g_pHMD) {
        vr::VR_Shutdown();
        g_pHMD = nullptr;
    }
    
    if (g_pContext) {
        SDL_GL_DeleteContext(g_pContext);
        g_pContext = nullptr;
    }
    
    if (g_pWindow) {
        SDL_DestroyWindow(g_pWindow);
        g_pWindow = nullptr;
    }
    
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    printf("=== VR Test Application ===\n");
    printf("This is a simple test to verify OpenVR -> SteamVR -> PICO 4 connection\n\n");
    
    // Инициализация VR
    if (!InitVR()) {
        printf("Failed to initialize VR!\n");
        return 1;
    }
    
    // Инициализация SDL и OpenGL
    if (!InitSDL()) {
        printf("Failed to initialize SDL/OpenGL!\n");
        Cleanup();
        return 1;
    }
    
    printf("\nVR Test is running!\n");
    printf("Press ESC to exit\n");
    printf("You should see a rotating colored cube\n");
    printf("If VR is working, you should see it in your PICO 4 headset\n\n");
    
    // Основной цикл
    bool running = true;
    while (running) {
        running = HandleEvents();
        RenderFrame();
        SDL_Delay(16); // ~60 FPS
    }
    
    Cleanup();
    printf("VR Test completed successfully!\n");
    return 0;
} 