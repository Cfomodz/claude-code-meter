#ifndef PARSER_H
#define PARSER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "config.h"

// ============================================================================
// JSON Parser — ArduinoJson Stream Filtering for ESP32 Memory Constraints
// ============================================================================
//
// Two parsing modes:
//
// 1. n8n Webhook Response (lightweight):
//    {"cost_usd": 12.50, "trend": "up", "tokens_total": 1234567}
//
// 2. Direct Anthropic API Response (heavy, requires filtering):
//    {"data": [{"results": {"uncached_input_tokens": N, "output_tokens": N, ...}}]}
//
// The filter-based approach discards ~90% of the raw API payload before
// deserialization, keeping heap usage well within ESP32 limits.

// Token usage breakdown from the Anthropic API
struct TokenUsage {
    uint64_t uncachedInputTokens;
    uint64_t outputTokens;
    uint64_t cacheCreationTokens;
    uint64_t cacheReadTokens;
    uint64_t totalTokens;  // Computed sum
};

// Parsed meter data (common format for both data sources)
struct MeterData {
    bool valid;
    float costUsd;         // Total cost in USD
    String trend;          // "up", "down", "flat"
    TokenUsage tokens;
};

class Parser {
public:
    // Parse a lightweight n8n webhook response
    // Expected: {"cost_usd": 12.50, "trend": "up", "tokens_total": 1234567}
    static MeterData parseWebhookResponse(const String& json);

    // Parse a direct Anthropic API usage report response
    // Uses ArduinoJson filter to minimize RAM usage
    static MeterData parseAnthropicUsage(const String& json);

    // Compute cost from token counts using current model rates
    // Rates (per 1M tokens, as of 2025):
    //   Opus:   input=$15,  output=$75
    //   Sonnet: input=$3,   output=$15
    static float computeCost(const TokenUsage& usage, const char* model = "sonnet");

private:
    // Aggregate token fields across all data entries
    static TokenUsage _sumTokens(JsonArray dataArray);
};

#endif // PARSER_H
