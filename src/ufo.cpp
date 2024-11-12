// VT Invaders
// Copyright (c) 2024 James Holderness
// Distributed under the MIT License

#include "ufo.h"

#include "engine.h"
#include "screen.h"
#include "turret.h"

#include <array>
#include <string>

namespace {

    constexpr auto ufo_sprite = R"(/JL\)";
    constexpr auto explosion_sprite = R"('BP`)";

    constexpr auto left_boundary = 4;
    constexpr auto right_boundary = engine::width - 6;

    constexpr auto possible_points = std::to_array({100, 50, 50, 100, 150, 100, 100, 50, 300, 100, 100, 100, 50, 150, 100});

}  // namespace

ufo::ufo(screen& screen, const laser& laser)
    : _screen{screen}, _laser{laser}
{
}

void ufo::reset()
{
    _active = false;
    _dead = false;
    _disabled = false;
}

int ufo::update(const int frame)
{
    // First UFO appears around 35 seconds, then every 25 seconds thereafter.
    constexpr auto first_frame = 35 * 60;
    constexpr auto interval = 25 * 60;
    if (_dead) {
        if (_active) {
            // The score is rendered as double width, so a 3 digit value is
            // actually 6 columns, and a 2 digit value is 4 columns. Since the
            // UFO itself is 4 columns, a 3 digit score will align best when
            // the UFO is in an odd column, and a 2 digit score will look best
            // with an even column. If we aren't on an ideal column, we move
            // one additional step in the direction we were going.
            if ((_points_earned >= 100) != (_x % 2 == 0)) _x += _x_delta;
            _screen.clear_line(_y);
            _screen.write(_y, _x, explosion_sprite, color::red);
            _death_frame = frame;
            _active = false;
        } else if (frame == _death_frame + 21) {
            _screen.clear_line(_y);
            _screen.double_width(_y);
            _screen.write(_y, (_x + 1) / 2, std::to_string(_points_earned), color::red);
            return _points_earned;
        } else if (frame == _death_frame + 93) {
            _screen.clear_line(_y);
            _screen.single_width(_y);
            _dead = false;
        }
    } else if (_active) {
        // The UFO should really only move once every 6 frames, but that
        // feels a bit jerky, so we've made it slightly faster.
        if (frame % 5 == 0) {
            _x += _x_delta;
            if (_x < left_boundary || _x > right_boundary) {
                _active = false;
                _screen.write(_y, _x - _x_delta, "    ");
            } else if (_x_delta < 0) {
                _screen.write(_y, _x, ufo_sprite, color::red, id);
                _screen.write(_y, _x + 4, ' ');
            } else {
                _screen.write(_y, _x - 1, ' ');
                _screen.write(_y, _x, ufo_sprite, color::red, id);
            }
        }
    } else if (frame >= first_frame && (frame - first_frame) % interval == 0 && !_disabled) {
        // The direction of movement depends on the shots fired so far.
        const auto left_to_right = (_laser.shots_fired() % 2 == 0);
        _active = true;
        _dead = false;
        _y = ufo::row;
        _x = left_to_right ? left_boundary : right_boundary;
        _x_delta = left_to_right ? 1 : -1;
        _screen.write(_y, _x, ufo_sprite, color::red, id);
    }
    return 0;
}

void ufo::disable()
{
    _disabled = true;
}

void ufo::kill()
{
    if (_active && !_dead) {
        // The points earned depends on the number of shots fired so far.
        _points_earned = possible_points[_laser.shots_fired() % possible_points.size()];
        _dead = true;
    }
}
