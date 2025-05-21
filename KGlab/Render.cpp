#include "Render.h"
#include "MyOGL.h"
#include <Windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "GUItextRectangle.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>
#include "glext.h"
#define DigitTextureId(value) TexId_digit_##value

float animationTime = 0.0f;
std::vector<float> Times{ 23.5, 33.0, 44.5, 53.0, 64.5, 73.0, 83.5, 91.0, 99.0, 108.0, 118.5, 126.0, 133.0, 143.0, 150.0, 157.0, 164.5, 173.0, 181.0, 189.5, 196.5, 205.0, 213.0, 220.5, 228.5, 237.0, 245.0, 252.5, 260.5, 268.5, 277.0, 285.0, 293.0, 301.0 };

bool inMenu = true;
bool gameStarted = false;

struct Cube
{
    float scale = 0.0f;
    float rotation = 0.0f;
    bool growing = true;
    bool isFirstFive = true;
    float axis_X = 0.0f;
    float axis_Y = 0.0f;
    float axis_Z = 1.0f;
};

const float RESET_SCALE = 0.00390625f;//Новый размер после отката
const float TARGET_SCALE = 10.0f;//Размер для отката
int global_step = 10;

float scaleSpeed = 0.2f;
int compare = 15;
const int MAX_CUBES = 5;
std::vector<Cube> cubes;
const float MAX_SCALE = 2.0f;
bool firstCall = true;
int timeIndex = 0, limitIndex = 0;
GLuint texId, scoreTexId, backgroundTexId, rockTexId, endingTexId, TexId_digit_0, TexId_digit_1, TexId_digit_2, TexId_digit_3, TexId_digit_4, TexId_digit_5, TexId_digit_6, TexId_digit_7, TexId_digit_8, TexId_digit_9;
bool moreThanLimit = false;
unsigned const short limit = 33;
bool staticGame = true;
bool texturing = true;
bool lightning = true;
bool alpha = false;


void InitializeCubes()
{
    cubes.resize(MAX_CUBES);

    for (int i = 0; i < MAX_CUBES; i++)
    {
        cubes[i] = { RESET_SCALE, 0.0f,  true, true, 0.0f, 0.0f, 1.0f };
    }

    compare = 20;//Начальное время первого сброса
    animationTime = 0.0f;
}

void handleKeyPress(OpenGL* sender, KeyEventArg arg)
{
    //Запуск игры
    if (inMenu)
    {
        if (arg.key == '1')
        { 
            inMenu = false;
            gameStarted = true;
            InitializeCubes();
            animationTime = 0.0f;
        }
        else if (arg.key == '2')
        { 
        }
    }
    else if (gameStarted && arg.key == '2')
    {
        timeIndex = 0;
        inMenu = true;
        gameStarted = false;
        cubes.clear();//Очистка
    }
}

#ifdef _DEBUG
#include <Debugapi.h>
struct debug_print
{
    template<class C>
    debug_print& operator<<(const C& a)
    {
        OutputDebugStringA((std::stringstream() << a).str().c_str());
        return *this;
    }
} debout;
#else
struct debug_print
{
    template<class C>
    debug_print& operator<<(const C& a)
    {
        return *this;
    }
} debout;
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "MyOGL.h"
extern OpenGL gl;
#include "Light.h"
Light light;
#include "Camera.h"
Camera camera;
GuiTextRectangle text;


