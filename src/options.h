// VT Invaders
// Copyright (c) 2024 James Holderness
// Distributed under the MIT License

#pragma once

class options {
public:
    options(const int argc, const char* argv[]);

    bool color = true;
    bool yolo = false;
    bool exit = false;
    int fps = 50;
};
