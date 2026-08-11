# David's ESP8266 Deauther

<img src='https://deauther.com/img/logo.png' alt='Deauther Logo' width='200' />

**Scan for WiFi devices, block selected connections, create dozens of networks and confuse WiFi scanners.**

> A personal fork of [SpacehuhnTech/esp8266_deauther](https://github.com/SpacehuhnTech/esp8266_deauther) (v2), for testing my own networks.

## ⭐ Changes in this fork

Everything below is on top of the upstream v2 (`2.6.1`) firmware:

* **Rebranded** to *David's ESP8266 Deauther*, version **`3.0.0-dt`** (shown on the boot banner and in the web interface).
* **Selectable LED themes** — a new, persisted setting with 5 options:
  * `default` – original state colors (blue = scan, red = attack, green = idle)
  * `red`, `blue`, `purple` – a solid color for all active states
  * `party` – animated, alternating light-blue and green
  * Switch it from the **serial CLI** (`led party`, `led default`, …) or on-device via **Main Menu → LED THEME** on boards with an OLED. The choice survives reboots.
* **Custom default access point** — SSID `david`, password `david123`.
* **Fixed the web asset converter** (`utils/web_converter`) so it runs without the abandoned `anglerfish` dependency.
* **Added `build.sh`** — a one-command `arduino-cli` build/flash/monitor helper.

See [`serialcommands.md`](serialcommands.md) for the full command reference.

## New Documentation

Hi 👋  
Please visit [Deauther.com](https://deauther.com) for information about this project.  
Here are some quick links:

* [Buy](https://deauther.com/docs/buy)
* [Download](https://deauther.com/docs/download)
* [DIY Tutorial](https://deauther.com/docs/category/diy-tutorial)
* [Usage](https://deauther.com/docs/category/usage)
* [FAQ](https://deauther.com/docs/faq)

## Password

This fork's default access point is `david` with the password `david123`.
(Upstream defaults were `pwned` / `deauther`.)

## About this Project

This firmware allows you to easily perform a variety of actions to test 802.11 networks using an [ESP8266](https://www.espressif.com/en/products/socs/esp8266). It's also a great project for learning about WiFi, microcontrollers, Arduino, hacking and electronics/programming in general.  

The deauthentication attack is the main feature, which can be used to disconnect devices from their WiFi network.  
Although this denial-of-service attack is nothing new, a lot of devices are still vulnerable to it. Luckily this is slowly changing with more WiFi 6 enabled devices being used. But a lot of outdated WiFi devices remain in place, for example in cheap IoT hardware.
With an ESP8266 Deauther, you can easily test this attack on your 2.4GHz WiFi network/devices and see whether it's successful or not. And if it is, you know you should upgrade your network.

## Disclaimer

This project is a proof of concept for testing and educational purposes.  
Neither the ESP8266, nor its SDK was meant or built for such purposes. **Bugs can occur!**  

**Use it only against your own networks and devices!**  
Please check the legal regulations in your country before using it.  
We don't take any responsibility for what you do with this program.  