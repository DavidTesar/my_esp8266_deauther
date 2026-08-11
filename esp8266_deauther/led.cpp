/* This software is licensed under the MIT License: https://github.com/spacehuhntech/esp8266_deauther */

#include "led.h"

#include "A_config.h" // Config for LEDs
#include <Arduino.h>  // digitalWrite, analogWrite, pinMode
#include "language.h" // Strings used in printColor and tempDisable
#include "settings.h" // used in update()
#include "Attack.h"   // used in update()
#include "Scan.h"     // used in update()

// Inlcude libraries for Neopixel or LED_MY92xx if used
#if defined(LED_NEOPIXEL)
#include "src/Adafruit_NeoPixel-1.7.0/Adafruit_NeoPixel.h"
#elif defined(LED_MY92)
#include "src/my92xx-3.0.3/my92xx.h"
#elif defined(LED_DOTSTAR)
#include "src/Adafruit_DotStar-1.1.4/Adafruit_DotStar.h"
#endif // if defined(LED_NEOPIXEL)

extern Attack attack;
extern Scan   scan;

// ===== THEME PALETTE ===== //
#ifndef LED_PARTY_INTERVAL
  #define LED_PARTY_INTERVAL 250 // ms between party color swaps
#endif // ifndef LED_PARTY_INTERVAL
#define LED_THEME_PURPLE_COLOR 160, 0, 255
#define LED_PARTY_A 0, 180, 255 // light blue
#define LED_PARTY_B 0, 255, 60  // green

namespace led {
    // ===== PRIVATE ===== //
    LED_MODE mode = OFF;

    // Party-theme animation state
    uint32_t party_last_swap = 0;
    bool     party_state     = false;

#if defined(LED_NEOPIXEL_RGB)
    Adafruit_NeoPixel strip { LED_NUM, LED_NEOPIXEL_PIN, NEO_RGB + NEO_KHZ800 };
#elif defined(LED_NEOPIXEL_GRB)
    Adafruit_NeoPixel strip { LED_NUM, LED_NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800 };
#elif defined(LED_MY92)
    my92xx myled { LED_MY92_MODEL, LED_NUM, LED_MY92_DATA, LED_MY92_CLK, MY92XX_COMMAND_DEFAULT };
#elif defined(LED_DOTSTAR)
    Adafruit_DotStar strip { LED_NUM, LED_DOTSTAR_DATA, LED_DOTSTAR_CLK, DOTSTAR_BGR };
#endif // if defined(LED_NEOPIXEL_RGB)


    void setColor(uint8_t r, uint8_t g, uint8_t b) {
#if defined(LED_DIGITAL)
        if (LED_ANODE) {
            if (LED_PIN_R < 255) digitalWrite(LED_PIN_R, r > 0);
            if (LED_PIN_G < 255) digitalWrite(LED_PIN_G, g > 0);
            if (LED_PIN_B < 255) digitalWrite(LED_PIN_B, b > 0);
        } else {
            if (LED_PIN_R < 255) digitalWrite(LED_PIN_R, r == 0);
            if (LED_PIN_G < 255) digitalWrite(LED_PIN_G, g == 0);
            if (LED_PIN_B < 255) digitalWrite(LED_PIN_B, b == 0);
        }
#elif defined(LED_RGB)
        uint8_t brightness = settings::getLEDSettings().brightness; // 0-100 %
        if (r > 0) r = r * brightness / 100;
        if (g > 0) g = g * brightness / 100;
        if (b > 0) b = b * brightness / 100;

        if (LED_ANODE) {
            r = 255 - r;
            g = 255 - g;
            b = 255 - b;
        }

        analogWrite(LED_PIN_R, r);
        analogWrite(LED_PIN_G, g);
        analogWrite(LED_PIN_B, b);
#elif defined(LED_NEOPIXEL) || defined(LED_DOTSTAR)
        strip.setBrightness(settings::getLEDSettings().brightness * 255 / 100);

        for (size_t i = 0; i < strip.numPixels(); i++) {
            strip.setPixelColor(i, r, g, b);
        }

        strip.show();
#elif defined(LED_MY9291)
        myled.setChannel(LED_MY92_CH_R, r);
        myled.setChannel(LED_MY92_CH_G, g);
        myled.setChannel(LED_MY92_CH_B, b);
        myled.setChannel(LED_MY92_CH_BRIGHTNESS, settings::getLEDSettings().brightness * 255 / 100);
        myled.setState(true);
        myled.update();
#endif // if defined(LED_DIGITAL)
    }