void initScore(int x, int y, int n, unsigned char* data)
{
    glGenTextures(1, &scoreTexId);
    glBindTexture(GL_TEXTURE_2D, scoreTexId);
    data = stbi_load("score.png", &x, &y, &n, 4);
    if (data)
    {
        unsigned char* _tmp = new unsigned char[x * 4];
        for (int i = 0; i < y / 2; ++i)
        {
            std::memcpy(_tmp, data + i * x * 4, x * 4);
            std::memcpy(data + i * x * 4, data + (y - 1 - i) * x * 4, x * 4);
            std::memcpy(data + (y - 1 - i) * x * 4, _tmp, x * 4);
        }
        delete[] _tmp;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    else
    {
        std::cerr << "Failed to load score texture!" << std::endl;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void initDigit_0(int x, int y, int n, unsigned char* data)
{
    glGenTextures(1, &TexId_digit_0);
    glBindTexture(GL_TEXTURE_2D, TexId_digit_0);
    data = stbi_load("digit_0.png", &x, &y, &n, 4);
    if (data)
    {
        unsigned char* _tmp = new unsigned char[x * 4];
        for (int i = 0; i < y / 2; ++i)
        {
            std::memcpy(_tmp, data + i * x * 4, x * 4);
            std::memcpy(data + i * x * 4, data + (y - 1 - i) * x * 4, x * 4);
            std::memcpy(data + (y - 1 - i) * x * 4, _tmp, x * 4);
        }
        delete[] _tmp;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    else
    {
        std::cerr << "Failed to load score texture!" << std::endl;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void initDigit_1(int x, int y, int n, unsigned char* data)
{
    glGenTextures(1, &TexId_digit_1);
    glBindTexture(GL_TEXTURE_2D, TexId_digit_1);
    data = stbi_load("digit_1.png", &x, &y, &n, 4);
    if (data)
    {
        unsigned char* _tmp = new unsigned char[x * 4];
        for (int i = 0; i < y / 2; ++i)
        {
            std::memcpy(_tmp, data + i * x * 4, x * 4);
            std::memcpy(data + i * x * 4, data + (y - 1 - i) * x * 4, x * 4);
            std::memcpy(data + (y - 1 - i) * x * 4, _tmp, x * 4);
        }
        delete[] _tmp;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    else
    {
        std::cerr << "Failed to load score texture!" << std::endl;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void initDigit_2(int x, int y, int n, unsigned char* data)
{
    glGenTextures(1, &TexId_digit_2);
    glBindTexture(GL_TEXTURE_2D, TexId_digit_2);
    data = stbi_load("digit_2.png", &x, &y, &n, 4);
    if (data)
    {
        unsigned char* _tmp = new unsigned char[x * 4];
        for (int i = 0; i < y / 2; ++i)
        {
            std::memcpy(_tmp, data + i * x * 4, x * 4);
            std::memcpy(data + i * x * 4, data + (y - 1 - i) * x * 4, x * 4);
            std::memcpy(data + (y - 1 - i) * x * 4, _tmp, x * 4);
        }
        delete[] _tmp;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    else
    {
        std::cerr << "Failed to load score texture!" << std::endl;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void initDigit_3(int x, int y, int n, unsigned char* data)
{
    glGenTextures(1, &TexId_digit_3);
    glBindTexture(GL_TEXTURE_2D, TexId_digit_3);
    data = stbi_load("digit_3.png", &x, &y, &n, 4);
    if (data)
    {
        unsigned char* _tmp = new unsigned char[x * 4];
        for (int i = 0; i < y / 2; ++i)
        {
            std::memcpy(_tmp, data + i * x * 4, x * 4);
            std::memcpy(data + i * x * 4, data + (y - 1 - i) * x * 4, x * 4);
            std::memcpy(data + (y - 1 - i) * x * 4, _tmp, x * 4);
        }
        delete[] _tmp;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    else
    {
        std::cerr << "Failed to load score texture!" << std::endl;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void initDigit_4(int x, int y, int n, unsigned char* data)
{
    glGenTextures(1, &TexId_digit_4);
    glBindTexture(GL_TEXTURE_2D, TexId_digit_4);
    data = stbi_load("digit_4.png", &x, &y, &n, 4);
    if (data)
    {
        unsigned char* _tmp = new unsigned char[x * 4];
        for (int i = 0; i < y / 2; ++i)
        {
            std::memcpy(_tmp, data + i * x * 4, x * 4);
            std::memcpy(data + i * x * 4, data + (y - 1 - i) * x * 4, x * 4);
            std::memcpy(data + (y - 1 - i) * x * 4, _tmp, x * 4);
        }
        delete[] _tmp;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    else
    {
        std::cerr << "Failed to load score texture!" << std::endl;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void initDigit_5(int x, int y, int n, unsigned char* data)
{
    glGenTextures(1, &TexId_digit_5);
    glBindTexture(GL_TEXTURE_2D, TexId_digit_5);
    data = stbi_load("digit_5.png", &x, &y, &n, 4);
    if (data)
    {
        unsigned char* _tmp = new unsigned char[x * 4];
        for (int i = 0; i < y / 2; ++i)
        {
            std::memcpy(_tmp, data + i * x * 4, x * 4);
            std::memcpy(data + i * x * 4, data + (y - 1 - i) * x * 4, x * 4);
            std::memcpy(data + (y - 1 - i) * x * 4, _tmp, x * 4);
        }
        delete[] _tmp;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    else
    {
        std::cerr << "Failed to load score texture!" << std::endl;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void initDigit_6(int x, int y, int n, unsigned char* data)
{
    glGenTextures(1, &TexId_digit_6);
    glBindTexture(GL_TEXTURE_2D, TexId_digit_6);
    data = stbi_load("digit_6.png", &x, &y, &n, 4);
    if (data)
    {
        unsigned char* _tmp = new unsigned char[x * 4];
        for (int i = 0; i < y / 2; ++i)
        {
            std::memcpy(_tmp, data + i * x * 4, x * 4);
            std::memcpy(data + i * x * 4, data + (y - 1 - i) * x * 4, x * 4);
            std::memcpy(data + (y - 1 - i) * x * 4, _tmp, x * 4);
        }
        delete[] _tmp;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    else
    {
        std::cerr << "Failed to load score texture!" << std::endl;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void initDigit_7(int x, int y, int n, unsigned char* data)
{
    glGenTextures(1, &TexId_digit_7);
    glBindTexture(GL_TEXTURE_2D, TexId_digit_7);
    data = stbi_load("digit_7.png", &x, &y, &n, 4);
    if (data)
    {
        unsigned char* _tmp = new unsigned char[x * 4];
        for (int i = 0; i < y / 2; ++i)
        {
            std::memcpy(_tmp, data + i * x * 4, x * 4);
            std::memcpy(data + i * x * 4, data + (y - 1 - i) * x * 4, x * 4);
            std::memcpy(data + (y - 1 - i) * x * 4, _tmp, x * 4);
        }
        delete[] _tmp;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    else
    {
        std::cerr << "Failed to load score texture!" << std::endl;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void initDigit_8(int x, int y, int n, unsigned char* data)
{
    glGenTextures(1, &TexId_digit_8);
    glBindTexture(GL_TEXTURE_2D, TexId_digit_8);
    data = stbi_load("digit_8.png", &x, &y, &n, 4);
    if (data)
    {
        unsigned char* _tmp = new unsigned char[x * 4];
        for (int i = 0; i < y / 2; ++i)
        {
            std::memcpy(_tmp, data + i * x * 4, x * 4);
            std::memcpy(data + i * x * 4, data + (y - 1 - i) * x * 4, x * 4);
            std::memcpy(data + (y - 1 - i) * x * 4, _tmp, x * 4);
        }
        delete[] _tmp;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    else
    {
        std::cerr << "Failed to load score texture!" << std::endl;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void initDigit_9(int x, int y, int n, unsigned char* data)
{
    glGenTextures(1, &TexId_digit_9);
    glBindTexture(GL_TEXTURE_2D, TexId_digit_9);
    data = stbi_load("digit_9.png", &x, &y, &n, 4);
    if (data)
    {
        unsigned char* _tmp = new unsigned char[x * 4];
        for (int i = 0; i < y / 2; ++i)
        {
            std::memcpy(_tmp, data + i * x * 4, x * 4);
            std::memcpy(data + i * x * 4, data + (y - 1 - i) * x * 4, x * 4);
            std::memcpy(data + (y - 1 - i) * x * 4, _tmp, x * 4);
        }
        delete[] _tmp;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    else
    {
        std::cerr << "Failed to load score texture!" << std::endl;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void initBackground(int x, int y, int n, unsigned char* data)
{
    glGenTextures(1, &backgroundTexId);
    glBindTexture(GL_TEXTURE_2D, backgroundTexId);
    data = stbi_load("TheSpace.png", &x, &y, &n, 4);
    if (data)
    {
        unsigned char* _tmp = new unsigned char[x * 4];
        for (int i = 0; i < y / 2; ++i)
        {
            std::memcpy(_tmp, data + i * x * 4, x * 4);
            std::memcpy(data + i * x * 4, data + (y - 1 - i) * x * 4, x * 4);
            std::memcpy(data + (y - 1 - i) * x * 4, _tmp, x * 4);
        }
        delete[] _tmp;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    else
    {
        std::cerr << "Failed to load score texture!" << std::endl;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void initRockTexture(int x, int y, int n, unsigned char* data)
{
    glGenTextures(1, &rockTexId);
    glBindTexture(GL_TEXTURE_2D, rockTexId);
    data = stbi_load("RockTexture.png", &x, &y, &n, 4);
    if (data)
    {
        unsigned char* _tmp = new unsigned char[x * 4];
        for (int i = 0; i < y / 2; ++i)
        {
            std::memcpy(_tmp, data + i * x * 4, x * 4);
            std::memcpy(data + i * x * 4, data + (y - 1 - i) * x * 4, x * 4);
            std::memcpy(data + (y - 1 - i) * x * 4, _tmp, x * 4);
        }
        delete[] _tmp;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    else
    {
        std::cerr << "Failed to load score texture!" << std::endl;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void initEnding(int x, int y, int n, unsigned char* data)
{
    glGenTextures(1, &endingTexId);
    glBindTexture(GL_TEXTURE_2D, endingTexId);
    data = stbi_load("TheEnding.png", &x, &y, &n, 4);
    if (data)
    {
        unsigned char* _tmp = new unsigned char[x * 4];
        for (int i = 0; i < y / 2; ++i)
        {
            std::memcpy(_tmp, data + i * x * 4, x * 4);
            std::memcpy(data + i * x * 4, data + (y - 1 - i) * x * 4, x * 4);
            std::memcpy(data + (y - 1 - i) * x * 4, _tmp, x * 4);
        }
        delete[] _tmp;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    else
    {
        std::cerr << "Failed to load score texture!" << std::endl;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void initRender()
{
    int x, y, n;
    unsigned char* data;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    //Инициализация текстуры Menu
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    data = stbi_load("MainMenu.png", &x, &y, &n, 4);
    if (data) 
    {
        unsigned char* _tmp = new unsigned char[x * 4];
        for (int i = 0; i < y / 2; ++i)
        {
            std::memcpy(_tmp, data + i * x * 4, x * 4);
            std::memcpy(data + i * x * 4, data + (y - 1 - i) * x * 4, x * 4);
            std::memcpy(data + (y - 1 - i) * x * 4, _tmp, x * 4);
        }
        delete[] _tmp;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    else
    {
        std::cerr << "Failed to load MainMenu texture!" << std::endl;
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //Инициализация текстуры Score
    initScore(x, y, n, data);
    //Инициализация текстуры digit_0
    initDigit_0(x, y, n, data);
    //Инициализация текстуры digit_1
    initDigit_1(x, y, n, data);
    //Инициализация текстуры digit_2
    initDigit_2(x, y, n, data);
    //Инициализация текстуры digit_3
    initDigit_3(x, y, n, data);
    //Инициализация текстуры digit_4
    initDigit_4(x, y, n, data);
    //Инициализация текстуры digit_5
    initDigit_5(x, y, n, data);
    //Инициализация текстуры digit_6
    initDigit_6(x, y, n, data);
    //Инициализация текстуры digit_7
    initDigit_7(x, y, n, data);
    //Инициализация текстуры digit_8
    initDigit_8(x, y, n, data);
    //Инициализация текстуры digit_9
    initDigit_9(x, y, n, data);
    //Инициализация текстуры фона
    initBackground(x, y, n, data);
    //Инициализация текстуры камня
    initRockTexture(x, y, n, data);
    //Инициализация текстуры концовки
    initEnding(x, y, n, data);

    camera.caclulateCameraPos();
    gl.WheelEvent.reaction(&camera, &Camera::Zoom);
    gl.MouseMovieEvent.reaction(&camera, &Camera::MouseMovie);
    gl.MouseLeaveEvent.reaction(&camera, &Camera::MouseLeave);
    gl.MouseLdownEvent.reaction(&camera, &Camera::MouseStartDrag);
    gl.MouseLupEvent.reaction(&camera, &Camera::MouseStopDrag);

    gl.MouseMovieEvent.reaction(&light, &Light::MoveLight);
    gl.KeyDownEvent.reaction(&light, &Light::StartDrug);
    gl.KeyUpEvent.reaction(&light, &Light::StopDrug);

    gl.KeyDownEvent.reaction(handleKeyPress);
    text.setSize(512, 180);

    camera.setPosition(2, 1.5, 1.5);
}

void drawCubeWithHole()
{
    double A[]{ 0.5,0.5,-0.5 };
    double B[]{ 0.5,-0.5,-0.5 };
    double C[]{ -0.5,-0.5,-0.5 };
    double D[]{ -0.5,0.5,-0.5 };
    double A1[]{ 0.5,0.5,0.5 };
    double B1[]{ 0.5,-0.5,0.5 };
    double C1[]{ -0.5,-0.5,0.5 };
    double D1[]{ -0.5,0.5,0.5 };

    double S1[]{ 0.5, 0.5, 0 };
    double S2[]{ 0.5, -0.5, 0 };
    double S3[]{ -0.5, -0.5, 0 };
    double S4[]{ -0.5, 0.5, 0 };
    double S5[]{ 0.5, 0, 0.5 };
    double S6[]{ 0, 0.5, 0.5 };
    double S7[]{ -0.5, 0, 0.5 };

    double O1[]{ 0.5, 0, 0 };
    double O2[]{ 0, 0, 0.5 };
    double O3[]{ 0, 0.5, 0 };
    glBindTexture(GL_TEXTURE_2D, rockTexId);
    glBegin(GL_QUADS);

    glNormal3d(1, 0, 0);
    glTexCoord2d(0, 0);
    glVertex3dv(A);
    glTexCoord2d(0, 1);
    glVertex3dv(S1);
    glTexCoord2d(1, 0);
    glVertex3dv(S2);
    glTexCoord2d(1, 1);
    glVertex3dv(B);
    glTexCoord2d(0, 0);
    glVertex3dv(O1);
    glTexCoord2d(0, 1);
    glVertex3dv(S2);
    glTexCoord2d(1, 0);
    glVertex3dv(B1);
    glTexCoord2d(1, 1);
    glVertex3dv(S5);

    glTexCoord2d(0, 0);
    glVertex3dv(C);
    glTexCoord2d(0, 1);
    glVertex3dv(D);
    glTexCoord2d(1, 0);
    glVertex3dv(D1);
    glTexCoord2d(1, 1);
    glVertex3dv(C1);

    glTexCoord2d(0, 0);
    glVertex3dv(D);
    glTexCoord2d(0, 1);
    glVertex3dv(S4);
    glTexCoord2d(1, 0);
    glVertex3dv(S1);
    glTexCoord2d(1, 1);
    glVertex3dv(A);
    glTexCoord2d(0, 0);
    glVertex3dv(S4);
    glTexCoord2d(0, 1);
    glVertex3dv(D1);
    glTexCoord2d(1, 0);
    glVertex3dv(S6);
    glTexCoord2d(1, 1);
    glVertex3dv(O3);

    glTexCoord2d(0, 0);
    glVertex3dv(B);
    glTexCoord2d(0, 1);
    glVertex3dv(C);
    glTexCoord2d(1, 0);
    glVertex3dv(C1);
    glTexCoord2d(1, 1);
    glVertex3dv(B1);

    glTexCoord2d(0, 0);
    glVertex3dv(B1);
    glTexCoord2d(0, 1);
    glVertex3dv(C1);
    glTexCoord2d(1, 0);
    glVertex3dv(S7);
    glTexCoord2d(1, 1);
    glVertex3dv(S5);
    glTexCoord2d(0, 0);
    glVertex3dv(O2);
    glTexCoord2d(0, 1);
    glVertex3dv(S7);
    glTexCoord2d(1, 0);
    glVertex3dv(D1);
    glTexCoord2d(1, 1);
    glVertex3dv(S6);

    glTexCoord2d(0, 0);
    glVertex3dv(A);
    glTexCoord2d(0, 1);
    glVertex3dv(B);
    glTexCoord2d(1, 0);
    glVertex3dv(C);
    glTexCoord2d(1, 1);
    glVertex3dv(D);
    glEnd();
}

void drawCube(Cube& cube, int firstFive, float animationTime)
{
    if (cube.isFirstFive)
    {
        //Первые 5 кубов
        if (animationTime > 5)
            cube.isFirstFive = false;
        glPushMatrix();
        glScalef(cube.scale, cube.scale, cube.scale);
        drawCubeWithHole();
        glPopMatrix();
    }
    else
    {
        //Все остальные кубы. Смена углов.
        glPushMatrix();
        glScalef(cube.scale, cube.scale, cube.scale);

        glRotatef(cube.rotation, cube.axis_X, cube.axis_Y, cube.axis_Z);

        drawCubeWithHole();
        glPopMatrix();

        if (animationTime >= 20 && animationTime < 50)
        {
            scaleSpeed = 0.6f;
            switch (firstFive)
            {
            case 0: cube.rotation = 90.0f; break;
            case 1: cube.rotation = -90.0f; break;
            case 2: cube.rotation = 45.0f; break;
            case 3: cube.rotation = -45.0f; break;
            case 4: cube.rotation = 180.0f; break;
            }
        }

        //Сложный уровень
        if (animationTime >= 50 && animationTime < 100)
        {
            scaleSpeed = 0.6f;
            global_step = 9;
            switch (firstFive)
            {
            case 0:
            {
                float timeBasedRotation = fmod(animationTime * 20.0f, 360.0f);
                cube.rotation = timeBasedRotation; break;
            }
            case 1: cube.rotation = -90.0f; break;
            case 2:
            {
                float timeBasedRotation = fmod(animationTime * (-20.0f), 360.0f);
                cube.rotation = timeBasedRotation; break;
            }
            case 3: cube.rotation = 180.0f; break;
            case 4:
            {
                float timeBasedRotation = fmod(animationTime * 20.0f, 360.0f);
                cube.rotation = timeBasedRotation; break;
            }
            }
        }
        if (animationTime >= 120)
        {
            global_step = 8;
            if (animationTime < 150)
                scaleSpeed = 0.8f;
            switch (firstFive)
            {
            case 0:
            {
                float timeBasedRotation = fmod(animationTime * 20.0f, 360.0f);
                cube.rotation = timeBasedRotation; break;
            }
            case 1: cube.rotation = -90.0f; break;
            case 2:
            {
                float timeBasedRotation = fmod(animationTime * (-20.0f), 360.0f);
                cube.rotation = timeBasedRotation; break;
            }
            case 3: cube.rotation = 180.0f; break;
            case 4:
            {
                float timeBasedRotation = fmod(animationTime * 20.0f, 360.0f);
                cube.rotation = timeBasedRotation; break;
            }
            }
        }
        switch (compare)
        {
        case 50:
        {
            cube.axis_X = 1;
            cube.axis_Y = 0;
            cube.axis_Z = 0;
            break;
        }
        case 70:
        {
            cube.axis_X = 0;
            cube.axis_Y = 1;
            cube.axis_Z = 0;
            global_step = 9;
            break;
        }
        case 90:
        {
            cube.axis_X = 0;
            cube.axis_Y = 0;
            cube.axis_Z = 1;
            break;
        }
        case 110:
        {
            cube.axis_X = 1;
            cube.axis_Y = 1;
            cube.axis_Z = 0;
            break;
        }
        case 130:
        {
            cube.axis_X = 1;
            cube.axis_Y = 0;
            cube.axis_Z = 1;
            break;
        }
        case 150:
        {
            cube.axis_X = 0;
            cube.axis_Y = 1;
            cube.axis_Z = 1;
            break;
        }
        }
    }
}

void drawMenuImage()
{
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texId);

    double menuA[]{ 1,1,-1 };
    double menuB[]{ 1,-1,-1 };
    double menuC[]{ -1,-1,-1 };
    double menuD[]{ -1,1,-1 };
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);

    glNormal3d(0, 0, 1);
    glTexCoord2d(1, 1);
    glVertex3dv(menuA);
    glTexCoord2d(1, 0);
    glVertex3dv(menuB);
    glTexCoord2d(0, 0);
    glVertex3dv(menuC);
    glTexCoord2d(0, 1);
    glVertex3dv(menuD);

    glEnd();

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
}

void drawScoreDigit(int timeIndex)
{
    if (timeIndex < 10)
    {
        switch (timeIndex)
        {
        case 0: glBindTexture(GL_TEXTURE_2D, DigitTextureId(0)); break;
        case 1: glBindTexture(GL_TEXTURE_2D, DigitTextureId(1)); break;
        case 2: glBindTexture(GL_TEXTURE_2D, DigitTextureId(2)); break;
        case 3: glBindTexture(GL_TEXTURE_2D, DigitTextureId(3)); break;
        case 4: glBindTexture(GL_TEXTURE_2D, DigitTextureId(4)); break;
        case 5: glBindTexture(GL_TEXTURE_2D, DigitTextureId(5)); break;
        case 6: glBindTexture(GL_TEXTURE_2D, DigitTextureId(6)); break;
        case 7: glBindTexture(GL_TEXTURE_2D, DigitTextureId(7)); break;
        case 8: glBindTexture(GL_TEXTURE_2D, DigitTextureId(8)); break;
        case 9: glBindTexture(GL_TEXTURE_2D, DigitTextureId(9)); break;
        }
        //digit=====
        glColor3f(0.9f, 0.9f, 0.9f);
        glBegin(GL_QUADS);

        glNormal3d(0, 0, 1);
        glTexCoord2d(1, 1);
        glVertex2f(0.98, 1.0);
        glTexCoord2d(1, 0);
        glVertex2f(0.98, 0.9);
        glTexCoord2d(0, 0);
        glVertex2f(0.95, 0.9);
        glTexCoord2d(0, 1);
        glVertex2f(0.95, 1.0);

        glEnd();
        //digit=====
    }
    else
    {
        //Выбор первой цифры
        switch (timeIndex / 10)
        {
        case 1: glBindTexture(GL_TEXTURE_2D, DigitTextureId(1)); break;
        case 2: glBindTexture(GL_TEXTURE_2D, DigitTextureId(2)); break;
        case 3: glBindTexture(GL_TEXTURE_2D, DigitTextureId(3)); break;
        case 4: glBindTexture(GL_TEXTURE_2D, DigitTextureId(4)); break;
        case 5: glBindTexture(GL_TEXTURE_2D, DigitTextureId(5)); break;
        case 6: glBindTexture(GL_TEXTURE_2D, DigitTextureId(6)); break;
        case 7: glBindTexture(GL_TEXTURE_2D, DigitTextureId(7)); break;
        case 8: glBindTexture(GL_TEXTURE_2D, DigitTextureId(8)); break;
        case 9: glBindTexture(GL_TEXTURE_2D, DigitTextureId(9)); break;
        }
        //Первая цифра=====
        glColor3f(0.9f, 0.9f, 0.9f);
        glBegin(GL_QUADS);

        glNormal3d(0, 0, 1);
        glTexCoord2d(1, 1);
        glVertex2f(0.97, 1.0);
        glTexCoord2d(1, 0);
        glVertex2f(0.97, 0.9);
        glTexCoord2d(0, 0);
        glVertex2f(0.94, 0.9);
        glTexCoord2d(0, 1);
        glVertex2f(0.94, 1.0);

        glEnd();
        //Первая цифра=====

        //Выбор второй цифры
        switch (timeIndex % 10)
        {
        case 0: glBindTexture(GL_TEXTURE_2D, DigitTextureId(0)); break;
        case 1: glBindTexture(GL_TEXTURE_2D, DigitTextureId(1)); break;
        case 2: glBindTexture(GL_TEXTURE_2D, DigitTextureId(2)); break;
        case 3: glBindTexture(GL_TEXTURE_2D, DigitTextureId(3)); break;
        case 4: glBindTexture(GL_TEXTURE_2D, DigitTextureId(4)); break;
        case 5: glBindTexture(GL_TEXTURE_2D, DigitTextureId(5)); break;
        case 6: glBindTexture(GL_TEXTURE_2D, DigitTextureId(6)); break;
        case 7: glBindTexture(GL_TEXTURE_2D, DigitTextureId(7)); break;
        case 8: glBindTexture(GL_TEXTURE_2D, DigitTextureId(8)); break;
        case 9: glBindTexture(GL_TEXTURE_2D, DigitTextureId(9)); break;
        }
        //Вторая цифра=====
        glColor3f(0.9f, 0.9f, 0.9f);
        glBegin(GL_QUADS);

        glNormal3d(0, 0, 1);
        glTexCoord2d(1, 1);
        glVertex2f(1.0, 1.0);
        glTexCoord2d(1, 0);
        glVertex2f(1.0, 0.9);
        glTexCoord2d(0, 0);
        glVertex2f(0.97, 0.9);
        glTexCoord2d(0, 1);
        glVertex2f(0.97, 1.0);

        glEnd();
        //Вторая цифра=====
    }
}

void drawBackground()
{
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, backgroundTexId);

    double backA[]{ 1,1,1 };
    double backB[]{ 1,-1,1 };
    double backC[]{ -1,-1,1 };
    double backD[]{ -1,1,1 };
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);

    glNormal3d(0, 0, 1);
    glTexCoord2d(1, 1);
    glVertex3dv(backA);
    glTexCoord2d(1, 0);
    glVertex3dv(backB);
    glTexCoord2d(0, 0);
    glVertex3dv(backC);
    glTexCoord2d(0, 1);
    glVertex3dv(backD);

    glEnd();

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

void drawScore(int timeIndex)
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1, 0, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, scoreTexId);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    //Score=====
    glColor3f(0.9f, 0.9f, 0.9f);
    glBegin(GL_QUADS);
    glNormal3d(0, 0, 1);
    glTexCoord2d(1, 1);
    glVertex2f(1.0, 1.0);
    glTexCoord2d(1, 0);
    glVertex2f(1.0, 0.9);
    glTexCoord2d(0, 0);
    glVertex2f(0.8, 0.9);
    glTexCoord2d(0, 1);
    glVertex2f(0.8, 1.0);
    glEnd();
    //digit
    drawScoreDigit(timeIndex);
    //digit
    //Score=====
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void drawEnding()
{
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, endingTexId);

    double menuA[]{ 1,1,-1 };
    double menuB[]{ 1,-1,-1 };
    double menuC[]{ -1,-1,-1 };
    double menuD[]{ -1,1,-1 };
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);

    glNormal3d(0, 0, 1);
    glTexCoord2d(1, 1);
    glVertex3dv(menuA);
    glTexCoord2d(1, 0);
    glVertex3dv(menuB);
    glTexCoord2d(0, 0);
    glVertex3dv(menuC);
    glTexCoord2d(0, 1);
    glVertex3dv(menuD);

    glEnd();

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
}

void Render(double delta_time)
{
    drawBackground();

    if (inMenu)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawMenuImage();
        return;
    }

    if (timeIndex == 100)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        drawEnding();
        return;
    }

    if (!gameStarted) return;

    camera.SetUpCamera();

    animationTime += delta_time;

    if (cubes.empty())
    {
        InitializeCubes();
        return; 
    }

    //Обновление размеров всех кубов
    for (size_t i = 0; i < cubes.size(); i++)
    {
        if (cubes[i].growing)
        {
            switch (i)
            {
            case 0: cubes[i].scale += 1.9 * scaleSpeed * delta_time; break;
            case 1: cubes[i].scale += 1.7 * scaleSpeed * delta_time; break;
            case 2: cubes[i].scale += 1.5 * scaleSpeed * delta_time; break;
            case 3: cubes[i].scale += 1.3 * scaleSpeed * delta_time; break;
            case 4: cubes[i].scale += 1.1 * scaleSpeed * delta_time; break;
            }

        }
    }

    //Поиск самого большого куба (внешнего)
    int largestIndex = 0;
    for (int i = 1; i < MAX_CUBES; i++)
    {
        if (cubes[i].scale > cubes[largestIndex].scale)
        {
            largestIndex = i;
        }
    }
    //Проверка на сброс
    if (animationTime >= compare)
    {
        compare += global_step; //Следующий сброс через global_step секунд (начиная с 10)

        //Поиск самого большого куба (внешнего)
        int largestIndex = 0;
        for (int i = 1; i < MAX_CUBES; i++)
        {
            if (cubes[i].scale > cubes[largestIndex].scale)
            {
                largestIndex = i;
            }
        }

        //Сброс
        cubes[largestIndex].scale = RESET_SCALE;
    }
    if (compare % 50 == 0)
        scaleSpeed = 0.4f;
    if (animationTime >= 150 && animationTime < 200)
    {
        scaleSpeed = 1.2f;
        global_step = 5;
    }
    else
    {
        if (animationTime >= 200 && animationTime < 300)
        {
            scaleSpeed = 1.5f;
            global_step = 3;
        }
        else
        {
            if (animationTime >= 300)
            {
                scaleSpeed = 2.0f;
                global_step = 1;
            }
        }
    }

    //Настройка освещения и материалов
    glEnable(GL_LIGHTING);

    float camLightAmb[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    float camLightDiff[] = { 0.9f, 0.9f, 0.8f, 1.0f };
    float camLightSpec[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    GLfloat modelView[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, modelView);
    float camX = -(modelView[0] * modelView[12] + modelView[1] * modelView[13] + modelView[2] * modelView[14]);
    float camY = -(modelView[4] * modelView[12] + modelView[5] * modelView[13] + modelView[6] * modelView[14]);
    float camZ = -(modelView[8] * modelView[12] + modelView[9] * modelView[13] + modelView[10] * modelView[14]);
    float camDirX = -modelView[8];
    float camDirY = -modelView[9];
    float camDirZ = -modelView[10];

    float camLightPos[] = { camX, camY, camZ, 1.0f };
    float camLightDir[] = { camDirX, camDirY, camDirZ };

    glLightfv(GL_LIGHT0, GL_POSITION, camLightPos);
    glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, camLightDir);
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 45.0f);
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 2.0f);
    glLightfv(GL_LIGHT0, GL_AMBIENT, camLightAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, camLightDiff);
    glLightfv(GL_LIGHT0, GL_SPECULAR, camLightSpec);
    glEnable(GL_LIGHT0);

    float fillLightAmb[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    float fillLightDiff[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    float fillLightPos[] = { 0.0f, 0.0f, 0.0f, 1.0f };

    glLightfv(GL_LIGHT1, GL_POSITION, fillLightPos);
    glLightfv(GL_LIGHT1, GL_AMBIENT, fillLightAmb);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, fillLightDiff);
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.1f);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.01f);
    glEnable(GL_LIGHT1);

    float cornerLightDiff[] = { 0.4f, 0.4f, 0.4f, 1.0f };
    float cornerLightPos1[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    float cornerLightPos2[] = { -0.5f, -0.5f, -0.5f, 1.0f };

    glLightfv(GL_LIGHT2, GL_POSITION, cornerLightPos1);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, cornerLightDiff);
    glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 1.0f);
    glEnable(GL_LIGHT2);

    glLightfv(GL_LIGHT3, GL_POSITION, cornerLightPos2);
    glLightfv(GL_LIGHT3, GL_DIFFUSE, cornerLightDiff);
    glLightf(GL_LIGHT3, GL_CONSTANT_ATTENUATION, 1.0f);
    glEnable(GL_LIGHT3);

    glPointSize(8);
    glBegin(GL_POINTS);
    glColor3f(1, 1, 0); glVertex3fv(camLightPos);
    glColor3f(0, 1, 1); glVertex3fv(fillLightPos);
    glColor3f(1, 0, 1); glVertex3fv(cornerLightPos1);
    glVertex3fv(cornerLightPos2);
    glEnd();

    float matAmb[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    float matDiff[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    float matSpec[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    float matShininess = 32.0f;

    glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiff);
    glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);
    glMaterialf(GL_FRONT, GL_SHININESS, matShininess);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texId);

    //Отрисовка всех активных кубов
    for (size_t i = 0; i < cubes.size(); i++)
    {
        drawCube(cubes[i], i, animationTime);
    }

    drawScore(timeIndex);
    if (!moreThanLimit)
    {
        if (animationTime > Times[timeIndex] && animationTime < Times[timeIndex] + 1)
        {
            if (timeIndex < Times.size() - 1)
                timeIndex++;
            else
            {
                moreThanLimit = true;
                Times[0] = Times.back() + 8.0;
                Times[1] = Times[0] + 8.0;
                timeIndex = Times.size() - 1;
            }
        }
    }
    else
    {
        if (animationTime > Times[limitIndex] && animationTime < Times[limitIndex] + 1)
        {
            timeIndex++;
            limitIndex = (timeIndex + 1) % 2;
            Times[limitIndex] = Times[(limitIndex + 1) % 2] + 8.0;
        }
    }
}