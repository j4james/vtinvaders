// VT Invaders
// Copyright (c) 2024 James Holderness
// Distributed under the MIT License

#include "turret.h"

#include "engine.h"
#include "screen.h"

#include <array>

namespace {

    constexpr auto left_boundary = 5;
    constexpr auto right_boundary = engine::width - 5;

    constexpr auto turret_sprite = "-^=";
    constexpr auto laser_sprites = "|!@";
    constexpr auto explosion_sprites = std::to_array({"BFP", "QHK"});

}  // namespace

turret::turret(screen& screen)
    : _screen{screen}
{
}

void turret::reset()
{
    _y = row;
    _x = left_boundary;
    _dead = false;
}

void turret::reveal()
{
    _render();
}

void turret::move_left()
{
    if (_x > left_boundary) {
        _x--;
        _render();
        _screen.write(_y, _x + 3, ' ');
    }
}

void turret::move_right()
{
    if (_x + 1 < right_boundary) {
        _screen.write(_y, _x, ' ');
        _x++;
        _render();
    }
}

void turret::hit()
{
    if (!_dead) {
        _dead = true;
        _explosion_frame = 0;
    }
}

bool turret::render_explosion()
{
    static constexpr auto explosion_frame_count = 55;
    if (_explosion_frame % 5 == 0 && _explosion_frame < explosion_frame_count) {
        const auto sprite_index = _explosion_frame / 5 % 2;
        _screen.write(_y, _x, explosion_sprites[sprite_index], color::green, id);
    } else if (_explosion_frame == explosion_frame_count) {
        _screen.write(_y, _x, "   ");
    }
    return _explosion_frame++ == explosion_frame_count;
}

bool turret::exploding() const
{
    return _dead;
}

int turret::x() const
{
    return _x + 1;
}

void turret::_render()
{
    _screen.write(_y, _x, turret_sprite, color::green, id);
}

laser::laser(screen& screen)
    : _screen{screen}
{
}

void laser::reset()
{
    _active = false;
}

void laser::fire(const int x)
{
    if (!_active) {
        _active = true;
        _x = x;
        _y = turret::row - 1;
        _phase = 0;
        _shots_fires++;
    }
}

int laser::update()
{
    if (!_active) return screen::empty;

    if (_y == 1 && _phase >= 1) {
        if (_phase == 1 || _phase == 2)
            _screen.write(_y, _x, laser_sprites[_phase], color::red);
        else if (_phase == 17)
            _screen.write(_y, _x, ' ');
        _active = (++_phase < 18);
        return screen::empty;
    }

    const auto hit_id = _screen.at(_y, _x);
    if (hit_id == screen::empty)
        _screen.write(_y, _x, laser_sprites[_phase], screen::color_for_row(_y));
    if (_phase == 0 && _screen.at(_y + 1, _x) == screen::empty)
        _screen.write(_y + 1, _x, ' ');

    _y -= _phase;
    _phase ^= 1;
    _active = (hit_id == screen::empty);
    return hit_id;
}

int laser::x() const
{
    return _x;
}

int laser::shots_fired() const
{
    return _shots_fires;
}
