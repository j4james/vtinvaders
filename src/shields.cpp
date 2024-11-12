// VT Invaders
// Copyright (c) 2024 James Holderness
// Distributed under the MIT License

#include "shields.h"

#include "screen.h"

#include <algorithm>

namespace {

    constexpr auto top_shield_sprite = "d%%b";
    constexpr auto top_damage_sprites = std::to_array({
        "dd_d___ell_lnn  ",
        "%%_%___krr_rtt  ",
        "%%_%___krr_rtt  ",
        "bb_b___fmm_mnn  ",
    });
    constexpr auto bottom_shield_sprite = "WpqW";
    constexpr auto bottom_damage_sprites = std::to_array({
        "Wa_c___yWa_cWaoy",
        "pg_h___ pg_hpgv ",
        "qg_h___ qg_hqgw ",
        "Wa_c___yWa_cWaoy",
    });

}  // namespace

shields::shields(screen& screen)
    : _screen{screen}
{
}

void shields::reset()
{
    for (auto n = 0; n < _shields.size(); n++) {
        _shields[n].reset(n, _screen);
        _screen.pause(1);
    }
}

void shields::update()
{
    for (auto& shield : _shields)
        shield.update(_screen);
}

void shields::hit(const bool from_above, const int x)
{
    for (auto& shield : _shields)
        shield.hit(from_above, x, _screen);
}

void shields::instance::reset(const int n, screen& screen)
{
    _y = shields::row;
    _x = 11 + n * 12;
    std::fill(_damage.begin(), _damage.end(), 0);
    screen.write(_y, _x, top_shield_sprite, color::green, shields::id);
    screen.write(_y + 1, _x, bottom_shield_sprite, color::green, shields::id);
}

void shields::instance::update(screen& screen)
{
    for (auto column = 0; column < 4; column++) {
        const auto x = _x + column;
        auto& damage = _damage[column];
        if ((~damage & 0b1100) && screen.at(_y, x) != shields::id)
            damage |= 0b1100;
        if ((~damage & 0b0011) && screen.at(_y + 1, x) != shields::id)
            damage |= 0b0011;
    }
}

void shields::instance::hit(const bool from_above, const int x, screen& screen)
{
    if (x >= _x && x < _x + 4) {
        const auto column = x - _x;
        auto& damage = _damage[column];

        const auto previous_top_sprite = top_damage_sprites[column][damage];
        const auto previous_bottom_sprite = bottom_damage_sprites[column][damage];

        if (from_above)
            damage = (((damage >> 1) + 0b1000) & 0b1111) | (damage & 0b0111);
        else
            damage = (((damage << 1) + 0b0001) & 0b1111) | (damage & 0b1110);

        const auto top_sprite = top_damage_sprites[column][damage];
        const auto bottom_sprite = bottom_damage_sprites[column][damage];

        if (screen.at(_y, x) == shields::id && top_sprite != previous_top_sprite)
            screen.write(_y, x, top_sprite, color::green, shields::id);

        if (screen.at(_y + 1, x) == shields::id && bottom_sprite != previous_bottom_sprite)
            screen.write(_y + 1, x, bottom_sprite, color::green, shields::id);
    }
}
