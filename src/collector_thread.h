#ifndef COLLECTOR_THREAD_H_
#define COLLECTOR_THREAD_H_

#include "global_thread_vars.h"
#include "log.h"

void sensor_collect_thread_loop()
{
    Timer timer;

    while (true)
    {
        timer.reset();
        timer.start();

        // Enter critical section
        g_sensors_lock.lock();
        g_sens_collector.collect();
        timer.stop();
        g_sensors_lock.unlock();
        // Exit critical section

        log_info("Got %d samples in %dms!\n", SAMPLE_MATRIX_ELEMENT_SIZE, timer.read_ms());
    }
}

#endif // COLLECTOR_THREAD_H_