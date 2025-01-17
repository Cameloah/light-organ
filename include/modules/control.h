#pragma once

#include "Arduino.h"
#include "main.h"


#define TIMEOUT_MQTT_CONNECT_MS                      5000
#define INTERVAL_MQTT_CHECK                          30000

void control_init();
void control_update();