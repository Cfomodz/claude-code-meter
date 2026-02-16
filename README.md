# Claude Code Meter

A physical desk meter that displays your Anthropic Claude API usage costs in real-time on a MAX7219 LED dot matrix, powered by an ESP32.

## How It Works

```
ESP32 + MAX7219  ◄── HTTPS poll ──  n8n Webhook  ── Admin API ──►  Anthropic
 (desk display)                     (middleware)                   (usage data)
```

The ESP32 polls an n8n webhook at a configurable interval (default 60s). The n8n workflow calls the Anthropic Admin API, computes cost from token counts, and returns a lightweight JSON payload. The ESP32 parses it and renders the cost (or token count) on the LED matrix.

The device **never stores your API key** — credentials are managed entirely by the n8n middleware layer.

## Hardware

| Component | Spec |
|-----------|------|
| MCU | ESP32-S3-DevKitC-1 or ESP32-WROOM-32 |
| Display | MAX7219 4-in-1 Dot Matrix (FC-16) |
| Level Shifter | 74HCT125 (3.3V → 5.0V for SPI) |
| Power | USB-C, 5V/2A with 1000µF bulk cap |

Full BOM, wiring diagram, enclosure specs, and n8n workflow setup are in [`firmware/HARDWARE.md`](firmware/HARDWARE.md).

## Firmware

Built with PlatformIO and Arduino framework.

### Dependencies

- **ArduinoJson** v7+ — stream-filtered JSON parsing
- **MD_Parola** — text animation on MAX7219
- **MD_MAX72XX** — hardware driver
- **WiFiManager** — captive portal provisioning

### Build & Flash

```bash
cd firmware

# ESP32-S3
pio run -e esp32s3 -t upload

# ESP32-WROOM-32
pio run -e esp32dev -t upload

# Serial monitor
pio device monitor -b 115200
```

## First Boot

1. Power on — display shows `CLAUDE` → `METER` → `WiFi`
2. Connect to the **ClaudeMeter_Setup** WiFi AP from your phone/laptop
3. Enter your WiFi credentials and n8n webhook URL in the captive portal
4. Device connects and begins polling

## Display Modes

- **Cost** — shows `$XX.XX` on the display (default)
- **Tokens** — shows total token count with K/M/B suffix

Mode is set during provisioning and stored persistently.

## Error Codes

| Display | Meaning |
|---------|---------|
| `E-WIFI` | WiFi disconnected |
| `E-TLS` | TLS handshake failed |
| `E-API` | 401/403 from upstream API |
| `E-JSON` | JSON parse error |
| `E-HTTP` | Non-200 HTTP response |

## Factory Reset

Hold the **BOOT** button (GPIO 0) for 5 seconds to clear all stored config and restart.

## License

MIT
