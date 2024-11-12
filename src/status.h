// VT Invaders
// Copyright (c) 2024 James Holderness
// Distributed under the MIT License

#pragma once

class screen;

class status {
public:
    status(screen& screen);
    void reset();
    void add_to_score(const int points);
    bool lose_life(const bool all);

private:
    void _render_score();
    void _render_lives(const bool decreasing = false);
    void _render_game_over();

    screen& _screen;
    int _score = 0;
    int _lives = 3;
};
