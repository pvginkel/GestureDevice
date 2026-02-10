#pragma once

#define _USE_MATH_DEFINES

using namespace std;

#include "support.h"

#include <ctype.h>
#include <math.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/unistd.h>

#include <string>

#include "cJSON.h"
#include "defer.h"
#include "error.h"
#include "esp_check.h"
#include "esp_err.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include "strformat.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define esp_get_millis() uint32_t(esp_timer_get_time() / 1000ull)

#define ESP_TIMER_MS(v) ((v) * 1000)
#define ESP_TIMER_SECONDS(v) ESP_TIMER_MS((v) * 1000)

#define LOG_TAG(v) [[maybe_unused]] static const char* TAG = #v
