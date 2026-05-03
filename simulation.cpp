#include "simulation.hpp"
#include <cmath>
#include <iostream>
#include <execution>
#include <thread>
#include <future>

namespace engine
{
    Simulation::Simulation()
    {
        end = std::chrono::high_resolution_clock::now();
        srand(time(NULL));
        liczbaWatkow = std::thread::hardware_concurrency();
    }

    void Simulation::Update()
    {
        CalcDeltaTime();
        if (!symulacjaAktywna) return;
        for (uint8_t k = 0; k < krokiFizyki; k++)
        {
            const uint16_t size = static_cast<uint16_t>(balls.size());
            if (size == 0) return;

            std::chrono::_V2::system_clock::time_point t1;
            if (mierzCzas)
                t1 = std::chrono::high_resolution_clock::now();

            // poruszanie kulek wielowątkowo
            std::for_each(std::execution::par_unseq, balls.begin(), balls.end(),
            [&](Ball& b)
            {
                b.vy += 0.5f * g * dt;
                b.px += b.vx * dt;
                b.py += b.vy * dt;
                b.vy += 0.5f * g * dt;
                if (b.px - b.radius < minX)
                {
                    b.px = minX + b.radius;
                    b.vx = -b.vx;
                }
                else if (b.px + b.radius > maxX)
                {
                    b.px = maxX - b.radius;
                    b.vx = -b.vx;
                }
                if (b.py - b.radius < minY)
                {
                    b.py = minY + b.radius;
                    b.vy = -b.vy;
                }
                else if (b.py + b.radius > maxY)
                {
                    b.py = maxY - b.radius;
                    b.vy = -b.vy;
                }
            }
            );

            // poruszanie kulek jednowątkowo
            /*for (Ball& ball : balls)
            {
                ball.vy += 0.5f * g * dt;
                ball.px += ball.vx * dt;
                ball.py += ball.vy * dt;
                ball.vy += 0.5f * g * dt;
                if (ball.px - ball.radius < minX)
                {
                    ball.px = minX + ball.radius;
                    ball.vx = -ball.vx;
                }
                else if (ball.px + ball.radius > maxX)
                {
                    ball.px = maxX - ball.radius;
                    ball.vx = -ball.vx;
                }
                if (ball.py - ball.radius < minY)
                {
                    ball.py = minY + ball.radius;
                    ball.vy = -ball.vy;
                }
                else if (ball.py + ball.radius > maxY)
                {
                    ball.py = maxY - ball.radius;
                    ball.vy = -ball.vy;
                }
            }*/

            if (mierzCzas)
            {
                auto t2 = std::chrono::high_resolution_clock::now();
                auto t = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
                std::cout<<"Czas poruszania kulek: "<<t.count()<<"us"<<'\n';

                t1 = std::chrono::high_resolution_clock::now();
            }

            // sorotwanie jednowątkowe
            /*std::sort(balls.begin(), balls.end(),
            [](const Ball& a, const Ball& b) {
                return a.px - a.radius < b.px - b.radius;
            });*/

            // sortowanie wielowątkowe
            std::sort(std::execution::par_unseq, balls.begin(), balls.end(),
            [](const Ball& a, const Ball& b)
            {
                return a.px - a.radius < b.px - b.radius;
            });

            if (mierzCzas)
            {
                auto t2 = std::chrono::high_resolution_clock::now();
                auto t = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
                std::cout<<"Czas sortowania kulek: "<<t.count()<<"us"<<'\n';

                t1 = std::chrono::high_resolution_clock::now();
            }

            // sprawdzanie kolizji według posortowanej listy wielowątkowo
            uint16_t chunk = (size - 1) / liczbaWatkow;
            uint16_t r = (size - 1) % liczbaWatkow;
            uint16_t poczatek = 0;
            std::vector<std::thread> threads;
            for (uint16_t t = 0; t < liczbaWatkow - 1; t++)
            {
                uint16_t dodatkowaIteracja = t < r ? 1 : 0;
                uint16_t koniec = poczatek + chunk + dodatkowaIteracja;
                threads.emplace_back([&, poczatek, koniec]()
                {
                    for (uint16_t i = poczatek; i < koniec; i++)
                    {
                        for (uint16_t j = i+1; balls[i].px + balls[i].radius > balls[j].px - balls[j].radius and j < size; j++)
                        {
                            Simulation::Collision(balls[i], balls[j]);
                        }
                    }
                });
                poczatek = koniec;
            }
            for (uint16_t i = poczatek; i < size - 1; i++)
            {
                for (uint16_t j = i+1; balls[i].px + balls[i].radius > balls[j].px - balls[j].radius and j < size; j++)
                {
                    Simulation::Collision(balls[i], balls[j]);
                }
            }
            for (auto& th : threads) th.join();

            // sprawdzanie kolizji według posortowanej listy
            /*for (uint16_t i = 0; i < size - 1; i++)
            {
                for (uint16_t j = i+1; balls[i].px + balls[i].radius > balls[j].px - balls[j].radius and j < size; j++)
                {
                    Simulation::Collision(balls[i], balls[j]);
                }
            }*/

            // proste sprawdzanie kolizji bez żadnych optymalizacji wersja z iteratorem albo nie
            /*for (auto it = balls.begin(); it != std::prev(balls.end()); it++)
            {
                for (auto it2 = std::next(it); it2 != balls.end(); it2++)
                {
                    Simulation::Collision(*it, *it2);
                }
            }*/
            /*for (uint16_t i = 0; i < size - 1; i++)
            {
                for (uint16_t j = i+1; j < size; j++)
                {
                    Simulation::Collision(balls[i], balls[j]);
                }
            }*/

            if (mierzCzas)
            {
                auto t2 = std::chrono::high_resolution_clock::now();
                auto t = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
                std::cout<<"Czas kolizji kulek: "<<t.count()<<"us"<<'\n';
            }
        }
    }

