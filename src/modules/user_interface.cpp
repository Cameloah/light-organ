//
// Created by Cameloah on 19.01.2022.
//

#include "Arduino.h"

#include "modules/user_interface.h"
#include "wifi_handler.h"
#include "ram_log.h"
#include "webserial_monitor.h"
#include "version.h"
#include "github_update.h"

void ui_unknown() {
    DualSerial.print("\nUnknown command. One of the following Parameters is missing:\n");
}

void ui_info_help() {
    DualSerial.print("info                          - system information about the device");
}

String ui_info() {
    String fw_version = "\nFirmware version:   " + String(FW_VERSION_MAJOR) + "."+ String(FW_VERSION_MINOR) + "." + String(FW_VERSION_PATCH);
    DualSerial.println(fw_version.c_str());
    DualSerial.print("Wifi mode:          "); DualSerial.println(wifi_handler_get_mode());
    DualSerial.print("Wifi connected to:  "); DualSerial.println(WiFi.isConnected() ? WiFi.SSID() : "not connected");
    DualSerial.print("IP-address:         "); DualSerial.println(WiFi.isConnected() ? WiFi.localIP().toString() : "");
    DualSerial.print("Uptime:             "); DualSerial.println(ram_log_time_str(millis()));
    ram_log_print_log();
    return fw_version;
}

void ui_debug_help() {
    DualSerial.print("debug --reboot                        - reboot device\n"
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