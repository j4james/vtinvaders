// VT Invaders
// Copyright (c) 2024 James Holderness
// Distributed under the MIT License

#pragma once

#include <array>
#include <tuple>

class screen;
class turret;

class aliens {
public:
    static constexpr int columns = 11;
    static constexpr int count = 5 * columns;
    static constexpr int explosion_id = 'E';

    aliens(screen& screen);
    void reset();
    bool init(const int frame, const int level);
    int update(const turret& turret);
    bool can_fire() const;
    std::pair<int, int> fire();
    void kill(const int id);
    bool exploding() const;
    bool landed() const;
    int remaining() const;

private:
    class instance {
    public:
        void init(const int id, const int level, screen& screen);
        bool update(const int y_delta, const int x_delta, screen& screen);
        void hide(screen& screen);
        void kill(screen& screen);
        int points() const;
        int y() const;
        int x() const;
        bool dead() const;

    private:
        void _render(screen& screen);

        int _id = 0;
        int _y = 0;
        int _x = 0;
        int _type = 0;
        bool _dead = false;
    };

    screen& _screen;
    int _y_delta = 0;
    int _x_delta = 1;
    bool _reverse = false;
    instance* _killed_instance = nullptr;
    int _killed_timer = 0;
    int _killed_count = 0;
    int _last_moved = -1;
    int _horizontal_offset = 0;
    int _best_shooter_column = 0;
    int _shooter_count = 0;
    int _shot_count = 0;
    bool _landed = false;
    std::array<instance, count> _aliens = {};
    std::array<instance*, columns> _shooters = {};
};
