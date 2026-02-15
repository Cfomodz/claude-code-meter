# Claude Code Meter — Hardware Build Guide

Physical ESP32 + MAX7219 desk meter for monitoring Anthropic Claude API usage costs in real-time.

## Architecture

```
┌──────────────────┐     HTTPS/TLS      ┌──────────────────┐     Admin API      ┌──────────────┐
│   ESP32 + MAX7219 │ ◄──────────────── │   n8n Middleware   │ ──────────────── │  Anthropic    │
│   (Edge Device)   │   Webhook Poll     │   (Control Plane)  │   sk-ant-admin   │  Usage API    │
└──────────────────┘                     └──────────────────┘                    └──────────────┘
        │                                         │
  Display cost/tokens                    Secrets management
  Error codes on LED                     JSON parsing & cost calc
  WiFi provisioning                      Currency conversion
```

**Security Model:** The ESP32 never stores the `sk-ant-admin` key. All credential management is handled by the n8n middleware layer.

## Bill of Materials

| # | Component | Specification | Qty | Notes |
|---|-----------|---------------|-----|-------|
| 1 | MCU | ESP32-S3-DevKitC-1 or ESP32-WROOM-32 DevKit | 1 | S3 preferred for native USB-C |
| 2 | Display | MAX7219 Dot Matrix Module (4-in-1, FC-16) | 1 | 32x8 LED matrix, daisy-chainable |
| 3 | Level Shifter | 74HCT125 Quad Bus Buffer | 1 | 3.3V→5.0V for SPI signals |
| 4 | Capacitor | 1000µF 10V Electrolytic | 1 | Across 5V/GND — prevents brownouts |
| 5 | Capacitor | 100nF Ceramic | 1 | Decoupling on MAX7219 VCC |
| 6 | Power | USB-C cable + 5V/2A adapter | 1 | Powers both ESP32 and display |
| 7 | Wire | 24AWG silicone hookup wire (6 colors) | 1 set | For SPI + power connections |
| 8 | Case | 3D printed PLA enclosure | 1 | See Enclosure section below |

**Optional but recommended:**
- Smoked acrylic or red transparency film (improves LED contrast in office lighting)
- M3 heat-set inserts (4x) for PCB mounting
- Thermal pad for ESP32 LDO regulator

## Wiring Diagram

### Pinout: ESP32 → 74HCT125 → MAX7219

```
ESP32                    74HCT125                  MAX7219 (4-in-1)
┌──────────┐            ┌──────────┐              ┌──────────────────┐
│          │            │          │              │                  │
│  GPIO 23 ├───────────►│ 1A → 1Y ├─────────────►│ DIN  (Data In)   │
│  (MOSI)  │            │          │              │                  │
│          │            │          │              │                  │
│  GPIO  5 ├───────────►│ 2A → 2Y ├─────────────►│ CS   (Chip Sel)  │
│  (CS)    │            │          │              │                  │
│          │            │          │              │                  │
│  GPIO 18 ├───────────►│ 3A → 3Y ├─────────────►│ CLK  (Clock)     │
│  (SCK)   │            │          │              │                  │
│          │            │          │              │                  │
│  5V/VBUS ├────┬──────►│ VCC      ├──────┬──────►│ VCC  (5V Power)  │
│          │    │       │          │      │       │                  │
│  GND     ├────┼──────►│ GND      ├──────┼──────►│ GND              │
│          │    │       │  1OE,2OE │      │       │                  │
└──────────┘    │       │  3OE,4OE ├►GND  │       └──────────────────┘
                │       └──────────┘      │
                │                         │
                └───┤├────────────────────┘
              1000µF + 100nF caps
```

### 74HCT125 Pin Detail

| Pin | Connection | Purpose |
|-----|-----------|---------|
| 1OE (pin 1) | GND | Enable buffer channel 1 (active low) |
| 1A (pin 2) | ESP32 GPIO 23 | Input: MOSI at 3.3V |
| 1Y (pin 3) | MAX7219 DIN | Output: MOSI at 5.0V |
| 2OE (pin 4) | GND | Enable buffer channel 2 |
| 2A (pin 5) | ESP32 GPIO 5 | Input: CS at 3.3V |
| 2Y (pin 6) | MAX7219 CS | Output: CS at 5.0V |
| GND (pin 7) | GND | Ground |
| 3Y (pin 8) | MAX7219 CLK | Output: SCK at 5.0V |
| 3A (pin 9) | ESP32 GPIO 18 | Input: SCK at 3.3V |
| 3OE (pin 10) | GND | Enable buffer channel 3 |
| 4OE (pin 11) | GND | Enable unused channel 4 |
| 4A (pin 12) | GND | Unused input tied low |
| 4Y (pin 13) | NC | Unused output |
| VCC (pin 14) | 5V | Buffer supply voltage |

> **Why a level shifter?** The MAX7219 specifies V_IH (input high) at 3.5V minimum. The ESP32 outputs 3.3V, which is below this threshold. Direct wiring appears to work on the bench but causes display artifacts, missed commands, and unreliable operation — especially at higher SPI clock speeds or elevated temperatures.

