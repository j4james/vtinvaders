// VT Invaders
// Copyright (c) 2024 James Holderness
// Distributed under the MIT License

#include "screen.h"

#include "capabilities.h"
#include "engine.h"
#include "options.h"
#include "shields.h"
#include "ufo.h"

#include <iostream>
#include <thread>

using namespace std::chrono_literals;

color screen::color_for_row(const int y)
{
    constexpr auto red_row = ufo::row;
    constexpr auto green_row = shields::row - 1;
    if (y <= red_row) return color::red;
    if (y >= green_row) return color::green;
    return color::white;
}

screen::screen(const capabilities& caps, const options& options)
    : _using_colors{options.color && caps.has_color}, _fps{options.fps}
{
    _ri = caps.has_8bit ? "\215" : "\033M";
    _csi = caps.has_8bit ? "\233" : "\033[";
    _y_indent = std::max((caps.height - engine::height) / 2, 0);
    _x_indent = std::max((caps.width - engine::width) / 4 * 2, 0);
    _ids.resize(engine::width * engine::height);
}

void screen::reset()
{
    std::fill(_ids.begin(), _ids.end(), empty);
    std::fill(_wide.begin(), _wide.end(), false);
    _last_y = -1;
    _last_x = -1;
    _last_color = color::any;
    _sgr(color::white);
    _write(_csi, _y_indent + engine::height - 1, ";999H");
    _write(_csi, "1J");
    pause(1);
}

void screen::clear_line(const int y)
{
    _cup(y, 1);
    _write(_csi, 'K');
}

void screen::double_width(const int y)
{
    _cup(y, 1);
    _write("\033#6");
    _wide[y - 1] = true;
}

void screen::single_width(const int y)
{
    _cup(y, 1);
    _write("\033#5");
    _wide[y - 1] = false;
}

void screen::write(const char c)
{
    _write(c);
    _last_x++;
}

void screen::write(const int y, const int x, const char c, const color color, const int id)
{
    const auto offset = _offset(y, x);
    _sgr(color);
    _cup(y, x);
    _write(c);
    _last_x++;
    _ids[offset] = _is_blank(c) ? empty : id;
}

void screen::write(const int y, const int x, const std::string_view s, const color color, const int id)
{
    auto offset = _offset(y, x);
    _sgr(color);
    _cup(y, x);
    for (auto c : s) {
        _write(c);
        _last_x++;
        _ids[offset++] = _is_blank(c) ? empty : id;
    }
}

void screen::pause(const int frames)
{
    flush();
    const auto frame_len = 1000ms / _fps;
    std::this_thread::sleep_for(frames * frame_len);
}

void screen::flush()
{
    if (_buffer_index) {
        std::cout.write(&_buffer[0], _buffer_index);
        std::cout.flush();
        _buffer_index = 0;
    }
}

int screen::at(const int y, const int x) const
{
    if (y < 1 || y > engine::height) return empty;
    if (x < 1 || x > engine::width) return empty;
    return _ids[_offset(y, x)];
}

void screen::_sgr(const color color)
{
    if (_using_colors && color != color::any && color != _last_color) {
        _last_color = color;
        switch (color) {
            case color::white:
                _write(_csi, 'm');
                break;
            case color::red:
                _write(_csi, "31m");
                break;
            case color::green:
                _write(_csi, "32m");
                break;
        }
    }
}

void screen::_cup(const int y, const int x)
{
    const auto wide = _wide[y - 1];
    const auto abs_y = y + _y_indent;
    const auto abs_x = x + (wide ? (_x_indent >> 1) : _x_indent);
    const auto unknown = _last_y == -1 || _last_x == -1;
    auto diff_y = unknown ? 9999 : abs_y - _last_y;
    auto diff_x = unknown ? 9999 : abs_x - _last_x;
    if (diff_y || diff_x) {
        if (abs(diff_y) > 2 && abs(diff_x) > 2)
            _write(_csi, abs_y, ';', abs_x, 'H');
        else {
            // When moving from a double-width row, we need to move vertically
            // first, because the horizontal movement may otherwise be clamped.
            // It's the other way around when not on a double-width row, since
            // if we move vertically first, the x oordinate may end up clamped
            // on the target row before we have a chance to reposition it.
            const auto last_y_index = _last_y - _y_indent - 1;
            const auto last_was_wide = last_y_index >= 0 ? _wide[last_y_index] : false;
            if (last_was_wide) {
                _move_y_relative(diff_y);
                _move_x_relative(diff_x);
            } else {
                _move_x_relative(diff_x);
                _move_y_relative(diff_y);
            }
        }
        _last_y = abs_y;
        _last_x = abs_x;
    }
}

void screen::_move_y_relative(const int diff_y)
{
    if (diff_y == -1)
        _write(_ri);
    else if (diff_y == -2)
        _write(_ri, _ri);
    else if (diff_y == 1)
        _write('\v');
    else if (diff_y == 2)
        _write("\v\v");
    else if (diff_y > 0)
        _write(_csi, diff_y, 'B');
    else if (diff_y < 0)
        _write(_csi, -diff_y, 'A');
}

void screen::_move_x_relative(const int diff_x)
{
    if (diff_x == -1)
        _write('\b');
    else if (diff_x == -2)
        _write("\b\b");
    else if (diff_x == 1)
        _write(_csi, 'C');
    else if (diff_x > 0)
        _write(_csi, diff_x, 'C');
    else if (diff_x < 0)
        _write(_csi, -diff_x, 'D');
}

void screen::_write()
{
}

template <typename... Args>
void screen::_write(const int n, Args... args)
{
    _write(std::to_string(n));
    _write(args...);
}

template <typename... Args>
void screen::_write(const std::string_view s, Args... args)
{
    for (auto c : s)
        _write(c);
    _write(args...);
}

template <typename... Args>
void screen::_write(const char c, Args... args)
{
    _buffer[_buffer_index++] = c;
    _write(args...);
}

bool screen::_is_blank(const char c)
{
    // The `y` is part of a destroyed shield sprite that is effectively
    // blank for the purposes of collision detection.
    return c == ' ' || c == 'y';
}

int screen::_offset(const int y, const int x)
{
    return (y - 1) * engine::width + (x - 1);
}
