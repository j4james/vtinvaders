// VT Invaders
// Copyright (c) 2024 James Holderness
// Distributed under the MIT License

#include "coloring.h"

#include "capabilities.h"
#include "options.h"

#include <iostream>

coloring::coloring(const capabilities& caps, const options& options)
    : _using_colors{options.color && caps.has_color}
{
    if (_using_colors) {
        // Save the current text color assignment.
        _color_assignment = caps.query_setting("1,|");
        // Make sure the text color assignment is white on black.
        if (!_color_assignment.empty())
            std::cout << "\033[1;7;0,|";
        // Save the current color table.
        _color_table = caps.query_color_table();
        // Set the desired color table entries for black, white, red, and blue.
        if (!_color_table.empty())
            std::cout << "\033P2$p0;2;0;0;0/7;2;100;100;100/1;2;96;11;29/2;2;11;95;6\033\\";
    }
}

coloring::~coloring()
{
    if (_using_colors) {
        // Restore the original color assignment.
        if (!_color_assignment.empty())
            std::cout << "\033[" << _color_assignment;
        // Restore the original color table.
        if (!_color_table.empty())
            std::cout << "\033P2$p" << _color_table << "\033\\";
    }
}
