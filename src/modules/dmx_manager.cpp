#include <Arduino.h>
#include <esp_dmx.h>

#include "main.h"
#include "modules/dmx_manager.h"

#include "main_project_utils.h"
#include "ram_log.h"

int transmitPin = 17;       // TX
int receivePin = 16;        // RX
int enablePin = 15;         // DE


dmx_port_t dmxPort = 1;

/* Now we want somewhere to store our DMX data. Since a single packet of DMX
  data can be up to 513 bytes long, we want our array to be at least that long.
  This library knows that the max DMX packet size is 513, so we can fill in the
  array size with `DMX_PACKET_SIZE`. */
byte data[DMX_PACKET_SIZE];

bool dmxIsConnected = false;
unsigned long lastUpdate = millis();

void dmx_init() {

  /* Now we will install the DMX driver! We'll tell it which DMX port to use,
    what device configuration to use, and what DMX personalities it should have.
    If you aren't sure which configuration to use, you can use the macros
    `DMX_CONFIG_DEFAULT` to set the configuration to its default settings.
    This device is being setup as a DMX responder so it is likely that it should
    respond to DMX commands. It will need at least one DMX personality. Since
    this is an example, we will use a default personality which only uses 1 DMX
    slot in its footprint. */
  dmx_config_t config = DMX_CONFIG_DEFAULT;
  dmx_personality_t personalities[] = {
    {1, "Default Personality"}
  };
  int personality_count = 1;
  dmx_driver_install(dmxPort, &config, personalities, personality_count);

  /* Now set the DMX hardware pins to the pins that we want to use and setup
    will be complete! */
  dmx_set_pin(dmxPort, transmitPin, receivePin, enablePin);
}

void dmx_listen() {
  /* We need a place to store information about the DMX packets we receive. We
    will use a dmx_packet_t to store that packet information.  */
    dmx_packet_t packet;
    
  /* And now we wait! The DMX standard defines the amount of time until DMX
    officially times out. That amount of time is converted into ESP32 clock
    ticks using the constant `DMX_TIMEOUT_TICK`. If it takes longer than that
    amount of time to receive data, this if statement will evaluate to false. */
  if (dmx_receive(dmxPort, &packet, DMX_TIMEOUT_TICK)) {
    if (!packet.err) {
      /* If this is the first DMX data we've received, lets log it! */
      if (!dmxIsConnected) {
        ram_log_notify(RAM_LOG_INFO, "DMX is connected!", true);
        led_mode_switch(3);
        dmxIsConnected = true;
      }

      dmx_read(dmxPort, data, packet.size);
    }

    else {
      ram_log_notify(RAM_LOG_INFO, "A DMX error occurred.", true);
    }
  } 
  
  else if (dmxIsConnected) {
    ram_log_notify(RAM_LOG_INFO, "DMX was disconnected.", true);
    led_mode_switch(0);
    dmxIsConnected = false;
  }
}


void dmx_update(LED_MUSHROOMS_SET_t* user_buffer) {
  user_buffer->leds_whiteshrooms = CRGB(data[1], data[2], data[3]);
}
