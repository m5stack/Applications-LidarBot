#ifndef __LOCK_H
#define __LOCK_H

#undef min

static SemaphoreHandle_t xSemaphore = xSemaphoreCreateMutex();

#endif
