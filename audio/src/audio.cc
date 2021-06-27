#include "audio.h"

audio::~audio(){
    audio::cleanup();
}

int audio::input(void* /*outputBuffer*/,
                void* inputBuffer,
                unsigned int nBufferFrames,
                double /*streamTime*/,
                unsigned int /*status*/,
                void* data){

    InputData* iData = (InputData*) data;

    // Simply copy the data to our allocated buffer.
    unsigned int frames = nBufferFrames;
    if ( iData->frameCounter + nBufferFrames > iData->totalFrames ) {
        frames = iData->totalFrames - iData->frameCounter;
        iData->bufferBytes = frames * iData->channels * sizeof( MY_TYPE );
    }

    unsigned long offset = iData->frameCounter * iData->channels;
    memcpy( iData->buffer+offset, inputBuffer, iData->bufferBytes );
    iData->frameCounter += frames;

    if ( iData->frameCounter >= iData->totalFrames ) return 2;
    return 0;
}

void audio::record( unsigned int channels,
                    unsigned int fs,
                    double time,
                    unsigned int device,
                    unsigned int offset){

    unsigned int bufferFrames = 0;
    if ( adc.getDeviceCount() < 1 ) {
        std::cout << "\nNo audio devices found!\n";
        return;
    }

    // Let RtAudio print messages to stderr.
    adc.showWarnings( true );

    // Set our stream parameters for input only.
    bufferFrames = 512;
    RtAudio::StreamParameters iParams;
    if ( device == 0 )
        iParams.deviceId = adc.getDefaultInputDevice();
    else
        iParams.deviceId = device;
    iParams.nChannels = channels;
    iParams.firstChannel = offset;

    data.buffer = 0;
    try {
        adc.openStream( NULL, &iParams, FORMAT, fs, &bufferFrames, (RtAudioCallback)&audio::input, (void*)&data );
    }
    catch ( RtAudioError& e ) {
        std::cout << '\n' << e.getMessage() << '\n' << std::endl;
        audio::cleanup();
    }
}

void audio::cleanup(){
    if ( adc.isStreamOpen() ) adc.closeStream();
    if ( data.buffer ) free( data.buffer );
}