## Enclosure Design

### Design Parameters
- **Aesthetic:** Retro-industrial / cyberdeck
- **Orientation:** 45-degree tilt for desktop visibility
- **Material:** PLA (matte black or dark grey)
- **Print Settings:** 0.2mm layer height, 20% infill, 3 walls

### Features
- Slot for MAX7219 display module with friction fit
- ESP32 mounting posts with M3 heat-set insert holes
- Rear channel for USB-C cable routing
- Optional front slot for smoked acrylic contrast filter
- Ventilation slots on bottom for ESP32 heat dissipation

### Assembly

1. Print main chassis and top cover
2. Press M3 heat-set inserts into mounting posts (soldering iron at 220°C)
3. Mount ESP32 with M3x6 screws
4. Solder wiring harness: ESP32 → 74HCT125 → MAX7219
5. Seat MAX7219 module in display slot
6. Apply thermal pad between ESP32 LDO and chassis wall (if using WROOM with voltage regulator)
7. Route USB-C cable through rear channel
8. Snap or screw top cover into place
9. (Optional) Insert smoked acrylic filter in front slot

## n8n Webhook Workflow

The recommended data pipeline offloads API calls and credential management to a self-hosted n8n instance.

### Workflow Setup

1. **Webhook Node** (Trigger)
   - Method: `GET`
   - Path: `/claude-meter`
   - Response Mode: "Last Node"

2. **HTTP Request Node**
   - URL: `https://api.anthropic.com/v1/organizations/usage_report/messages`
   - Method: `GET`
   - Headers:
     - `x-api-key`: `{{ $credentials.anthropicAdminKey }}`
     - `anthropic-version`: `2023-06-01`
   - Query Parameters:
     - `grouping`: `none`
     - `start_date`: `{{ $now.minus({hours: 24}).toISO().split('T')[0] }}`
     - `end_date`: `{{ $now.toISO().split('T')[0] }}`

3. **Code Node** (Transform)
   ```javascript
   const data = $input.first().json.data || [];
   let totalInput = 0, totalOutput = 0, totalCacheWrite = 0, totalCacheRead = 0;

   for (const entry of data) {
     const r = entry.results || {};
     totalInput      += r.uncached_input_tokens || 0;
     totalOutput     += r.output_tokens || 0;
     totalCacheWrite += r.cache_creation_input_tokens || 0;
     totalCacheRead  += r.cache_read_input_tokens || 0;
   }

   // Sonnet rates (per 1M tokens)
   const cost =
     (totalInput * 3.0 / 1e6) +
     (totalOutput * 15.0 / 1e6) +
     (totalCacheWrite * 3.75 / 1e6) +
     (totalCacheRead * 0.30 / 1e6);

   return [{
     json: {
       cost_usd: Math.round(cost * 100) / 100,
       trend: "flat",
       tokens_total: totalInput + totalOutput + totalCacheWrite + totalCacheRead,
       uncached_input_tokens: totalInput,
       output_tokens: totalOutput,
       cache_creation_input_tokens: totalCacheWrite,
       cache_read_input_tokens: totalCacheRead
     }
   }];
   ```

4. **Respond to Webhook Node**
   - Response Body: `{{ $json }}`

### ESP32 Receives

```json
{
  "cost_usd": 12.50,
  "trend": "flat",
  "tokens_total": 1234567,
  "uncached_input_tokens": 500000,
  "output_tokens": 600000,
  "cache_creation_input_tokens": 100000,
  "cache_read_input_tokens": 34567
}
```

## First Boot / Provisioning

1. Power on the ESP32 — display shows `CLAUDE` → `METER` → `WiFi`
2. If no WiFi credentials are stored, the device creates AP: **ClaudeMeter_Setup**
3. Connect to this AP from your phone/laptop
4. A captive portal opens automatically
5. Enter your WiFi credentials and n8n webhook URL
6. Device connects to WiFi and begins polling

## Error Codes

| Display | Meaning | Resolution |
|---------|---------|------------|
| `E-WIFI` | WiFi disconnected | Check router, move device closer to AP |
| `E-TLS` | TLS handshake failed | Verify webhook uses valid SSL cert; check root CA |
| `E-API` | 401/403 from API | Check n8n credentials; verify API key is valid |
| `E-JSON` | JSON parse error | Check n8n workflow output format |
| `E-HTTP` | Non-200 HTTP response | Check n8n webhook URL; verify n8n is running |

## Factory Reset

Hold the **BOOT** button (GPIO 0) for 5 seconds. The display will show `RESET...` and the device will restart with cleared WiFi and webhook configuration.

## Flashing Firmware

### Prerequisites
- [PlatformIO CLI](https://platformio.org/install/cli) or PlatformIO IDE extension for VS Code

### Build & Upload

```bash
cd firmware

# For ESP32-S3
pio run -e esp32s3 -t upload

# For ESP32-WROOM-32
pio run -e esp32dev -t upload

# Monitor serial output
pio device monitor -b 115200
```
