// VT Invaders
// Copyright (c) 2024 James Holderness
// Distributed under the MIT License

#include "options.h"

#include <algorithm>
#include <iostream>
#include <string>

options::options(const int argc, const char* argv[])
{
    auto ignore_compatibility = false;
    for (auto i = 1; i < argc; i++) {
        const auto arg = std::string{argv[i]};
        if (arg == "--mono") {
            color = false;
        } else if (arg == "--yolo") {
            yolo = true;
        } else if (arg == "--speed" && i + 1 < argc) {
            try {
                fps = std::stoi(argv[++i]) * 10;
                fps = std::clamp(fps, 1, 100);
            } catch (std::exception) {
                // ignore invalid speed
            }
        } else if (arg == "--help") {
            std::cout << "Usage: vtinvaders [OPTION]...\n\n";
            std::cout << "  --mono        no coloring\n";
            std::cout << "  --speed N     set initial speed (1 to 10)\n";
            std::cout << "  --yolo        bypass compatibility checks\n";
            std::cout << "  --help        display this help and exit\n";
            exit = true;
        } else {
            std::cout << "VT Invaders: unrecognized option '" << arg << "'\n";
            std::cout << "Try 'vtinvaders --help' for more information.\n";
            exit = true;
        }
    }
}
