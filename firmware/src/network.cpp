#include "network.h"

// ============================================================================
// Network Manager Implementation
// ============================================================================

// Mozilla/ISRG Root X1 — covers Let's Encrypt (common for n8n instances)
// and many other certificate chains. For api.anthropic.com, Amazon Root CA 1
// is also included below.
static const char* ROOT_CA_ISRG = R"EOF(
-----BEGIN CERTIFICATE-----
MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw
TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh
cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4
WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu
ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY
MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc
h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+
0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6
UA5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+s
WT8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qy
HB5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+
UCB5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHz
UvKBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahm
bWnOlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3Sz
zmbbDRRgkUo/8Oll8rR6/9Zy0RFPy5Fg2htE6V7Qa3uvECo3B7DYIHQb9LpGMO2
FlI+EFVCNGpY8pMj+AiOhS8M/lPQNe6lPtlarNTNYN7VKq4SWPFglRbDeVmncVoc
lXGKcdBf25XBqERCMLIRAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNVHRMB
Af8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkqhkiG
9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZLnBhb
DLfhHL/k4tWHLUmVZRfCnRCJqAch+AR9NqGRJ+9jJellICG2E+vN2at+r7QLW2d8
WNT/0Rsid+aOPQkBkXKQAhLSjfaWfjREnMgJAIbehH1qqCBMclKJPFmkgDRsRkfV
7MkPHShA0MLDcp1AOLV+bpCMakUlPScQP4MHI1T40mONhIpJNQyE6oE6iV9u0Pkz
6YMxrJDMRBnIMC1U3bHFH4kBfCD03K0+KJzJHEIX2+jKGcF0cMV1f34/BbPwamrB
X+hrGB8BQKSEV7dbxPauVtNN5Qy1P/FrjU7XBYEFNX5ETPCK/2fICBDcB2sLF9u
p71EZIG7fL+e+mGGI75RIaDGSzflUPCtHKmncRWmKNGmSqgAMCIVMPM8Mk2VqjER
NhBERVLNDveYf6Qgrl/K1nZ2+NZuPBKgbS+Oh0S0cciGMCHotLLnRWsDFNz59EBZ
3r27hbhE8Of39nmLFMRXrySohii3NJBuieYWTWGeRpD2I0J+i4p/WcCGyUJIsBJP
FqMJp/CB3VnFdqSoVF5NK/BGCV8CuxuMlSeyZePJbHIylNQV/v7LIiRjoUSG7Frt
pEA09so8v1YDbd0lOfseiqhzVeAY9DJH3PiD7q8W3dcG9PFdR9C9JE/r5Ck=
-----END CERTIFICATE-----
)EOF";

// Amazon Root CA 1 — covers api.anthropic.com and many AWS-hosted services
static const char* ROOT_CA_AMAZON = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNjMGEwDgYDVR0PAQH/BAQDAgGGMA8GA1UdEwEB/wQFMAMB
Af8wHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA8GA1UdIwQIMAaABBeN
5CUwDQYJKoZIhvcNAQELBQADggEBAHafN3wN1wv0EdCb8VPKqVMHqeQBLPErIa6E
PlS8k27sQ0utMSDLIVfUmbrAjORnAoYAJfOxk9+2qjNb6GjUGRun6b4FplnRsbTD
Xd/4J+JFXlLMdce5DPVJ3rgOl1u8T8MAJS1S+7lJpLJjBKhJLjjxlmV5GcGvJHS
v37eLfjZvEkJdL99uNaTx9EqrdvJsnJMIfcVXQB8cWpFsLMN39FJ6XFDK8SOfILm
+QVII9fS2MaFhL4xU65C0G8q7MPb/0RNQSR7CqzfGd0S2wy0TjChwW1gYfsBTNb
HK8NhXBx4SA7WkL/LFaaTkH4VudbVLfVBKKQheS0tvQ9GMz8Z/k=
-----END CERTIFICATE-----
)EOF";

// Static instance pointer for WiFiManager callback
NetworkManager* NetworkManager::_instance = nullptr;

NetworkManager::NetworkManager()
    : _paramWebhook(nullptr),
      _paramMode(nullptr)
{
    _instance = this;
}

