// VT Invaders
// Copyright (c) 2024 James Holderness
// Distributed under the MIT License

#pragma once

#include <array>
#include <string_view>
#include <vector>

class capabilities;
class options;

enum class color {
    any,
    white,
    red,
    green
};

class screen {
public:
    static constexpr int empty = -1;

    static color color_for_row(const int y);

    screen(const capabilities& caps, const options& options);
    void reset();
    void clear_line(const int y);
    void double_width(const int y);
    void single_width(const int y);
    void write(const char c);
    void write(const int y, const int x, const char c, const color color = color::any, const int id = empty);
    void write(const int y, const int x, const std::string_view s, const color color = color::any, const int id = empty);
    void pause(const int frames);
    void flush();
    int at(const int y, const int x) const;

private:
    void _sgr(const color color);
    void _cup(const int y, const int x);
    void _move_y_relative(const int diff_y);
    void _move_x_relative(const int diff_y);
    void _write();
    template <typename... Args>
    void _write(const int n, Args... args);
    template <typename... Args>
    void _write(const std::string_view s, Args... args);
    template <typename... Args>
    void _write(const char c, Args... args);
    static bool _is_blank(const char c);
    static int _offset(const int y, const int x);

    const bool _using_colors;
    const int _fps;
    const char* _ri;
    const char* _csi;
    int _y_indent;
    int _x_indent;
    int _last_y = -1;
    int _last_x = -1;
    color _last_color = color::any;
    std::vector<int> _ids = {};
    std::array<bool, 24> _wide = {};
    std::array<char, 256> _buffer = {};
    int _buffer_index = 0;
};
