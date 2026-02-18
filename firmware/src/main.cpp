// ============================================================================
// Claude Code Meter — ESP32 Firmware
// ============================================================================
//
// Physical token-usage / cost meter for Anthropic Claude API.
//
// Architecture:
//   ESP32 + MAX7219 4-in-1 Dot Matrix → polls n8n webhook (or direct API)
//   → displays real-time cost or token count on desk-mounted display.
//
// Data Flow:
//   1. Boot → WiFi provisioning via captive portal (WiFiManager)
//   2. Run  → Poll n8n webhook every POLL_INTERVAL_MS
//   3. Parse JSON → extract cost_usd or token counts
//   4. Render on MAX7219 via MD_Parola
//
// Error Codes (shown on display):
//   E-WIFI  — WiFi disconnected
//   E-TLS   — TLS handshake failed
//   E-API   — 401/403 from API
//   E-JSON  — JSON parse error
//   E-HTTP  — Other HTTP error
//
// Factory Reset:
//   Hold GPIO 0 (BOOT button) for 5 seconds during operation.
//
// ============================================================================

#include <Arduino.h>
#include "config.h"
#include "display.h"
#include "network.h"
#include "parser.h"

// ---------------------------------------------------------------------------
// State Machine
// ---------------------------------------------------------------------------
enum DeviceState {
    STATE_BOOT,          // Initial boot animation
    STATE_CONNECTING,    // WiFi connection / captive portal
    STATE_RUNNING,       // Normal operation — polling and displaying
    STATE_ERROR,         // Displaying an error code
    STATE_PORTAL_ACTIVE  // Captive portal is active, waiting for config
};

// ---------------------------------------------------------------------------
// Globals
// ---------------------------------------------------------------------------
static DisplayManager display;
static NetworkManager network;
static DeviceState state = STATE_BOOT;

static unsigned long lastPollTime = 0;
static unsigned long lastWifiCheck = 0;
static int consecutiveFailures = 0;

// Factory reset: hold BOOT button (GPIO 0) for 5 seconds
#define RESET_BUTTON_PIN  0
#define RESET_HOLD_MS     5000
static unsigned long resetButtonDown = 0;
static bool resetButtonActive = false;

// Last displayed data (for trend comparison)
static float lastCostUsd = 0.0f;

// ---------------------------------------------------------------------------
// Forward Declarations
// ---------------------------------------------------------------------------
void handleBoot();
void handleConnecting();
void handleRunning();
void handleError(const char* errorCode);
void checkFactoryReset();
void pollAndDisplay();

// ---------------------------------------------------------------------------
// Setup
// ---------------------------------------------------------------------------
void setup() {
    Serial.begin(115200);
    delay(500);

    log_i("=== Claude Code Meter v1.0 ===");
    log_i("Heap free: %u bytes", ESP.getFreeHeap());

    // Initialize display hardware
    display.begin();
    state = STATE_BOOT;

    // Factory reset button
    pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
}

// ---------------------------------------------------------------------------
// Main Loop
// ---------------------------------------------------------------------------
void loop() {
    // Always tick the display animation
    display.update();

    // Check for factory reset hold
    checkFactoryReset();

    switch (state) {
        case STATE_BOOT:
            handleBoot();
            break;

        case STATE_CONNECTING:
            handleConnecting();
            break;

        case STATE_RUNNING:
            handleRunning();
            break;

        case STATE_ERROR:
            // Errors auto-recover: retry WiFi check periodically
            if (millis() - lastWifiCheck > 10000) {
                lastWifiCheck = millis();
                if (network.isConnected()) {
                    state = STATE_RUNNING;
                    consecutiveFailures = 0;
                }
            }
            break;

        case STATE_PORTAL_ACTIVE:
            // WiFiManager handles the portal in the background
            if (network.isConnected()) {
                display.showStatic("OK");
                delay(500);
                state = STATE_RUNNING;
                lastPollTime = 0;  // Force immediate poll
            }
            break;
    }
}

// ---------------------------------------------------------------------------
// State Handlers
// ---------------------------------------------------------------------------

void handleBoot() {
    display.showBootAnimation();
    log_i("Boot animation complete");
    state = STATE_CONNECTING;
}

void handleConnecting() {
    display.showStatic("WiFi");

    bool connected = network.begin();

    if (connected) {
        log_i("WiFi connected, entering run mode");
        display.showStatic("OK");
        delay(500);
        state = STATE_RUNNING;
        lastPollTime = 0;  // Force immediate first poll
    } else {
        log_w("WiFi not connected — captive portal may be active");
        display.showScrolling("Setup: Connect to ClaudeMeter_Setup WiFi");
        state = STATE_PORTAL_ACTIVE;
    }
}

void handleRunning() {
    // Periodic WiFi health check
    if (millis() - lastWifiCheck > 30000) {
        lastWifiCheck = millis();
        if (!network.isConnected()) {
            log_w("WiFi connection lost");
            handleError(ERR_WIFI);
            return;
        }
    }

    // Poll webhook at configured interval
    if (millis() - lastPollTime >= POLL_INTERVAL_MS || lastPollTime == 0) {
        lastPollTime = millis();
        pollAndDisplay();
    }
}

void handleError(const char* errorCode) {
    log_e("Error state: %s", errorCode);
    display.showError(errorCode);
    state = STATE_ERROR;
    lastWifiCheck = millis();
}

// ---------------------------------------------------------------------------
// Core Logic: Poll webhook and update display
// ---------------------------------------------------------------------------

void pollAndDisplay() {
    log_i("Polling webhook... (heap: %u)", ESP.getFreeHeap());

    PollResult result = network.poll();

    if (!result.success) {
        consecutiveFailures++;
        log_w("Poll failed (%d/%d): %s (HTTP %d)",
              consecutiveFailures, MAX_NET_FAILURES,
              result.errorMsg.c_str(), result.httpCode);

        if (consecutiveFailures >= MAX_NET_FAILURES) {
            handleError(result.errorMsg.c_str());
        }
        return;
    }

    // Reset failure counter on success
    consecutiveFailures = 0;

    // Parse response based on content
    MeterData data;

    // Detect response format: if it contains "data" array, it's direct API
    if (result.payload.indexOf("\"data\"") >= 0) {
        data = Parser::parseAnthropicUsage(result.payload);
    } else {
        data = Parser::parseWebhookResponse(result.payload);
    }

    if (!data.valid) {
        handleError(ERR_JSON);
        return;
    }

    log_i("Cost: $%.2f | Tokens: %llu | Trend: %s",
          data.costUsd, (unsigned long long)data.tokens.totalTokens,
          data.trend.c_str());

    // Display based on configured mode
    String mode = network.getDisplayMode();

    if (mode == "tokens") {
        display.showTokens(data.tokens.totalTokens);
    } else {
        display.showCost(data.costUsd);
    }

    lastCostUsd = data.costUsd;
}

// ---------------------------------------------------------------------------
// Factory Reset (hold BOOT button for 5 seconds)
// ---------------------------------------------------------------------------

void checkFactoryReset() {
    bool pressed = (digitalRead(RESET_BUTTON_PIN) == LOW);

    if (pressed && !resetButtonActive) {
        resetButtonActive = true;
        resetButtonDown = millis();
    } else if (pressed && resetButtonActive) {
        if (millis() - resetButtonDown >= RESET_HOLD_MS) {
            log_w("Factory reset triggered!");
            display.showScrolling("RESET...");
            delay(1000);
            network.resetConfig();
            ESP.restart();
        }
    } else {
        resetButtonActive = false;
    }
}
