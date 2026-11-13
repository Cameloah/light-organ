#pragma once

#include "led_config.h"

/// \brief installs the DMX driver
///
void dmx_init();

/// \brief receives DMX packets and switches to DMX mode while a controller is connected.
///        Call from the network task.
///
void dmx_listen();

/// \brief draws the received DMX values into the LEDs
///
void dmx_update(LED_MUSHROOMS_SET_t* user_buffer);
