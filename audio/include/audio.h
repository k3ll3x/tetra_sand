#pragma once

#include "rtaudio/RtAudio.h"

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <stdio.h>

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
    InputData data;
    RtAudio adc;

    // Interleaved buffers
    int input(  void* /*outputBuffer*/,
                void* inputBuffer,
                unsigned int nBufferFrames,
                double /*streamTime*/,
                unsigned int /*status*/,
                void* data);

public:
    ~audio();
    void record(unsigned int channels = 1,
                unsigned int fs = 44100,
                double time = 2.0,
                unsigned int device = 0,
                unsigned int offset = 0);
    
    void cleanup();
};