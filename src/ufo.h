// VT Invaders
// Copyright (c) 2024 James Holderness
// Distributed under the MIT License

#pragma once

class laser;
class screen;

class ufo {
public:
    static constexpr int id = 'U';
    static constexpr int row = 2;

    ufo(screen& screen, const laser& laser);
    void reset();
    int update(const int frame);
    void disable();
    void kill();

private:
    screen& _screen;
    const laser& _laser;
    int _x = 0;
    int _y = 0;
    int _x_delta = 1;
    bool _active = false;
    bool _dead = false;
    bool _disabled = false;
    int _death_frame = 0;
    int _points_earned = 0;
};
