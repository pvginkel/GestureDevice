#pragma once

#include "ApplicationBase.h"
#include "LDR.h"
#include "SEN0626.h"
#include "WS2812StatusLed.h"

class Application : public ApplicationBase {
    struct DeviceState {
        int lux;
    };

    DeviceState _state{};
    LDR _ldr_sensor;
    SEN0626 _device;
    map<string, size_t> _room_index;
    WS2812StatusLed _status_led;
    int _last_reported_gesture{};
    int64_t _last_reported_gesture_time{};

public:
    Application();

protected:
    void do_begin() override;
    void do_ready() override;
    void do_configuration_loaded(cJSON* data) override;
    void do_process() override;

private:
    void state_changed();
    void publish_mqtt_discovery();
    void handle_gesture(GestureEvent event);
    const char* get_gesture_name(int gesture_type);
};
