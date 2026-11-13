# Light Organ

Firmware for the Magic Mushroom Light: mushroom lamps with LED strips that react to music,
show calm ambient animations, and can be switched from a web page or Home Assistant. Runs on an
ESP32 and uses [ProjectUtils-esp32](https://github.com/Cameloah/ProjectUtils-esp32) for WiFi
setup, web pages, settings, console and updates.

## Modes

| Mode | What the LEDs do |
|---|---|
| Off | All LEDs dark |
| Music | Bass flashes the large mushrooms red and leaves sparkles, the loudest band fills the white mushrooms from blue to yellow, treble flashes the red mushrooms |
| Ambient | Twinkling palettes on the large and red mushrooms, ocean waves on the white ones |
| DMX | Colors from a DMX controller (code present, currently switched off in `src/main.cpp`) |

Mode changes blend over smoothly. The chosen mode is stored and restored after a restart.

## Hardware

- ESP32 dev kit (AZ-Delivery DevKit v4)
- Spectrum analyzer shield with two MSGEQ7 chips: strobe on GPIO 32, reset on 33,
  left and right outputs on 34 and 35
- Five WS2812B strips:

| Group | Pin | LEDs |
|---|---|---|
| Large mushrooms, left (bass) | 23 | 31 |
| Large mushrooms, right (bass) | 22 | 29 |
| White mushrooms (mid) | 19 | 20 |
| Red mushrooms, left (treble) | 5 | 9 |
| Red mushrooms, right (treble) | 4 | 6 |

Pins and LED counts are in `include/led_config.h`.

## Using it

**First start:** the device opens a WiFi access point named after it. Connect, enter your WiFi
on the settings page that opens, and the device restarts and joins your network.

**Web page** at the device's address: the mode, the MQTT login and whether the broker is
connected. The menu leads to the library's pages for WiFi settings, firmware updates, the
console and device info.

**Home Assistant (MQTT):** the device connects to `homeassistant.local:1883` with the user and
password entered on its web page.

| Topic | Direction | Payload |
|---|---|---|
| `lichtorgel/light` | to the device | `music_on`, `ambient_on`, anything else switches off |
| `lichtorgel/light/music/state` | from the device | `music_on` or `off` |
| `lichtorgel/light/ambient/state` | from the device | `ambient_on` or `off` |

Commands that arrive within a second of connecting are ignored, so a retained command on the
broker does not override the mode stored on the device.

**Console** (USB serial at 115200 baud, or the web console):

| Command | Effect |
|---|---|
| `info` | Firmware version, WiFi, uptime, reset reason and the log |
| `debug --reboot` | Restart |
| `debug --update` | Install the latest GitHub release |
| `debug --update --version v1.2.0` | Install a specific release |

## Building and flashing

```sh
pio run                 # build
pio run -t upload       # flash over WiFi to the address in platformio.ini
pio device monitor      # serial console
```

For the first flash over USB, comment out `upload_protocol` and `upload_port` in
`platformio.ini`.

## Releasing

1. Raise the version in `include/version.h`.
2. Build, then publish a GitHub release tagged `vX.Y.Z` with
   `.pio/build/az-delivery-devkit-v4/firmware.bin` attached as `light_organ_vX.Y.Z.bin`.

Devices then find it with "Check now" on their update page or `debug --update`. CI also keeps
the built `firmware.bin` of every run as a download.

## Development

Formatting is enforced by clang-format through pre-commit. On every push GitHub Actions checks
formatting, builds the firmware, and runs cppcheck and clang-tidy (the same bug-pattern checks as
ProjectUtils-esp32, without its naming rules).

```sh
uv tool install pre-commit   # or: pipx install pre-commit
pre-commit install
git config blame.ignoreRevsFile .git-blame-ignore-revs   # skip the formatting commit in blame

pio check --fail-on-defect=low      # cppcheck
uv tool install clang-tidy==23.1.4  # the version CI uses
python3 scripts/run_clang_tidy.py   # clang-tidy; apply its suggestions by hand
```
