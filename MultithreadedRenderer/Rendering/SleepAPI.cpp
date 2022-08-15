#include "SleepAPI.hpp"
#include <Windows.h>

SleepAPI::SleepAPI() {
    timer = CreateWaitableTimer(NULL, TRUE, NULL);
}

SleepAPI::~SleepAPI() {
    CloseHandle(timer);
}

/* Windows sleep in accurate milliseconds */
void SleepAPI::millisleep(long long ms) {
    LARGE_INTEGER li;   /* Time defintion */

    /* Set timer properties */
    li.QuadPart = -ms * 10000;//convert to ns
    if (!SetWaitableTimer(timer, &li, 0, NULL, NULL, FALSE)) {
        CloseHandle(timer);
        return;
    }

    /* Start & wait for timer */
    WaitForSingleObject(timer, INFINITE);
}