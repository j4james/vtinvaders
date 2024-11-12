// VT Invaders
// Copyright (c) 2024 James Holderness
// Distributed under the MIT License

#pragma once

class screen;

class turret {
public:
    static constexpr int id = 'T';
    static constexpr int row = 22;

    turret(screen& screen);
    void reset();
    void reveal();
    void move_left();
    void move_right();
    void hit();
    bool render_explosion();
    bool exploding() const;
    int x() const;

private:
    void _render();

    screen& _screen;
    int _x = 0;
    int _y = 0;
    bool _dead = false;
    int _explosion_frame = 0;
};

class laser {
public:
    laser(screen& screen);
    void reset();
    void fire(const int x);
    int update();
    int x() const;
    int shots_fired() const;

private:
    screen& _screen;

    int _x = 0;
    int _y = 0;
    int _phase = 0;
    bool _active = false;
    int _shots_fires = 0;
};