    void Simulation::Collision(Ball& ball1, Ball& ball2)
    {
        float dx = ball1.px - ball2.px;
        float dy = ball1.py - ball2.py;
        float odleglosc = dx * dx + dy * dy;
        float sumR = ball1.radius + ball2.radius;
        if (odleglosc >= sumR * sumR) return;
        if (odleglosc == 0) return;
        odleglosc = std::sqrtf(odleglosc);

        float normalnyX = dx / odleglosc;
        if (odleglosc == 0)
            std::cout<<normalnyX<<'\n';
        float normalnyY = dy / odleglosc;
        if (odleglosc == 0)
            std::cout<<normalnyY<<'\n';
        float prostopadlyX = -normalnyY;
        float prostopadlyY = normalnyX;

        float predkoscWKolizje = ball1.vx * normalnyX + ball1.vy * normalnyY;
        float predkoscProstopadlaDoKolizji = ball1.vx * prostopadlyX + ball1.vy * prostopadlyY;
        float predkoscWKolizje2Kula = ball2.vx * normalnyX + ball2.vy * normalnyY;
        float predkoscProstopadlaDoKolizji2Kula = ball2.vx * prostopadlyX + ball2.vy * prostopadlyY;

        float sumaMas = ball1.m + ball2.m;
        float predkoscWKolizjePo = (predkoscWKolizje * (ball1.m - ball2.m) + 2 * ball2.m * predkoscWKolizje2Kula) / sumaMas;
        float predkoscWKolizjePo2Kula = (predkoscWKolizje2Kula * (ball2.m - ball1.m) + 2 * ball1.m * predkoscWKolizje) / sumaMas;

        ball1.vx = predkoscWKolizjePo * normalnyX + predkoscProstopadlaDoKolizji * prostopadlyX;
        ball1.vy = predkoscWKolizjePo * normalnyY + predkoscProstopadlaDoKolizji * prostopadlyY;
        ball2.vx = predkoscWKolizjePo2Kula * normalnyX + predkoscProstopadlaDoKolizji2Kula * prostopadlyX;
        ball2.vy = predkoscWKolizjePo2Kula * normalnyY + predkoscProstopadlaDoKolizji2Kula * prostopadlyY;

        float nachodzenie = sumR - odleglosc;
        ball1.px += normalnyX * nachodzenie * ball2.m / sumaMas;
        ball1.py += normalnyY * nachodzenie * ball2.m / sumaMas;
        ball2.px -= normalnyX * nachodzenie * ball1.m / sumaMas;
        ball2.py -= normalnyY * nachodzenie * ball1.m / sumaMas;
    }

    void Simulation::AddGas(uint16_t amount, float radius, float m, float left, float right, float up, float down, uint8_t r, uint8_t g, uint8_t b)
    {
        for (uint16_t i = 0; i < amount; i++)
        {
            float px = rand()%10001 / 10000.0f * (right - left) + left;
            float py = rand()%10001 / 10000.0f * (down - up) + up;
            AddBall(radius, m, px, py, 0.0f, 0.0f, r, g, b);
        }
    }

    void Simulation::AddGas(uint16_t amount, float radius, float m, uint8_t r, uint8_t g, uint8_t b)
    {
        for (uint16_t i = 0; i < amount; i++)
        {
            float px = rand()%10001 / 10000.0f * (maxX - minX) + minX;
            float py = rand()%10001 / 10000.0f * (maxY - minY) + minY;
            AddBall(radius, m, px, py, 0.0f, 0.0f, r, g, b);
        }
    }

    void Simulation::AddBall(float radius, float m, float px, float py, float vx, float vy, uint8_t r, uint8_t g, uint8_t b)
    {
        balls.push_back(Ball{radius, m, px, py, vx, vy, nextId, r, g, b});
        nextId++;
    }

    void Simulation::ClearAll()
    {
        balls.clear();
        nextId = 0;
    }

    void Simulation::CalcDeltaTime()
    {
        start = end;
        end = std::chrono::high_resolution_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        if (mierzCzas)
        {
            std::cout<<"Czas calej klatki: "<<elapsedTime.count()<<"us"<<'\n';
        }
        dt = elapsedTime.count() * 0.000001f * timeScale / (float)krokiFizyki;
    }
}