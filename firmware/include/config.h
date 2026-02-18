#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// Claude Code Meter — Hardware & Firmware Configuration
// ============================================================================
//
// Pinout: VSPI bus to MAX7219 4-in-1 Dot Matrix via 74HCT125 level shifter
// All SPI signals require 3.3V → 5.0V level shifting.
//
//  ESP32 GPIO  │ MAX7219 Pin │ Function │ Logic Level
//  ────────────┼─────────────┼──────────┼────────────
//  5V / VBUS   │ VCC         │ Power    │ 5.0V
//  GND         │ GND         │ Ground   │ 0V
//  GPIO 23     │ DIN         │ MOSI     │ 3.3V → 5.0V
//  GPIO 5      │ CS          │ SS       │ 3.3V → 5.0V
//  GPIO 18     │ CLK         │ SCK      │ 3.3V → 5.0V
// ============================================================================

#include <Arduino.h>

// ---------------------------------------------------------------------------
// SPI / Display Pins (VSPI)
// ---------------------------------------------------------------------------
#define PIN_SPI_MOSI   23   // DIN on MAX7219
#define PIN_SPI_CS      5   // CS  on MAX7219
#define PIN_SPI_CLK    18   // CLK on MAX7219

// ---------------------------------------------------------------------------
// MAX7219 Display Configuration
// ---------------------------------------------------------------------------
#define DISPLAY_NUM_DEVICES   4    // 4-in-1 dot matrix module
#define HARDWARE_TYPE         MD_MAX72XX::FC16_HW  // Common FC-16 module type

// Display brightness 0–15
#define DISPLAY_BRIGHTNESS    4

// Scroll speed (ms per frame). Lower = faster.
#define SCROLL_SPEED_MS       35

// Pause time (ms) between scroll cycles
#define SCROLL_PAUSE_MS       2000

// ---------------------------------------------------------------------------
// Network Configuration
// ---------------------------------------------------------------------------

// WiFi provisioning AP name and password
#define AP_NAME          "ClaudeMeter_Setup"
#define AP_PASSWORD      ""   // Open AP for initial setup

// Polling interval in milliseconds (how often to fetch cost data)
#define POLL_INTERVAL_MS  60000   // 60 seconds

// HTTP timeout for webhook/API requests (ms)
#define HTTP_TIMEOUT_MS   10000

// Maximum consecutive network failures before showing E-WIFI
#define MAX_NET_FAILURES  5

// ---------------------------------------------------------------------------
// n8n Webhook Configuration
// ---------------------------------------------------------------------------
// The actual URL is stored in persistent preferences (set via captive portal).
// This is the key name used in the Preferences namespace.
#define PREF_NAMESPACE       "claude_meter"
#define PREF_KEY_WEBHOOK     "webhook_url"
#define PREF_KEY_MODE        "display_mode"  // "cost" or "tokens"

// ---------------------------------------------------------------------------
// Cost Display
// ---------------------------------------------------------------------------

// Prefix character for cost display
#define COST_PREFIX          "$"

// Number of decimal places for cost display
#define COST_DECIMALS        2

// ---------------------------------------------------------------------------
// Error Codes (displayed on 7-segment / dot matrix)
// ---------------------------------------------------------------------------
#define ERR_WIFI   "E-WIFI"    // Network connection lost
#define ERR_TLS    "E-TLS"     // SSL handshake failure
#define ERR_API    "E-API"     // 401/403 — invalid key or auth error
#define ERR_JSON   "E-JSON"    // JSON parsing failure
#define ERR_HTTP   "E-HTTP"    // Non-200 HTTP response

#endif // CONFIG_H
