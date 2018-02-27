#ifndef STREAM_DEV_H
#define STREAM_DEV_H

#include "typedef.h"

enum {
    STREAM_DEV_STANDBY,
    STREAM_DEV_PLAYING,
    STREAM_DEV_PENDING,
    STREAM_DEV_STOP,
};

enum {
    STREAM_OUTPUT = 0,
    STREAM_INPUT,
};

#define STREAM_BUFF_SIZE            512


void app_stream_dev_init();

void stream_dev_output(void *priv, void *pcm_buf, u16 len);

void stream_dev_input(void *priv, void *pcm_buf, u16 len);

void stream_dev_ioctrl(int ctrl, void *priv, ...);

#endif

