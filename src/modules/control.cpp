#include "modules/control.h"
#include "PubSubClient.h"
#include "webserial_monitor.h"
#include "main_project_utils.h"
#include "ram_log.h"




MemoryModule control_settings("control");
AsyncEventSource events("/events");


// ---------------- MQTT CONTROL ------------------ //

WiFiClient mqtt_client;
PubSubClient mqtt(mqtt_client);	

const char* mqtt_server = "192.168.0.22";

void control_switch(mode_num_t mode){
    switch (mode)
    {
    case MODE_BLACKOUT:
        led_mode_switch(MODE_BLACKOUT);
        control_settings.set("musicMode", false, true);
        control_settings.set("ambientMode", false, true);
        mqtt.publish("lichtorgel/light/music/state","off");
        mqtt.publish("lichtorgel/light/ambient/state","off");
        break;

    case MODE_MUSIC:
        led_mode_switch(MODE_MUSIC);
        control_settings.set("musicMode", true, true);
        control_settings.set("ambientMode", false, true);
        mqtt.publish("lichtorgel/light/music/state","music_on");
        mqtt.publish("lichtorgel/light/ambient/state","off");
        break;

    case MODE_AMBIENT:
        led_mode_switch(MODE_AMBIENT);
        control_settings.set("musicMode", false, true);
        control_settings.set("ambientMode", true, true);
        mqtt.publish("lichtorgel/light/ambient/state","ambient_on");
        mqtt.publish("lichtorgel/light/music/state","off");
        break;

    default:
        break;
    }
}

bool firstMessage = true;

void mqtt_control_callback(char* topic, byte* payload, unsigned int length) {
    DualSerial.print("Message arrived: ");

    String message;
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    DualSerial.println(message);

    if (firstMessage) {
        DualSerial.println("skipping first message");
        firstMessage = false;
        return;
    }

    if (String(topic) == "lichtorgel/light") {
        control_settings.set("musicMode", message == "music_on", true);
        if (message == "music_on") {
            control_switch(MODE_MUSIC);
        }
        else if (message == "ambient_on") {
            control_switch(MODE_AMBIENT);
        }
        else {
            control_switch(MODE_BLACKOUT);
        }

        events.send("update", "state");
    }
}

void mqtt_connect(String user, String pass) {
    mqtt.setServer(mqtt_server, 1883);

    DualSerial.print("connecting to mqtt broker...");
    unsigned long startAttemptTime = millis();
    while (!mqtt.connect("lichtorgel", user.c_str(), pass.c_str())) {
        if (millis() - startAttemptTime > TIMEOUT_MQTT_CONNECT_MS) {
            ram_log_notify(RAM_LOG_ERROR_SYSTEM, "mqtt connection timed out!", true);
            return;
        }
        DualSerial.print(".");
        delay(500);
    }
    ram_log_notify(RAM_LOG_INFO, "mqtt connected", true);

    mqtt.setCallback(mqtt_control_callback);
    mqtt.subscribe("lichtorgel/light");
}


// ---------------- WEB CONTROL ------------------ //

void webfct_control_serve(AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/control.html", "text/html");
}

void webfct_control_get(AsyncWebServerRequest *request){
    if (request->hasParam("musicMode")) {
        if (request->getParam("musicMode")->value().equals("true")) {
            control_switch(MODE_MUSIC);
        }
        
        else control_switch(MODE_BLACKOUT);
    }

    else if (request->hasParam("ambientMode")) {
        if (request->getParam("ambientMode")->value().equals("true")) {
            control_switch(MODE_AMBIENT);
        }

        else control_switch(MODE_BLACKOUT);
    }

    else {
        request->send(400, "text/plain", "400: Invalid Request");
        return; }

    request->send(200, "text/plain", "OK");
}

void webfct_get_musicMode(AsyncWebServerRequest *request) {
    String payload = String(*control_settings.getBool("ambientMode")) + "\n"
    + String(*control_settings.getBool("musicMode")) + "\n";
    request->send(200, "text/plain", payload);
}

void webfct_mqtt_post(AsyncWebServerRequest *request) {

    int params = request->params();
    for(int i=0;i<params;i++){
        AsyncWebParameter* p = request->getParam(i);
        if(p->isPost()){
            // save wifi settings
            if (p->name() == "mqtt_user") {
                control_settings.set("mqtt_user", p->value(), true);
                DualSerial.print("mqtt_user set to: ");
                DualSerial.println(p->value());
            }
    
            if (p->name() == "mqtt_pass") {
                control_settings.set("mqtt_pass", p->value(), true);
                DualSerial.print("mqtt_pass set to: ");
                DualSerial.println(p->value());
            }
        }
    }
    request->send(200, "text/plain", "Done. ESP will restart and apply settings.");
    delay(3000);
    ESP.restart();
}

void webfct_get_mqtt_cred(AsyncWebServerRequest *request) {

    String payload = control_settings.getString("mqtt_user") + "\n"
                    + control_settings.getString("mqtt_pass");
    
    request->send(200, "text/plain", payload);
}

void control_init() {
    control_settings.addParameter("musicMode", false);
    control_settings.addParameter("ambientMode", false);
    control_settings.loadAll();

    //add SSE handler
    server.addHandler(&events);

    server.on("/control", HTTP_POST, webfct_mqtt_post);                                       // listens for POST requests and saves mqtt user data

    server.on("/control/mode", HTTP_GET, webfct_get_musicMode);
    server.on("/control/toggle", HTTP_GET, webfct_control_get); 
    server.on("/control/mqtt-config", HTTP_GET, webfct_get_mqtt_cred);                                             // publishes wifi credentials
    server.on("/control", HTTP_GET, webfct_control_serve);

    // MQTT
    control_settings.addParameter("mqtt_user", String(""));
    control_settings.addParameter("mqtt_pass", String(""));
    int retval = control_settings.load("mqtt_user");
    retval += control_settings.load("mqtt_pass");

    if(retval == ESP_OK)
        mqtt_connect(control_settings.getString("mqtt_user"), control_settings.getString("mqtt_pass"));

    else ram_log_notify(RAM_LOG_ERROR_MEMORY, "mqtt credentials not found", true);


    if (*control_settings.getBool("musicMode"))
        control_switch(MODE_MUSIC);
    else if (*control_settings.getBool("ambientMode"))
        control_switch(MODE_AMBIENT);
    else control_switch(MODE_BLACKOUT);
}

static unsigned long lastUpdate = 0;

void control_update() {
    
    unsigned long now = millis();
    if (now - lastUpdate >= INTERVAL_MQTT_CHECK) { // 5000 milliseconds = 5 seconds
        lastUpdate = now;
        if (!mqtt.connected())
            mqtt_connect(control_settings.getString("mqtt_user"), control_settings.getString("mqtt_pass"));
    }
    mqtt.loop();
}