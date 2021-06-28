#pragma once

#include "rtaudio/RtAudio.h"

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <stdio.h>
#include <thread>
#include <mutex>

typedef signed short MY_TYPE;
#define FORMAT RTAUDIO_SINT16

#include <unistd.h>
#define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )

struct InputData {
    MY_TYPE* buffer;
    unsigned long bufferBytes;
    unsigned long totalFrames;
    unsigned long frameCounter;
    unsigned int channels;
};

class audio {
    //audio();
    std::mutex data_mutex;
    InputData data;
    RtAudio adc = RtAudio(RtAudio::Api::LINUX_ALSA);
    std::thread record_thread;
    std::thread save_raw_data_thread;

public:
    ~audio();
    void record(unsigned int channels = 1,
                unsigned int fs = 44100,
                double time = 2.0,
                unsigned int device = 5,
                unsigned int offset = 0);

    void record_hndl(   unsigned int channels,
                        unsigned int fs,
                        double time,
                        unsigned int device,
                        unsigned int offset);
    
    void save_raw_data(const char* filename, unsigned int channels = 1);

    void save_raw_data_hndl(const char* filename, unsigned int channels);

    void cleanup();
};