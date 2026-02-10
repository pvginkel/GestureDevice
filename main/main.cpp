#include "support.h"

#include "Application.h"

LOG_TAG(main);

extern "C" void app_main() {
#ifdef CONFIG_DEVICE_SHOW_CPU_USAGE
    show_task_statistics();
#endif

    Application application;

    application.begin();

    while (1) {
        application.process();

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
