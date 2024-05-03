//
// Created by Cameloah on 19.01.2022.
//

#include "Arduino.h"

#include "modules/user_interface.h"
#include "tools/loop_timer.h"

#include "wifi_handler.h"
#include "ram_log.h"
#include "webserial_monitor.h"
#include "version.h"
#include "github_update.h"

void ui_unknown() {
    DualSerial.print("\nUnknown command. One of the following Parameters is missing:\n");
}

void ui_info_help() {
    DualSerial.println("info                          - system information about the device");
}

String ui_info() {
    String fw_version;
    String info;

    // Construct the firmware version string
    fw_version += "\nFirmware version:   ";
    fw_version += String(FW_VERSION_MAJOR) + "." + String(FW_VERSION_MINOR) + "." + String(FW_VERSION_PATCH);
    fw_version += "\n";

    // Append additional system information to the info string.
    info += "Wifi mode:          "; info += wifi_handler_get_mode(); info += "\n";
    info += "Wifi connected to:  "; info += WiFi.isConnected() ? WiFi.SSID() : "not connected"; info += "\n";
    info += "IP-address:         "; info += WiFi.isConnected() ? WiFi.localIP().toString() : ""; info += "\n";
    info += "Uptime:             "; info += ram_log_time_str(millis()); info += "\n";

    DualSerial.print(fw_version + info);

    ram_log_print_log();

    return fw_version;
}

void ui_debug_help() {
    DualSerial.println("debug --reboot                        - reboot device\n"
                     "      --update --version [vX.X.X]     - search and upate to desired FW version");
}

void ui_debug() {
    char *sub_key = strtok(nullptr, " \n");

    if (sub_key == nullptr) {
        ui_unknown();
        ui_debug_help();
        }

    else if (!strcmp(sub_key, "--reboot")) {
        DualSerial.println("rebooting...");
        delay(1000);
        esp_restart();
    }

    else if (!strcmp(sub_key, "--update")) {
        sub_key = strtok(nullptr, " \n");

        if (sub_key == nullptr)
            strcpy(sub_key, "");

        else if (!strcmp(sub_key, "--version")) {

            sub_key = strtok(nullptr, " \n");

            github_update_firmwareUpdate(sub_key);
            Serial.println("Update failed.");
            return;
        }
    }

    else
        ui_debug_help();
}

void ui_help() {
    ui_info_help();
    ui_debug_help();
}

void ui_serial_comm_handler() {
    // listen for user input
    if (DualSerial.available())
        delay(50); // wait a bit for transfer of all serial data
    uint8_t rx_available_bytes = DualSerial.available();
    if (rx_available_bytes > 0) {
        // lets pause the loop time reporting for 1 iteration
        loop_timer_skip_flag = true;

        // import entire string until "\n"
        char rx_user_input[rx_available_bytes];
        DualSerial.readBytes(rx_user_input, rx_available_bytes);

        // extract first word as command key
        char *rx_command_key = strtok(rx_user_input, " \n");

        // catch exception where no token was sent
        if (rx_command_key == nullptr)
            return;

        else if (!strcmp(rx_command_key, "info"))
            ui_info();

        else if (!strcmp(rx_command_key, "debug"))
            ui_debug();

        else if (!strcmp(rx_command_key, "help"))
            ui_help();

        else {
            // unknown command
            DualSerial.println("\nUnknown command. Type 'help' for a list of availabe commands.");
        }

        // flush serial buffer
        DualSerial.readString();

        DualSerial << '\n';
    }
}