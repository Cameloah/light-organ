#include "modules/control.h"

#include <PubSubClient.h>
#include <WiFi.h>

static pu::Settings control_settings;

// ---------------- MODE ------------------ //

// Shown on the web page; the position of each name is its mode_num_t value.
static const char* const MODE_NAMES[] = {"Off", "Music", "Ambient"};

// ---------------- MQTT CONTROL ------------------ //

static WiFiClient mqtt_client;
static PubSubClient mqtt(mqtt_client);

static const char* mqtt_server = "homeassistant.local";
static bool flag_mqtt_was_connected = false;
static bool flag_mqtt_reconnect = true; // connect at the first update and after changed credentials
static unsigned long lastUpdate = 0;

// The broker hands out a retained command right after subscribing. It would override the mode
// stored on the device, so commands arriving this soon after a (re)connect are ignored.
static const uint32_t RETAINED_WINDOW_MS = 1000;
static uint32_t ignore_commands_until_ms = 0;

static void control_publish_state() {
    const int32_t mode = control_settings.getInt("mode");
    mqtt.publish("lichtorgel/light/music/state", mode == MODE_MUSIC ? "music_on" : "off");
    mqtt.publish("lichtorgel/light/ambient/state", mode == MODE_AMBIENT ? "ambient_on" : "off");
}

// Stores the mode, switches the LEDs and reports the new state to Home Assistant.
// Always called from the network task (or from setup() before that task starts).
static void control_switch(mode_num_t mode) {
    if (mode > MODE_AMBIENT) {
        return; // DMX is switched by the DMX module itself and not stored
    }
    if (control_settings.getInt("mode") != mode) {
        control_settings.set("mode", static_cast<int>(mode));
    }
    led_request_mode(mode);
    control_publish_state();
}

static void mqtt_control_callback(char* topic, byte* payload, unsigned int length) {
    String message;
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    ProjectUtils.console().println("Message arrived: " + message);

    if (static_cast<int32_t>(millis() - ignore_commands_until_ms) < 0) {
        ProjectUtils.console().println("ignoring retained message after connecting");
        return;
    }

    if (String(topic) == "lichtorgel/light") {
        if (message == "music_on") {
            control_switch(MODE_MUSIC);
        } else if (message == "ambient_on") {
            control_switch(MODE_AMBIENT);
        } else {
            control_switch(MODE_BLACKOUT);
        }
        // the home page picks the new state up by itself within a second
    }
}

// One connection attempt. Called again by control_update() after INTERVAL_MQTT_CHECK.
static void mqtt_connect() {
    const String user = control_settings.getString("mqtt_user");
    const String pass = control_settings.getString("mqtt_pass");
    if (user.isEmpty() || !ProjectUtils.wifi().isConnected()) {
        return;
    }

    mqtt.setServer(mqtt_server, 1883);
    mqtt.setSocketTimeout(TIMEOUT_MQTT_CONNECT_MS / 1000);
    if (!mqtt.connect("lichtorgel", user.c_str(), pass.c_str())) {
        if (flag_mqtt_was_connected) {
            ProjectUtils.log().error("mqtt connection failed, state %d", mqtt.state());
            flag_mqtt_was_connected = false;
        }
        return;
    }
    ProjectUtils.log().info("mqtt connected");
    flag_mqtt_was_connected = true;

    mqtt.setCallback(mqtt_control_callback);
    ignore_commands_until_ms = millis() + RETAINED_WINDOW_MS;
    mqtt.subscribe("lichtorgel/light");
    control_publish_state(); // Home Assistant may have missed changes while disconnected
}

// ---------------- WEB CONTROL ------------------ //

static void control_build_page() {
    pu::Page& home = ProjectUtils.web().home("Magic Mushroom Light");

    home.section("Light");
    home.select(control_settings, "mode", "Mode", {MODE_NAMES[0], MODE_NAMES[1], MODE_NAMES[2]})
        .onChange([]() { control_switch(static_cast<mode_num_t>(control_settings.getInt("mode"))); });

    home.section("MQTT");
    const auto reconnect = []() {
        mqtt.disconnect();
        flag_mqtt_reconnect = true;
    };
    home.text(control_settings, "mqtt_user", "User").onChange(reconnect);
    home.password(control_settings, "mqtt_pass", "Password").onChange(reconnect);
    home.indicator("Connected to broker", []() { return mqtt.connected(); });

    // the page used to live at /control
    ProjectUtils.server().on("/control", HTTP_GET,
                             [](AsyncWebServerRequest* request) { request->redirect("/"); });
}

void control_init() {
    control_settings.begin("control", &ProjectUtils.log());
    control_settings.define("mode", -1); // -1: not stored yet
    control_settings.define("mqtt_user", "");
    control_settings.define("mqtt_pass", "");
    // Up to firmware 1.1 the mode was stored as two flags; read once to carry it over.
    control_settings.define("musicMode", false);
    control_settings.define("ambientMode", false);
    control_settings.loadAll();

    if (control_settings.getInt("mode") < 0) {
        const mode_num_t mode = control_settings.getBool("musicMode")     ? MODE_MUSIC
                                : control_settings.getBool("ambientMode") ? MODE_AMBIENT
                                                                          : MODE_BLACKOUT;
        control_settings.set("mode", static_cast<int>(mode));
    }

    control_build_page();

    if (control_settings.getString("mqtt_user").isEmpty()) {
        ProjectUtils.log().warn("mqtt credentials not set");
    }

    control_switch(static_cast<mode_num_t>(control_settings.getInt("mode")));
}

void control_update() {
    const unsigned long now = millis();
    if (flag_mqtt_reconnect || now - lastUpdate >= INTERVAL_MQTT_CHECK) {
        flag_mqtt_reconnect = false;
        lastUpdate = now;
        if (!mqtt.connected()) {
            if (flag_mqtt_was_connected) {
                ProjectUtils.log().error("mqtt connection lost. reconnecting...");
            }
            mqtt_connect();
        }
    }
    mqtt.loop();
}
