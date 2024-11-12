// VT Invaders
// Copyright (c) 2024 James Holderness
// Distributed under the MIT License

#pragma once

class capabilities;
class options;

class engine {
public:
    static constexpr int width = 60;
    static constexpr int height = 24;

    engine(const capabilities& caps, const options& options);
    bool run();

private:
    const capabilities& _caps;
    const options& _options;

    volatile bool _fire_pressed = false;
    volatile bool _right_pressed = false;
    volatile bool _left_pressed = false;
};
