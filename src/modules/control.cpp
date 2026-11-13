#include "modules/control.h"

#include <PubSubClient.h>
#include <WiFi.h>

pu::Settings control_settings;

// ---------------- MQTT CONTROL ------------------ //

WiFiClient mqtt_client;
PubSubClient mqtt(mqtt_client);

static const char* mqtt_server = "homeassistant.local";
static bool flag_mqtt_was_connected = false;
static bool flag_mqtt_reconnect = true; // connect at the first update and after changed credentials
static bool firstMessage = true;
static unsigned long lastUpdate = 0;

void control_switch(mode_num_t mode) {
    switch (mode) {
    case MODE_BLACKOUT:
        led_mode_switch(MODE_BLACKOUT);
        control_settings.set("musicMode", false);
        control_settings.set("ambientMode", false);
        mqtt.publish("lichtorgel/light/music/state", "off");
        mqtt.publish("lichtorgel/light/ambient/state", "off");
        break;

    case MODE_MUSIC:
        led_mode_switch(MODE_MUSIC);
        control_settings.set("musicMode", true);
        control_settings.set("ambientMode", false);
        mqtt.publish("lichtorgel/light/music/state", "music_on");
        mqtt.publish("lichtorgel/light/ambient/state", "off");
        break;

    case MODE_AMBIENT:
        led_mode_switch(MODE_AMBIENT);
        control_settings.set("musicMode", false);
        control_settings.set("ambientMode", true);
        mqtt.publish("lichtorgel/light/ambient/state", "ambient_on");
        mqtt.publish("lichtorgel/light/music/state", "off");
        break;

    default:
        break;
    }
}

void mqtt_control_callback(char* topic, byte* payload, unsigned int length) {
    String message;
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    ProjectUtils.console().println("Message arrived: " + message);

    if (firstMessage) {
        ProjectUtils.console().println("skipping first message");
        firstMessage = false;
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
void mqtt_connect() {
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
    mqtt.subscribe("lichtorgel/light");
}

// ---------------- WEB CONTROL ------------------ //

void control_build_page() {
    pu::Page& home = ProjectUtils.web().home("Magic Mushroom Light");

    home.section("Light");
    home.toggle(
        "Ambient mode", []() { return control_settings.getBool("ambientMode"); },
        [](bool on) { control_switch(on ? MODE_AMBIENT : MODE_BLACKOUT); });
    home.toggle(
        "Music mode", []() { return control_settings.getBool("musicMode"); },
        [](bool on) { control_switch(on ? MODE_MUSIC : MODE_BLACKOUT); });

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
    control_settings.define("musicMode", false);
    control_settings.define("ambientMode", false);
    control_settings.define("mqtt_user", "");
    control_settings.define("mqtt_pass", "");
    control_settings.loadAll();

    control_build_page();

    if (control_settings.getString("mqtt_user").isEmpty()) {
        ProjectUtils.log().warn("mqtt credentials not set");
    }

    if (control_settings.getBool("musicMode")) {
        control_switch(MODE_MUSIC);
    } else if (control_settings.getBool("ambientMode")) {
        control_switch(MODE_AMBIENT);
    } else {
        control_switch(MODE_BLACKOUT);
    }
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
