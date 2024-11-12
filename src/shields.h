// VT Invaders
// Copyright (c) 2024 James Holderness
// Distributed under the MIT License

#pragma once

#include <array>

class screen;

class shields {
public:
    static constexpr int id = 'S';
    static constexpr int row = 19;

    shields(screen& screen);
    void reset();
    void update();
    void hit(const bool from_above, const int x);

private:
    class instance {
    public:
        void reset(const int n, screen& screen);
        void update(screen& screen);
        void hit(const bool from_above, const int x, screen& screen);

    private:
        int _y = 0;
        int _x = 0;
        std::array<int, 4> _damage = {};
    };

    screen& _screen;
    std::array<instance, 4> _shields = {};
};
