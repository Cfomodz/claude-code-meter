#ifndef NETWORK_H
#define NETWORK_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Preferences.h>
#include "config.h"

// ============================================================================
// Network Manager — WiFi Provisioning, TLS, Webhook Polling
// ============================================================================
//
// Handles:
//   1. WiFi provisioning via captive portal (WiFiManager)
//   2. Persistent storage of webhook URL and display mode
//   3. HTTPS polling of n8n webhook (or direct API)
//   4. TLS with root CA validation
//
// Security Model:
//   - The ESP32 never stores the sk-ant-admin key.
//   - Credentials are managed by the n8n middleware.
//   - TLS 1.2+ is enforced for all connections.

// Response from a webhook poll
struct PollResult {
    bool success;
    int httpCode;
    String payload;    // Raw JSON body on success
    String errorMsg;   // Human-readable error on failure
};

class NetworkManager {
public:
    NetworkManager();

    // Start WiFi using stored credentials, or launch captive portal if unconfigured.
    // Returns true if connected to WiFi.
    bool begin();

    // Check if WiFi is currently connected
    bool isConnected();

    // Poll the configured webhook URL and return the result.
    PollResult poll();

    // Get the stored webhook URL
    String getWebhookUrl();

    // Get the stored display mode ("cost" or "tokens")
    String getDisplayMode();

    // Reset stored WiFi credentials and webhook config (factory reset)
    void resetConfig();

private:
    WiFiManager _wifiManager;
    Preferences _preferences;
    WiFiClientSecure _secureClient;

    String _webhookUrl;
    String _displayMode;

    // Custom WiFiManager parameters
    WiFiManagerParameter* _paramWebhook;
    WiFiManagerParameter* _paramMode;

    // Callback: save custom parameters after portal config
    static void _saveConfigCallback();
    static NetworkManager* _instance;  // For static callback access

    void _loadPreferences();
    void _savePreferences();
    void _setupTLS();
};

#endif // NETWORK_H
