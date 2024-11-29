#include "modules/control.h"





MemoryModule control_settings("control");


// ---------------- WEB CONTROL ------------------ //

void webfct_control_serve(AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/control.html", "text/html");
}

void webfct_control_get(AsyncWebServerRequest *request){
    if (request->hasParam("musicMode")) {
        control_settings.set("musicMode", request->getParam("musicMode")->value().equals("true"), true);
        if (*control_settings.getBool("musicMode")) {
            led_mode_switch(MODE_MUSIC);
            control_settings.set("ambientMode", false, true);
            }

        else led_mode_switch(MODE_BLACKOUT);
    }

    else if (request->hasParam("ambientMode")) {
        control_settings.set("ambientMode", request->getParam("ambientMode")->value().equals("true"), true);
        if (*control_settings.getBool("ambientMode")) {
            led_mode_switch(MODE_AMBIENT);
            control_settings.set("musicMode", false, true);
            }

        else led_mode_switch(MODE_BLACKOUT);
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

void control_init() {
    control_settings.addParameter("musicMode", false);
    control_settings.addParameter("ambientMode", false);
    control_settings.loadAll();

    if (*control_settings.getBool("musicMode"))
        led_mode_switch(MODE_MUSIC);

    else if (*control_settings.getBool("ambientMode"))
        led_mode_switch(MODE_AMBIENT);

    else led_mode_switch(MODE_BLACKOUT);

    server.on("/control/mode", HTTP_GET, webfct_get_musicMode);
    server.on("/control/toggle", HTTP_GET, webfct_control_get); 
    server.on("/control", HTTP_GET, webfct_control_serve);
}