    // Apply the color for the given mode, honoring the selected theme.
    // OFF always means truly off, regardless of the theme.
    void applyTheme(LED_MODE m) {
        if (m == OFF) {
            setColor(LED_MODE_OFF);
            return;
        }

        switch (settings::getLEDSettings().theme) {
            case LED_THEME_RED:
                setColor(255, 0, 0);
                break;
            case LED_THEME_BLUE:
                setColor(0, 0, 255);
                break;
            case LED_THEME_PURPLE:
                setColor(LED_THEME_PURPLE_COLOR);
                break;
            case LED_THEME_PARTY:
                if (party_state) setColor(LED_PARTY_A);
                else setColor(LED_PARTY_B);
                break;
            case LED_THEME_DEFAULT:
            default:
                switch (m) {
                    case SCAN:   setColor(LED_MODE_SCAN);   break;
                    case ATTACK: setColor(LED_MODE_ATTACK); break;
                    case IDLE:   setColor(LED_MODE_IDLE);   break;
                    default:     setColor(LED_MODE_OFF);    break;
                }
                break;
        }
    }

    // ===== PUBLIC ===== //
    void setup() {
        analogWriteRange(0xff);

#if defined(LED_DIGITAL) || defined(LED_RGB)
        if (LED_PIN_R < 255) pinMode(LED_PIN_R, OUTPUT);
        if (LED_PIN_G < 255) pinMode(LED_PIN_G, OUTPUT);
        if (LED_PIN_B < 255) pinMode(LED_PIN_B, OUTPUT);
#elif defined(LED_NEOPIXEL) || defined(LED_DOTSTAR)
        strip.begin();
        strip.setBrightness(LED_MODE_BRIGHTNESS);
        strip.show();
#elif defined(LED_MY9291)
        myled.setChannel(LED_MY92_CH_R, 0);
        myled.setChannel(LED_MY92_CH_G, 0);
        myled.setChannel(LED_MY92_CH_B, 0);
        myled.setChannel(LED_MY92_CH_BRIGHTNESS, LED_MODE_BRIGHTNESS);
        myled.setState(true);
        myled.update();
#endif // if defined(LED_DIGITAL) || defined(LED_RGB)
    }

    void update() {
        if (!settings::getLEDSettings().enabled) {
            setMode(OFF);
        } else if (scan.isScanning() && (scan.deauths < settings::getSnifferSettings().min_deauth_frames)) {
            setMode(SCAN);
        } else if (attack.isRunning()) {
            setMode(ATTACK);
        } else {
            setMode(IDLE);
        }

        // Animate the party theme independently of mode changes.
        if ((settings::getLEDSettings().theme == LED_THEME_PARTY) &&
            settings::getLEDSettings().enabled && (mode != OFF)) {
            uint32_t now = millis();

            if (now - party_last_swap >= LED_PARTY_INTERVAL) {
                party_last_swap = now;
                party_state     = !party_state;
                applyTheme(mode);
            }
        }
    }

    void setMode(LED_MODE new_mode, bool force) {
        if ((new_mode != mode) || force) {
            mode = new_mode;
            applyTheme(mode);
        }
    }

    void refresh() {
        applyTheme(mode);
    }

    int themeFromName(const String& name) {
        if (name.equalsIgnoreCase("default")) return LED_THEME_DEFAULT;
        if (name.equalsIgnoreCase("red")) return LED_THEME_RED;
        if (name.equalsIgnoreCase("blue")) return LED_THEME_BLUE;
        if (name.equalsIgnoreCase("purple")) return LED_THEME_PURPLE;
        if (name.equalsIgnoreCase("party")) return LED_THEME_PARTY;
        return -1;
    }
}
