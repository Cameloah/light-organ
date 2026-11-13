//
// Created by Cameloah on 19.01.2022.
//

#include "Arduino.h"

#include <WiFi.h>
#include <esp_timer.h>

#include "main.h"
#include "modules/user_interface.h"
#include "version.h"

static Stream& console() {
    return ProjectUtils.console();
}

void ui_info_help() {
    console().println("info                          - system information about the device");
}

String ui_info() {
    String fw_version = "\nFirmware version:   v" + String(FW_VERSION_MAJOR) + "." +
                        String(FW_VERSION_MINOR) + "." + String(FW_VERSION_PATCH) + "\n";

    // get reset reason
    String boot_msg = String(esp_reset_reason(), HEX);
    if (boot_msg.length() == 1)
        boot_msg = "0" + boot_msg;
    boot_msg = "0x" + boot_msg;

    const uint32_t uptime_s = static_cast<uint32_t>(esp_timer_get_time() / 1000000);
    char uptime[24];
    snprintf(uptime, sizeof(uptime), "%lud %02lu:%02lu:%02lu", (unsigned long)(uptime_s / 86400),
             (unsigned long)(uptime_s / 3600 % 24), (unsigned long)(uptime_s / 60 % 60),
             (unsigned long)(uptime_s % 60));

    String info =
        String("Wifi mode:          ") + pu::WifiManager::modeName() + "\n" +
        "Wifi connected to:  " + (WiFi.isConnected() ? WiFi.SSID() : "not connected") + "\n" +
        "IP-address:         " + (WiFi.isConnected() ? WiFi.localIP().toString() : "") + "\n" +
        "Uptime:             " + uptime + "\n" + "Boot message:       " + boot_msg + "\n\n";

    console().print(fw_version + info);
    ProjectUtils.log().printTo(console());

    return fw_version;
}

void ui_debug_help() {
    console().println("debug --reboot                        - reboot device\n"
                      "      --update                        - update to the latest FW version\n"
                      "      --update --version [vX.X.X]     - update to the desired FW version");
}

// Downloads and installs a release. An empty version means the latest one.
static void ui_update(const String& version) {
    pu::GithubUpdater& updater = ProjectUtils.updater();
    if (version.isEmpty() && updater.check() != pu::UpdateState::Available) {
        console().println("No newer version found: " + updater.status().message);
        return;
    }
    if (updater.install(version)) {
        console().println("Update installed, rebooting...");
        delay(1000);
        esp_restart();
    }
    console().println("Update failed: " + updater.status().message);
}

void ui_debug(const String& arguments) {
    if (arguments == "--reboot") {
        console().println("rebooting...");
        delay(1000);
        esp_restart();
    } else if (arguments == "--update") {
        ui_update("");
    } else if (arguments.startsWith("--update --version ")) {
        String version = arguments.substring(strlen("--update --version "));
        version.trim();
        ui_update(version);
    } else {
        console().print("\nUnknown command. One of the following Parameters is missing:\n");
        ui_debug_help();
    }
}

void ui_help() {
    ui_info_help();
    ui_debug_help();
}

void ui_serial_comm_handler() {
    // listen for user input, from USB serial or the web console
    if (console().available() <= 0)
        return;

    // import the line and split off the first word as command key
    String line = console().readStringUntil('\n');
    line.trim();
    if (line.isEmpty())
        return;
    const int space = line.indexOf(' ');
    const String command = space < 0 ? line : line.substring(0, space);
    String arguments = space < 0 ? String() : line.substring(space + 1);
    arguments.trim();

    if (command == "info")
        ui_info();
    else if (command == "debug")
        ui_debug(arguments);
    else if (command == "help")
        ui_help();
    else
        console().println("\nUnknown command. Type 'help' for a list of availabe commands.");

    console().println();
}
