#pragma once

#include "main.h"

#define TIMEOUT_MQTT_CONNECT_MS                      5000
#define INTERVAL_MQTT_CHECK                          30000

/// \brief loads the settings, builds the web page and switches to the stored mode
///
void control_init();

/// \brief keeps the MQTT connection alive. Call from the network task.
///
void control_update();
