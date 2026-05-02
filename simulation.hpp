#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <list>
#include <chrono>
#include <cstdint>

namespace engine
{
    const float minX = -1.0f;
    inline float maxX = 1.0f;
    const float minY = -1.0f;
    const float maxY = 1.0f;
    inline float g = 1.0f;
    inline float timeScale = 0.25f;
    inline bool symulacjaAktywna = true;
    const bool mierzCzas = false;
    inline uint8_t liczbaWatkow = 1;
    inline uint8_t maxLiczbaWatkow;
    inline uint16_t nextId = 0;

    struct Ball
    {
        float radius;
        float m;
        float px;
        float py;
        float vx;
        float vy;
        uint16_t id;
        uint8_t r;
        uint8_t g;
        uint8_t b;
    };

    class Simulation
    {
        public:
        std::vector<Ball> balls;
        float dt;
        std::chrono::_V2::system_clock::time_point start;
        std::chrono::_V2::system_clock::time_point end;

        Simulation();

        void Update();

        void CalcDeltaTime();

        void MoveBallsFromRange(uint16_t poczatek, uint16_t koniec);

        static void Collision(Ball& ball1, Ball& ball2);
        void AddGas(uint16_t amount, float radius, float m, float left, float right, float up, float down, uint8_t r, uint8_t g, uint8_t b);
        void AddGas(uint16_t amount, float radius, float m, uint8_t r, uint8_t g, uint8_t b);
        void AddBall(float radius, float m, float px, float py, float vx, float vy, uint8_t r, uint8_t g, uint8_t b);
        void ClearAll();
    };
}