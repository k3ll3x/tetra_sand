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
#define SCALE 32767.0

#include <unistd.h>
#define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )

struct InputData {
    MY_TYPE* buffer;
    unsigned long bufferBytes;
    unsigned long totalFrames;
    unsigned long frameCounter;
    unsigned int channels;
};

struct OutputData {
    FILE *fd;
    unsigned int channels;
};


class audio {
    //audio();
    std::mutex idata_mutex;
    std::mutex odata_mutex;
    InputData idata;
    OutputData odata;
    RtAudio adc = RtAudio(RtAudio::Api::LINUX_ALSA);
    RtAudio dac = RtAudio(RtAudio::Api::LINUX_ALSA);
    std::thread record_thread;
    std::thread play_thread;
    std::thread save_raw_data_thread;

    const char* save_path = "misc/media/";

    void record_hndl(   unsigned int channels,
                        unsigned int fs,
                        double time,
                        unsigned int device,
                        unsigned int offset,
                        unsigned int bufferFrames);

    void play_hndl( const char* filename,
                    unsigned int channels,
                    unsigned int fs,
                    double time,
                    unsigned int device,
                    unsigned int offset,
                    unsigned int bufferFrames);

    void save_raw_data_hndl(const char* filename, unsigned int channels);

    // int input(  void* outputBuffer,
    //             void* inputBuffer,
    //             unsigned int nBufferFrames,
    //             double streamTime,
    //             unsigned int status,
    //             void* idata);

    // int output( void* outputBuffer,
    //             void* inputBuffer,
    //             unsigned int nBufferFrames,
    //             double streamTime,
    //             RtAudioStreamStatus status,
    //             void* odata);

public:
    ~audio();

    void record(unsigned int channels = 1,
                unsigned int fs = 44100,
                double time = 2.0,
                unsigned int device = 5,
                unsigned int offset = 0,
                unsigned int bufferFrames = 512);

    void play(  const char* filename,
                unsigned int channels = 1,
                unsigned int fs = 44100,
                double time = 2.0,
                unsigned int device = 1,
                unsigned int offset = 0,
                unsigned int bufferFrames = 512);
    
    void save_raw_data(const char* filename, unsigned int channels = 1);

    void input_cleanup();
    void output_cleanup();
};
