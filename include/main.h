#pragma once

// ---------- DEBUG AND SYSTEM CONTROL ---------- //

// #define DEBUG_DISPLAY_LOOP_FRQ          // use this to output the loop freq in Hz via serial print
#define SYSCTRL_LOOPTIMER               // enable loop frequency control, remember to also set the loop freq in the loop_timer.h

#define SYS_CONTROL_STAT_IP
#define SYS_CONTROL_WEBSERIAL

// set this to any password to protect the AP
#define AP_PASSWORD                     nullptr

// AP verbosity:
// 1 - AP will spawn if esp cannot connect to existing wifi for any reason
// 2 - AP will always spawn, even if connected to existing wifi
#define AP_VERBOSITY                    1

// comment this out to have an always on AP once its spawned
// #define AP_TIMEOUT                      300000      // 5 min
#define TIMEOUT_WIFI_CONNECT_MS         5000

// time module settings
#define TIMEZONE_SEC_OFFSET             3600  // CET is UTC+1
#define DAYLIGHT_SAVING_SEC_OFFSET      3600  // Additional offset for DST (CEST is UTC+2)