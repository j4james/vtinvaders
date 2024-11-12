// VT Invaders
// Copyright (c) 2024 James Holderness
// Distributed under the MIT License

#include "aliens.h"

#include "engine.h"
#include "screen.h"
#include "turret.h"

#include <algorithm>

namespace {

    constexpr auto alien_sprites_1 = std::to_array({"[X]", "(U)", ":#;"});
    constexpr auto alien_sprites_2 = std::to_array({"{x}", "<u>", "i+j"});
    constexpr auto landed_sprite = alien_sprites_2[2];
    constexpr auto explosion_sprite = "'*`";

    constexpr auto shoot_order = std::to_array({

        0, 1, 11, 1, 0, 7, 0, 1, 6, 3, 0, 1, 1, 0, 1, 1, 0, 4, 11,
        9, 2, 0, 11, 0, 1, 8, 2, 0, 6, 0, 3, 11, 4, 0, 1, 7, 0, 1,
        0, 11, 0, 9, 0, 2, 10, 11, 1, 0, 8, 0, 1, 6, 3, 0, 7, 0, 1,
        1, 0, 1, 1, 0, 1, 11, 9, 2, 0, 4, 0, 11, 0, 9, 0, 1, 0, 5

    });

}  // namespace

aliens::aliens(screen& screen)
    : _screen{screen}
{
}

void aliens::reset()
{
    _y_delta = 0;
    _x_delta = 1;
    _reverse = false;
    _killed_instance = nullptr;
    _killed_timer = 0;
    _killed_count = 0;
    _last_moved = -1;
    _horizontal_offset = 10;
    _best_shooter_column = 0;
    _shooter_count = 0;
    _shot_count = 0;
    _landed = false;
    std::fill(_shooters.begin(), _shooters.end(), nullptr);
}

bool aliens::init(const int frame, const int level)
{
    if (frame < _aliens.size()) {
        _aliens[frame].init(frame, level, _screen);
        if (frame < _shooters.size()) {
            _shooters[frame] = &_aliens[frame];
            _shooter_count++;
        }
        return false;
    }
    return true;
}

int aliens::update(const turret& turret)
{
    const auto just_killed = (_killed_timer == 16);
    if (_killed_timer > 0) {
        if (--_killed_timer == 0) {
            _killed_instance->hide(_screen);
            _killed_count++;
        }
    } else if (!_landed && !turret.exploding()) {
        auto current = _last_moved;
        do {
            current = (current + 1) % _aliens.size();
        } while (_aliens[current].dead());

        // If the current alien is less than the last one we moved, that means
        // we're starting a new cycle, and may need to reverse direction. We
        // also take this opportunity to update the horizontal offset of the
        // group, which is used to calculate which column is best positioned
        // to fire at the laser turret.
        if (current <= _last_moved) {
            _horizontal_offset = _aliens[current].x() - current * 4;
            _y_delta = 0;
            if (_reverse) {
                _x_delta = -_x_delta;
                _y_delta = 1;
            }
            _reverse = false;
        }

        // If the current alien is already on the turret row before we've even
        // moved it, it's safe to conclude that the aliens have landed.
        // Otherwise we'll only trigger a landing when moving an alien onto the
        // turret row while also being right of the turret position.
        if (_aliens[current].y() == turret::row)
            _landed = true;
        else {
            if (_aliens[current].update(_y_delta, _x_delta, _screen))
                _reverse = true;
            if (_aliens[current].y() == turret::row && _aliens[current].x() >= turret.x())
                _landed = true;
        }

        _last_moved = current;
    }
    _best_shooter_column = std::clamp((turret.x() - _horizontal_offset) / 4, 0, columns - 1);
    return just_killed ? _killed_instance->points() : 0;
}

bool aliens::can_fire() const
{
    return _shooter_count > 0;
}

std::pair<int, int> aliens::fire()
{
    // The column chosen to shoot follows a fixed pattern. It will sometimes be
    // a specfic column, and sometimes whichever column is best positioned to
    // hit the turret. If the chosen column is no longer available, we try again
    // with the next position in the series.
    auto column = 0;
    do {
        column = shoot_order[_shot_count] - 1;
        _shot_count = (_shot_count + 1) % shoot_order.size();
        if (column < 0) {
            column = _best_shooter_column;
            if (column >= _shooters.size()) column = -1;
        }
    } while (column < 0 || _shooters[column] == nullptr);

    const auto shooter = *_shooters[column];
    return std::make_pair(shooter.y() + 1, shooter.x() + 1);
}

void aliens::kill(const int id)
{
    _killed_timer = 16;
    _killed_instance = &_aliens[id];
    _killed_instance->kill(_screen);
    const auto column = id % columns;
    if (_shooters[column] == _killed_instance) {
        auto id_above = id + columns;
        while (id_above < _aliens.size() && _aliens[id_above].dead())
            id_above += columns;
        _shooters[column] = (id_above < _aliens.size() ? &_aliens[id_above] : nullptr);
        if (!_shooters[column]) _shooter_count--;
    }
}

bool aliens::exploding() const
{
    return _killed_timer > 0;
}

bool aliens::landed() const
{
    return _landed;
}

int aliens::remaining() const
{
    return count - _killed_count;
}

void aliens::instance::init(const int id, const int level, screen& screen)
{
    constexpr auto y_offsets = std::to_array({11, 14, 16, 17});
    const auto y_offset = y_offsets[std::min(level, 3)];
    const auto x_offset = 10;
    const auto row = id / 11;
    const auto col = id % 11;
    _y = y_offset - row * 2;
    _x = x_offset + col * 4;
    _id = id;
    _type = row / 2;
    _dead = false;
    if (col != 0)
        screen.write(_y, _x - 1, ' ');
    _render(screen);
}

bool aliens::instance::update(const int y_delta, const int x_delta, screen& screen)
{
    if (y_delta) {
        screen.write(_y, _x, "   ");
        _y += y_delta;
        _x += x_delta;
        _render(screen);
    } else if (x_delta > 0) {
        screen.write(_y, _x, ' ');
        _x++;
        _render(screen);
    } else if (x_delta < 0) {
        _x--;
        _render(screen);
        screen.write(_y, _x + 3, ' ');
    }
    // We return true to indicate when we've reached one of the boundaries,
    // and the group will then need to reverse direction on the next cycle.
    return _x <= 1 || _x + 2 >= engine::width;
}

void aliens::instance::hide(screen& screen)
{
    screen.write(_y, _x, "   ");
}

void aliens::instance::kill(screen& screen)
{
    _dead = true;
    _render(screen);
}

int aliens::instance::points() const
{
    return (_type + 1) * 10;
}

int aliens::instance::y() const
{
    return _y;
}

int aliens::instance::x() const
{
    return _x;
}


bool aliens::instance::dead() const
{
    return _dead;
}

void aliens::instance::_render(screen& screen)
{
    const auto color = screen::color_for_row(_y);
    if (_dead)
        screen.write(_y, _x, explosion_sprite, color, explosion_id);
    else if (_y == turret::row)
        screen.write(_y, _x, landed_sprite, color);
    else if (_x % 2 == 0)
        screen.write(_y, _x, alien_sprites_1[_type], color, _id);
    else
        screen.write(_y, _x, alien_sprites_2[_type], color, _id);
}
