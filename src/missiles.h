// VT Invaders
// Copyright (c) 2024 James Holderness
// Distributed under the MIT License

#pragma once

#include <array>
#include <functional>

class screen;

class missiles {
public:
    static constexpr int id = 'M';
    using hit_function = std::function<void(int, int)>;

    missiles(screen& screen);
    void reset();
    void update(const int frame, const hit_function& on_hit);
    bool can_fire() const;
    void fire(const int y, const int x);

private:
    class instance {
    public:
        void reset();
        bool update(const hit_function& on_hit, screen& screen);
        bool fire(const int y, const int x, screen& screen);

    private:
        void _render(const char* sprite, screen& screen);

        int _y = 0;
        int _x = 0;
        int _phase = 0;
        bool _active = false;
    };

    screen& _screen;
    std::array<instance, 3> _missiles = {};
    int _active_count = 0;
    int _fire_frame = 0;
    bool _can_fire = false;
};