bool NetworkManager::begin() {
    _loadPreferences();
    _setupTLS();

    // Add custom parameters to the captive portal
    _paramWebhook = new WiFiManagerParameter(
        "webhook", "n8n Webhook URL", _webhookUrl.c_str(), 256);
    _paramMode = new WiFiManagerParameter(
        "mode", "Display Mode (cost/tokens)", _displayMode.c_str(), 16);

    _wifiManager.addParameter(_paramWebhook);
    _wifiManager.addParameter(_paramMode);
    _wifiManager.setSaveParamsCallback(_saveConfigCallback);

    // Non-blocking: returns false if portal is active, true if connected
    _wifiManager.setConfigPortalTimeout(300);  // 5 min portal timeout

    bool connected = _wifiManager.autoConnect(AP_NAME, AP_PASSWORD);

    if (connected) {
        log_i("WiFi connected: %s (RSSI: %d dBm)",
              WiFi.localIP().toString().c_str(), WiFi.RSSI());
    }

    return connected;
}

bool NetworkManager::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

PollResult NetworkManager::poll() {
    PollResult result = { false, 0, "", "" };

    if (!isConnected()) {
        result.errorMsg = ERR_WIFI;
        return result;
    }

    if (_webhookUrl.length() == 0) {
        result.errorMsg = "NO_URL";
        return result;
    }

    HTTPClient https;
    https.setTimeout(HTTP_TIMEOUT_MS);

    // Determine if URL is HTTPS
    if (_webhookUrl.startsWith("https://")) {
        if (!https.begin(_secureClient, _webhookUrl)) {
            result.errorMsg = ERR_TLS;
            return result;
        }
    } else {
        // Allow HTTP for local n8n instances on trusted networks
        // Use member variable to persist client for duration of request
        if (!https.begin(_plainClient, _webhookUrl)) {
            result.errorMsg = ERR_HTTP;
            return result;
        }
    }

    https.addHeader("Accept", "application/json");
    https.addHeader("User-Agent", "ClaudeCodeMeter/1.0 ESP32");

    int httpCode = https.GET();
    result.httpCode = httpCode;

    if (httpCode == HTTP_CODE_OK) {
        result.success = true;
        result.payload = https.getString();
    } else if (httpCode == 401 || httpCode == 403) {
        result.errorMsg = ERR_API;
    } else if (httpCode < 0) {
        // WiFiClientSecure / HTTPClient error codes are negative
        result.errorMsg = ERR_TLS;
    } else {
        result.errorMsg = ERR_HTTP;
    }

    https.end();
    return result;
}

String NetworkManager::getWebhookUrl() {
    return _webhookUrl;
}

String NetworkManager::getDisplayMode() {
    return _displayMode;
}

void NetworkManager::resetConfig() {
    _preferences.begin(PREF_NAMESPACE, false);
    _preferences.clear();
    _preferences.end();
    _wifiManager.resetSettings();
    log_w("Factory reset: all config cleared");
}

// --- Private Methods ---

void NetworkManager::_saveConfigCallback() {
    if (_instance) {
        _instance->_webhookUrl = _instance->_paramWebhook->getValue();
        _instance->_displayMode = _instance->_paramMode->getValue();

        // Refresh TLS settings in case the new URL requires a different CA
        _instance->_setupTLS();

        // Validate display mode
        if (_instance->_displayMode != "cost" && _instance->_displayMode != "tokens") {
            _instance->_displayMode = "cost";
        }

        _instance->_savePreferences();
        log_i("Config saved — webhook: %s, mode: %s",
              _instance->_webhookUrl.c_str(), _instance->_displayMode.c_str());
    }
}

void NetworkManager::_loadPreferences() {
    _preferences.begin(PREF_NAMESPACE, true);  // read-only
    _webhookUrl = _preferences.getString(PREF_KEY_WEBHOOK, "");
    _displayMode = _preferences.getString(PREF_KEY_MODE, "cost");
    _preferences.end();

    log_i("Loaded prefs — webhook: %s, mode: %s",
          _webhookUrl.c_str(), _displayMode.c_str());
}

void NetworkManager::_savePreferences() {
    _preferences.begin(PREF_NAMESPACE, false);  // read-write
    _preferences.putString(PREF_KEY_WEBHOOK, _webhookUrl);
    _preferences.putString(PREF_KEY_MODE, _displayMode);
    _preferences.end();
}

void NetworkManager::_setupTLS() {
    // Use the ISRG Root X1 CA by default (covers Let's Encrypt)
    // For Anthropic direct API, Amazon Root CA 1 is needed
    _secureClient.setCACert(ROOT_CA_ISRG);

    // If webhook URL points to api.anthropic.com, switch to Amazon CA
    if (_webhookUrl.indexOf("anthropic.com") >= 0) {
        _secureClient.setCACert(ROOT_CA_AMAZON);
    }
}
