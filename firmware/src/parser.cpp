#include "parser.h"

// ============================================================================
// JSON Parser Implementation
// ============================================================================

MeterData Parser::parseWebhookResponse(const String& json) {
    MeterData data = { false, 0.0f, "flat", {0, 0, 0, 0, 0} };

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, json);

    if (err) {
        log_e("Webhook JSON parse error: %s", err.c_str());
        return data;
    }

    data.valid = true;
    data.costUsd = doc["cost_usd"] | 0.0f;
    data.trend = doc["trend"] | "flat";

    // Optional token fields from n8n
    data.tokens.totalTokens = doc["tokens_total"] | (uint64_t)0;
    data.tokens.uncachedInputTokens = doc["uncached_input_tokens"] | (uint64_t)0;
    data.tokens.outputTokens = doc["output_tokens"] | (uint64_t)0;
    data.tokens.cacheCreationTokens = doc["cache_creation_input_tokens"] | (uint64_t)0;
    data.tokens.cacheReadTokens = doc["cache_read_input_tokens"] | (uint64_t)0;

    // If total wasn't provided but individual fields were, compute it
    if (data.tokens.totalTokens == 0) {
        data.tokens.totalTokens =
            data.tokens.uncachedInputTokens +
            data.tokens.outputTokens +
            data.tokens.cacheCreationTokens +
            data.tokens.cacheReadTokens;
    }

    return data;
}

MeterData Parser::parseAnthropicUsage(const String& json) {
    MeterData data = { false, 0.0f, "flat", {0, 0, 0, 0, 0} };

    // Build a filter document to extract only the token fields we need.
    // This discards ~90% of the API response payload before deserialization,
    // critical for staying within ESP32 heap limits.
    JsonDocument filter;
    filter["data"][0]["results"]["uncached_input_tokens"] = true;
    filter["data"][0]["results"]["output_tokens"] = true;
    filter["data"][0]["results"]["cache_creation_input_tokens"] = true;
    filter["data"][0]["results"]["cache_read_input_tokens"] = true;

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, json,
        DeserializationOption::Filter(filter));

    if (err) {
        log_e("Anthropic JSON parse error: %s", err.c_str());
        return data;
    }

    JsonArray dataArray = doc["data"].as<JsonArray>();
    if (dataArray.isNull() || dataArray.size() == 0) {
        log_e("Anthropic response: empty 'data' array");
        return data;
    }

    data.tokens = _sumTokens(dataArray);
    data.costUsd = computeCost(data.tokens);
    data.valid = true;

    return data;
}

float Parser::computeCost(const TokenUsage& usage, const char* model) {
    // Rates per token (derived from per-1M-token pricing)
    float inputRate, outputRate, cacheWriteRate, cacheReadRate;

    if (strcmp(model, "opus") == 0) {
        inputRate      = 15.0f  / 1000000.0f;  // $15/1M
        outputRate     = 75.0f  / 1000000.0f;  // $75/1M
        cacheWriteRate = 18.75f / 1000000.0f;  // $18.75/1M
        cacheReadRate  = 1.50f  / 1000000.0f;  // $1.50/1M
    } else {
        // Default: Sonnet rates
        inputRate      = 3.0f  / 1000000.0f;   // $3/1M
        outputRate     = 15.0f / 1000000.0f;    // $15/1M
        cacheWriteRate = 3.75f / 1000000.0f;    // $3.75/1M
        cacheReadRate  = 0.30f / 1000000.0f;    // $0.30/1M
    }

    float cost =
        (float)usage.uncachedInputTokens   * inputRate +
        (float)usage.outputTokens          * outputRate +
        (float)usage.cacheCreationTokens   * cacheWriteRate +
        (float)usage.cacheReadTokens       * cacheReadRate;

    return cost;
}

TokenUsage Parser::_sumTokens(JsonArray dataArray) {
    TokenUsage total = {0, 0, 0, 0, 0};

    for (JsonObject entry : dataArray) {
        JsonObject results = entry["results"];
        if (results.isNull()) continue;

        total.uncachedInputTokens  += results["uncached_input_tokens"] | (uint64_t)0;
        total.outputTokens         += results["output_tokens"] | (uint64_t)0;
        total.cacheCreationTokens  += results["cache_creation_input_tokens"] | (uint64_t)0;
        total.cacheReadTokens      += results["cache_read_input_tokens"] | (uint64_t)0;
    }

    total.totalTokens =
        total.uncachedInputTokens +
        total.outputTokens +
        total.cacheCreationTokens +
        total.cacheReadTokens;

    return total;
}
