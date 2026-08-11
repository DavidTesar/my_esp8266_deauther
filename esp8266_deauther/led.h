/* This software is licensed under the MIT License: https://github.com/spacehuhntech/esp8266_deauther */

#pragma once

#include <cstdint>

enum LED_MODE {
    OFF,
    SCAN,
    ATTACK,
    IDLE
};

class String;

namespace led {
    void setup();
    void update();
    void setMode(LED_MODE new_mode, bool force = false);
    void setColor(uint8_t r, uint8_t g, uint8_t b);

    // Re-apply the current mode's color (e.g. after a theme change).
    void refresh();

    // Parse a theme name ("default"/"red"/"blue"/"purple"/"party").
    // Returns the matching led_theme_t as int, or -1 if unknown.
    int themeFromName(const String& name);
}