// VT Invaders
// Copyright (c) 2024 James Holderness
// Distributed under the MIT License

#include "missiles.h"

#include "engine.h"
#include "screen.h"

namespace {

    constexpr auto missile_sprites = std::to_array({R"(|")", R"( $)", R"( &)"});
    constexpr auto explosion_sprites = std::to_array({R"(|Y)", R"( ?)"});
    constexpr auto ground_sprites = std::to_array({R"( .)", R"( ,)"});

}  // namespace

missiles::missiles(screen& screen)
    : _screen{screen}
{
}

void missiles::reset()
{
    _active_count = 0;
    _fire_frame = 174;
    _can_fire = false;
    for (auto& missile : _missiles)
        missile.reset();
}

void missiles::update(const int frame, const hit_function& on_hit)
{
    for (auto& missile : _missiles) {
        if (missile.update(on_hit, _screen))
            if (--_active_count == 0)
                _fire_frame = frame + 12;
    }
    // For the first 2000 frames, only 1 missile may be fired at a time, and
    // thereafter we can have up to 3 simultaneously. A new missile may be
    // fired 12 frames after the last one terminated (see above), or 50 frames
    // after the last one was launched (see the fire method below).
    const auto max_active = (frame < 2000 ? 1 : 3);
    _can_fire = (frame >= _fire_frame && _active_count < max_active);
    _fire_frame = std::max(_fire_frame, frame);
}

bool missiles::can_fire() const
{
    return _can_fire;
}

void missiles::fire(const int y, const int x)
{
    for (auto& missile : _missiles)
        if (missile.fire(y, x, _screen)) {
            _active_count++;
            _fire_frame += 50;
            break;
        }
}

void missiles::instance::reset()
{
    _active = false;
}

bool missiles::instance::update(const hit_function& on_hit, screen& screen)
{
    if (!_active) return false;

    if (_y == engine::height - 2) {
        if (_phase == 1)
            _render(explosion_sprites[0], screen);
        else if (_phase == 2)
            _render(explosion_sprites[1], screen);
        else if (_phase == 5)
            _render(ground_sprites[_x % 2], screen);
        _active = (++_phase < 6);
        return !_active;
    }

    const auto hit_id = screen.at(_y + 1, _x);
    if (hit_id != screen::empty && hit_id != missiles::id) {
        screen.write(_y, _x, ' ');
        on_hit(hit_id, _x);
        _active = false;
        return true;
    }

    _phase ^= 1;
    if (_phase == 0)
        _render(missile_sprites[0], screen);
    else if (_y % 2 == 0)
        _render(missile_sprites[1], screen);
    else
        _render(missile_sprites[2], screen);
    _y += _phase;
    return false;
}

bool missiles::instance::fire(const int y, const int x, screen& screen)
{
    if (_active) return false;
    _y = y;
    _x = x;
    _phase = 0;
    _active = true;
    return true;
}

void missiles::instance::_render(const char* sprite, screen& screen)
{
    if (screen.at(_y, _x) == missiles::id)
        screen.write(_y, _x, sprite[0], screen::color_for_row(_y), missiles::id);
    screen.write(_y + 1, _x, sprite[1], screen::color_for_row(_y + 1), missiles::id);
}
