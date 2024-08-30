// config.properties.h
#ifndef CONFIG_PROPERTIES_H
#define CONFIG_PROPERTIES_H

// WiFi settings
#define WIFI_SSID "changeme"
#define WIFI_PASSWORD "changeme"

#define SECRET_SSID "changeme"    // replace MySSID with your WiFi network name
#define SECRET_PASS "changeme"  // replace MyPassword with your WiFi password

// Weather Underground settings
#define WU_API_KEY "YOUR_API_KEY"
#define WU_STATION_ID "YOUR_STATION_ID"

// SHT30 sensor settings
#define SHT30_ADDRESS 0x45

// Deep sleep duration (in seconds)
#define SLEEP_DURATION 600

// Temp Corretion %
#define CORRECTION 0.92


#endif // CONFIG_PROPERTIES_H
