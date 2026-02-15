#ifndef DISPLAY_H
#define DISPLAY_H

#include <MD_Parola.h>
#include <MD_MAX72XX.h>
#include <SPI.h>
#include "config.h"

// ============================================================================
// Display Manager — MAX7219 4-in-1 Dot Matrix via MD_Parola
// ============================================================================
//
// Handles text scrolling, static display, and error code presentation.
// Uses MD_Parola for smooth text animation and sprite effects.

class DisplayManager {
public:
    DisplayManager();

    // Initialize hardware and set default brightness
    void begin();

    // Run the display animation state machine. Call every loop().
    void update();

    // Show a static (non-scrolling) message centered on the display
    void showStatic(const char* text);

    // Show a scrolling message (for long text like cost breakdowns)
    void showScrolling(const char* text);

    // Show an error code (e.g. "E-WIFI"). Blinks to draw attention.
    void showError(const char* errorCode);

    // Show cost value formatted as "$XX.XX"
    void showCost(float costUsd);

    // Show token count with K/M suffix (e.g. "1.2M")
    void showTokens(uint64_t tokens);

    // Show a brief startup animation
    void showBootAnimation();

    // Set brightness (0–15)
    void setBrightness(uint8_t level);

private:
    MD_Parola _parola;
    char _scrollBuf[128];  // Buffer for scrolling text
    char _staticBuf[32];   // Buffer for static text
    bool _isError;
    unsigned long _errorBlinkTimer;
    bool _errorVisible;

    // Format large numbers with K/M suffix
    void _formatCompact(uint64_t value, char* buf, size_t bufSize);
};

#endif // DISPLAY_H
