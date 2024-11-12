// VT Invaders
// Copyright (c) 2024 James Holderness
// Distributed under the MIT License

#include "engine.h"

#include "aliens.h"
#include "capabilities.h"
#include "missiles.h"
#include "options.h"
#include "os.h"
#include "screen.h"
#include "shields.h"
#include "status.h"
#include "turret.h"
#include "ufo.h"

#include <thread>

using namespace std::chrono_literals;

engine::engine(const capabilities& caps, const options& options)
    : _caps{caps}, _options{options}
{
}

bool engine::run()
{
    volatile auto exit_requested = false;
    volatile auto keyboard_shutdown = false;
    auto keyboard_thread = std::thread([&]() {
        while (!keyboard_shutdown && !exit_requested) {
            const auto ch = os::getch();
            if (ch == 32) {
                _fire_pressed = true;
            } else if (ch == 'C') {
                _right_pressed = true;
            } else if (ch == 'D') {
                _left_pressed = true;
            } else if (ch == 'q' || ch == 'Q' || ch == 3) {
                exit_requested = true;
            }
        }
    });

    screen screen{_caps, _options};
    status status{screen};
    shields shields{screen};
    aliens aliens{screen};
    missiles missiles{screen};
    turret turret{screen};
    laser laser{screen};
    ufo ufo{screen, laser};

    auto game_over = false;
    for (auto level = 0; !exit_requested && !game_over; level++) {
        _fire_pressed = false;
        _right_pressed = false;
        _left_pressed = false;

        screen.reset();
        status.reset();
        shields.reset();
        aliens.reset();
        missiles.reset();
        turret.reset();
        laser.reset();
        ufo.reset();

        using clock = std::chrono::high_resolution_clock;
        const auto frame_len = 1000ms / _options.fps;
        for (auto frame = 0; !exit_requested; frame++) {
            const auto frame_end = clock::now() + frame_len;

            if (aliens.init(frame, level)) {
                status.add_to_score(aliens.update(turret));
                if (aliens.landed()) turret.hit();
                if (aliens.remaining() == 0 && !turret.exploding()) {
                    screen.pause(30);
                    break;
                }

                if (aliens.remaining() < 8) ufo.disable();
                status.add_to_score(ufo.update(frame));

                shields.update();

                constexpr auto start_frame = aliens::count + 73;
                if (frame >= start_frame) {
                    if (frame % 3 == 0) {
                        if (aliens.can_fire() && missiles.can_fire() && !turret.exploding()) {
                            const auto [y, x] = aliens.fire();
                            missiles.fire(y, x);
                        }
                        missiles.update(frame, [&](const auto hit_id, const auto x) {
                            if (hit_id == turret::id)
                                turret.hit();
                            else if (hit_id == shields::id)
                                shields.hit(true, x);
                        });
                    }

                    if (turret.exploding()) {
                        if (turret.render_explosion()) {
                            if (status.lose_life(aliens.landed())) {
                                game_over = true;
                                break;
                            }
                            turret.reset();
                            turret.reveal();
                        }
                    } else {
                        if (frame == start_frame) {
                            turret.reveal();
                        } else if (_right_pressed) {
                            turret.move_right();
                            _right_pressed = false;
                        } else if (_left_pressed) {
                            turret.move_left();
                            _left_pressed = false;
                        }

                        if (_fire_pressed && !aliens.exploding()) {
                            laser.fire(turret.x());
                            _fire_pressed = false;
                        }
                    }

                    const auto hit_id = laser.update();
                    if (hit_id == shields::id)
                        shields.hit(false, laser.x());
                    else if (hit_id == ufo::id)
                        ufo.kill();
                    else if (hit_id >= 0 && hit_id < aliens::count)
                        aliens.kill(hit_id);
                }
            }

            screen.flush();

            // We're using a busy-wait loop here because the sleep_for
            // function isn't accurate enough for the delay we need.
            while (clock::now() < frame_end) {
            }
        }
    }

    keyboard_shutdown = true;
    keyboard_thread.join();
    return !exit_requested;
}
