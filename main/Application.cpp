#include "support.h"

#include "Application.h"

#include "MQTTSupport.h"
#include "NVSProperty.h"
#include "driver/i2c.h"
#include "nvs_flash.h"

#define LDR_DIVIDER_RESISTOR 20'000.0f
#define LDR_RESISTANCE_AT_10_LUX 25'000.0f
#define LDR_GAMMA 0.75f

#define GESTURE_REPORT_DEBOUNCE_MS 2000

LOG_TAG(Application);

Application::Application()
    : _ldr_sensor(&get_queue(), LDR_DIVIDER_RESISTOR, LDR_RESISTANCE_AT_10_LUX, LDR_GAMMA), _device(&get_queue()) {}

void Application::do_begin() {
    _status_led.begin();

    _status_led.set_color(Colors::Blue);
    _status_led.set_mode(StatusLedMode::Blinking, 400);

    get_mqtt_connection().on_publish_discovery([this]() { publish_mqtt_discovery(); });

    get_mqtt_connection().on_connected_changed([this](auto state) {
        if (state.connected) {
            state_changed();
        }
    });
}

void Application::do_configuration_loaded(cJSON* data) { _status_led.set_color(Colors::Green); }

void Application::do_ready() {
    _status_led.set_color(Colors::Green);
    _status_led.set_mode(StatusLedMode::Continuous, 3000);

    ESP_LOGI(TAG, "Startup complete");

    _device.on_gesture_detected([this](auto event) { handle_gesture(event); });

    ESP_ERROR_CHECK(_device.begin(CONFIG_DEVICE_SDA_PIN, CONFIG_DEVICE_SCL_PIN, CONFIG_DEVICE_INT_PIN));

    _ldr_sensor.on_lux_changed([this](auto lux) {
        if (_state.lux != int(lux)) {
            _state.lux = int(lux);

            state_changed();
        }
    });

    ESP_ERROR_CHECK(_ldr_sensor.begin(CONFIG_DEVICE_LDR_PIN, CONFIG_DEVICE_LDR_SENSOR_REPORT_INTERVAL_MS));
}

void Application::do_process() { _status_led.process(); }

void Application::state_changed() {
    if (!get_mqtt_connection().is_connected()) {
        return;
    }

    // Signal activity.
    if (!_status_led.is_active()) {
        _status_led.set_mode(StatusLedMode::Continuous, 1);
    }

    const auto json = cJSON_CreateObject();
    ESP_ASSERT_CHECK(json);
    DEFER(cJSON_Delete(json));

    cJSON_AddNumberToObject(json, "lux", _state.lux);

    get_mqtt_connection().send_state(json);
}

void Application::publish_mqtt_discovery() {
    get_mqtt_connection().publish_button_discovery(
        {
            .name = "Identify",
            .object_id = "identify",
            .entity_category = "config",
            .device_class = "identify",
        },
        []() { ESP_LOGI(TAG, "Requested identification"); });

    get_mqtt_connection().publish_button_discovery(
        {
            .name = "Restart",
            .object_id = "restart",
            .entity_category = "config",
            .device_class = "restart",
        },
        []() {
            ESP_LOGI(TAG, "Requested restart");

            esp_restart();
        });

    get_mqtt_connection().publish_sensor_discovery(
        {
            .name = "Illuminance",
            .object_id = "illuminance",
            .device_class = "illuminance",
        },
        {
            .state_class = "measurement",
            .unit_of_measurement = "lx",
            .value_template = "{{ value_json.lux }}",
        });

    for (int i = 1; i <= 5; i++) {
        get_mqtt_connection().publish_device_automation({
            .trigger_name = "gesture",
            .trigger_value = get_gesture_name(i),
        });
    }
}

void Application::handle_gesture(GestureEvent event) {
    ESP_LOGI(TAG, "Gesture detected: type=0x%04X score=%d", event.type, event.score);

    const auto now = esp_get_millis();
    if (now > _last_reported_gesture_time + GESTURE_REPORT_DEBOUNCE_MS) {
        _last_reported_gesture = 0;
    }

    _last_reported_gesture_time = now;

    if (event.type != _last_reported_gesture) {
        _last_reported_gesture = event.type;

        const auto gesture_name = get_gesture_name(event.type);
        if (gesture_name) {
            get_mqtt_connection().send_trigger("gesture", gesture_name);
        }
    }
}

const char* Application::get_gesture_name(int gesture_type) {
    switch (gesture_type) {
        case 1:
            return "good";
        case 2:
            return "ok";
        case 3:
            return "stop";
        case 4:
            return "victory";
        case 5:
            return "call_me";
        default:
            ESP_LOGE(TAG, "Invalid gesture type %d", gesture_type);
            return nullptr;
    }
}
