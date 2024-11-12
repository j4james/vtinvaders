// VT Invaders
// Copyright (c) 2024 James Holderness
// Distributed under the MIT License

#include "status.h"

#include "engine.h"
#include "screen.h"

#include <string>

namespace {

    constexpr auto turret_sprite = "sz";

    constexpr int extra_life_score = 1500;

    constexpr int game_over_row = 2;
    constexpr int score_row = 24;

}  // namespace

status::status(screen& screen)
    : _screen{screen}
{
}

void status::reset()
{
    // MLTerm doesn't reset double-width lines correctly, so we need to
    // manually reset the GAME OVER line when restarting a level.
    _screen.single_width(game_over_row);
    _screen.double_width(score_row);
    _render_lives();
    _screen.pause(1);
    _screen.write(score_row, 19, "SCORE ", color::white);
    _render_score();
    _screen.pause(1);
    _screen.write(score_row - 1, 1, "", color::green);
    for (auto i = engine::width; i-- > 0;) {
        _screen.write('_');
        if (i % (engine::width / 3) == 0)
            _screen.pause(1);
    }
}

void status::add_to_score(const int points)
{
    if (points > 0) {
        _score += points;
        _render_score();
        if (_score >= extra_life_score && _score - points < extra_life_score) {
            _lives++;
            _render_lives();
        }
    }
}

bool status::lose_life(const bool all)
{
    _lives -= (all ? _lives : 1);
    _render_lives(true);
    if (_lives > 0) {
        _screen.pause(128);
        return false;
    } else {
        _render_game_over();
        return true;
    }
}

void status::_render_score()
{
    auto score_string = std::to_string(_score % 10000);
    score_string.insert(0, 4 - score_string.length(), '0');
    _screen.write(score_row, 25, score_string, color::white);
}

void status::_render_lives(const bool decreasing)
{
    _screen.write(score_row, 3, std::to_string(_lives), color::white);
    _screen.write(' ');
    if (decreasing) {
        if (_lives > 0)
            _screen.write(score_row, 3 + _lives * 2, "  ");
        else
            _screen.write(score_row, 5, "            ");
    } else {
        for (auto i = 1; i < _lives; i++)
            _screen.write(score_row, 3 + i * 2, turret_sprite, color::green);
    }
}

void status::_render_game_over()
{
    _screen.clear_line(game_over_row);
    _screen.double_width(game_over_row);
    _screen.write(game_over_row, 11, "", color::red);
    for (auto ch : "GAME OVER") {
        _screen.write(ch);
        _screen.pause(6);
    }
}
