// VT Invaders
// Copyright (c) 2024 James Holderness
// Distributed under the MIT License

#include "capabilities.h"
#include "coloring.h"
#include "engine.h"
#include "font.h"
#include "options.h"
#include "os.h"

#include <iostream>
#include <thread>

using namespace std::chrono_literals;

bool check_compatibility(const capabilities& caps, const options& options)
{
    if (!caps.has_soft_fonts && !options.yolo) {
        std::cout << "VT Invaders requires a VT320-compatible terminal or better.\n";
        std::cout << "Try 'vtinvaders --yolo' to bypass the compatibility checks.\n";
        return false;
    }
    if (caps.height < engine::height) {
        std::cout << "VT Invaders requires a minimum screen height of " << engine::height << ".\n";
        return false;
    }
    if (caps.width < engine::width) {
        std::cout << "VT Invaders requires a minimum screen width of " << engine::width << ".\n";
        return false;
    }
    return true;
}

void title_banner(const capabilities& caps)
{
    const auto y = (caps.height + 1) / 2;
    const auto x = (caps.width - 11 * 2 + 2) / 4 + 1;
    std::cout << "\033[" << y << ';' << x << "H";
    std::cout << "\033#6";
    std::cout << "VT INVADERS";
    std::cout.flush();
    std::this_thread::sleep_for(3s);
    // MLTerm doesn't reset double-width lines correctly, so we need to
    // manually reset the title banner line before starting the game.
    std::cout << "\033[2K\033#5";
}

int main(const int argc, const char* argv[])
{
    os os;

    options options(argc, argv);
    if (options.exit)
        return 1;

    capabilities caps;
    if (!check_compatibility(caps, options))
        return 1;

    // Set the window title.
    std::cout << "\033]21;VT Invaders\033\\";
    // Set default attributes.
    std::cout << "\033[m";
    // Clear the screen.
    std::cout << "\033[2J";
    // Save the modes and settings that we're going to change.
    const auto original_decscnm = caps.query_mode(5);
    const auto original_decawm = caps.query_mode(7);
    const auto original_decssdt = caps.query_setting("$~");
    // Hide the cursor.
    std::cout << "\033[?25l";
    // Disable line wrapping.
    std::cout << "\033[?7l";
    // Hide the status line.
    std::cout << "\033[0$~";
    // Load the soft font.
    const auto font = soft_font{caps};
    // Setup the color assignment and palette.
    const auto colors = coloring{caps, options};
    // Wait until the terminal is ready.
    caps.query_cursor_position();

    title_banner(caps);
    while (true) {
        auto game_engine = engine{caps, options};
        if (!game_engine.run()) break;
    }

    // Clear the window title.
    std::cout << "\033]21;\033\\";
    // Set default attributes.
    std::cout << "\033[m";
    // Clear the screen.
    std::cout << "\033[H\033[J";
    // Reset reverse screen attributes if not originally set.
    if (original_decscnm != true)
        std::cout << "\033[?5l";
    // Reapply line wrapping if not originally reset.
    if (original_decawm != false)
        std::cout << "\033[?7h";
    // Restore the original status display type.
    if (!original_decssdt.empty())
        std::cout << "\033[" << original_decssdt;
    // Show the cursor.
    std::cout << "\033[?25h";

    return 0;
}
