#include "display.h"

// ============================================================================
// Display Manager Implementation
// ============================================================================

DisplayManager::DisplayManager()
    : _parola(HARDWARE_TYPE, PIN_SPI_CS, DISPLAY_NUM_DEVICES),
      _isError(false),
      _errorBlinkTimer(0),
      _errorVisible(true)
{
    memset(_scrollBuf, 0, sizeof(_scrollBuf));
    memset(_staticBuf, 0, sizeof(_staticBuf));
}

void DisplayManager::begin() {
    _parola.begin();
    _parola.setIntensity(DISPLAY_BRIGHTNESS);
    _parola.setTextAlignment(PA_CENTER);
    _parola.setSpeed(SCROLL_SPEED_MS);
    _parola.setPause(SCROLL_PAUSE_MS);
    _parola.displayClear();
}

void DisplayManager::update() {
    // Handle error blink state
    if (_isError) {
        unsigned long now = millis();
        if (now - _errorBlinkTimer >= 500) {
            _errorBlinkTimer = now;
            _errorVisible = !_errorVisible;
            if (_errorVisible) {
                _parola.setTextAlignment(PA_CENTER);
                _parola.print(_staticBuf);
            } else {
                _parola.displayClear();
            }
        }
        return;
    }

    // Normal Parola animation tick
    if (_parola.displayAnimate()) {
        _parola.displayReset();
    }
}

void DisplayManager::showStatic(const char* text) {
    _isError = false;
    strncpy(_staticBuf, text, sizeof(_staticBuf) - 1);
    _staticBuf[sizeof(_staticBuf) - 1] = '\0';
    _parola.displayClear();
    _parola.setTextAlignment(PA_CENTER);
    _parola.print(_staticBuf);
}

void DisplayManager::showScrolling(const char* text) {
    _isError = false;
    strncpy(_scrollBuf, text, sizeof(_scrollBuf) - 1);
    _scrollBuf[sizeof(_scrollBuf) - 1] = '\0';
    _parola.displayClear();
    _parola.displayText(_scrollBuf, PA_LEFT, SCROLL_SPEED_MS, SCROLL_PAUSE_MS,
                        PA_SCROLL_LEFT, PA_SCROLL_LEFT);
}

void DisplayManager::showError(const char* errorCode) {
    _isError = true;
    _errorVisible = true;
    _errorBlinkTimer = millis();
    strncpy(_staticBuf, errorCode, sizeof(_staticBuf) - 1);
    _staticBuf[sizeof(_staticBuf) - 1] = '\0';
    _parola.displayClear();
    _parola.setTextAlignment(PA_CENTER);
    _parola.print(_staticBuf);
}

void DisplayManager::showCost(float costUsd) {
    _isError = false;
    // Format as "$XX.XX" — fits on 4-module display for values under $1000
    if (costUsd < 100.0f) {
        snprintf(_staticBuf, sizeof(_staticBuf), "%s%.2f", COST_PREFIX, costUsd);
        _parola.displayClear();
        _parola.setTextAlignment(PA_CENTER);
        _parola.print(_staticBuf);
    } else if (costUsd < 10000.0f) {
        // For larger values, drop decimals
        snprintf(_staticBuf, sizeof(_staticBuf), "%s%.0f", COST_PREFIX, costUsd);
        _parola.displayClear();
        _parola.setTextAlignment(PA_CENTER);
        _parola.print(_staticBuf);
    } else {
        // Scroll very large values
        snprintf(_scrollBuf, sizeof(_scrollBuf), "%s%.0f", COST_PREFIX, costUsd);
        _parola.displayClear();
        _parola.displayText(_scrollBuf, PA_LEFT, SCROLL_SPEED_MS, SCROLL_PAUSE_MS,
                            PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    }
}

void DisplayManager::showTokens(uint64_t tokens) {
    _isError = false;
    _formatCompact(tokens, _staticBuf, sizeof(_staticBuf));

    // Short enough to display statically
    if (strlen(_staticBuf) <= 8) {
        _parola.displayClear();
        _parola.setTextAlignment(PA_CENTER);
        _parola.print(_staticBuf);
    } else {
        strncpy(_scrollBuf, _staticBuf, sizeof(_scrollBuf) - 1);
        _scrollBuf[sizeof(_scrollBuf) - 1] = '\0';
        _parola.displayClear();
        _parola.displayText(_scrollBuf, PA_LEFT, SCROLL_SPEED_MS, SCROLL_PAUSE_MS,
                            PA_SCROLL_LEFT, PA_SCROLL_LEFT);
    }
}

void DisplayManager::showBootAnimation() {
    // Simple sweep animation: light each column left-to-right then display name
    _parola.displayClear();
    _parola.setTextAlignment(PA_CENTER);
    _parola.print("CLAUDE");
    delay(1200);
    _parola.displayClear();
    _parola.print("METER");
    delay(800);
    _parola.displayClear();
}

void DisplayManager::setBrightness(uint8_t level) {
    if (level > 15) level = 15;
    _parola.setIntensity(level);
}

void DisplayManager::_formatCompact(uint64_t value, char* buf, size_t bufSize) {
    if (value >= 1000000000ULL) {
        snprintf(buf, bufSize, "%.1fB", (double)value / 1000000000.0);
    } else if (value >= 1000000ULL) {
        snprintf(buf, bufSize, "%.1fM", (double)value / 1000000.0);
    } else if (value >= 1000ULL) {
        snprintf(buf, bufSize, "%.1fK", (double)value / 1000.0);
    } else {
        snprintf(buf, bufSize, "%llu", (unsigned long long)value);
    }
}
