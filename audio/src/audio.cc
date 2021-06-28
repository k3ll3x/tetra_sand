#include "audio.h"

audio::~audio(){
    record_thread.join();
    save_raw_data_thread.join();
    audio::cleanup();
}

int input(void* /*outputBuffer*/,
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

void audio::record_hndl(unsigned int channels,
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

    std::lock_guard<std::mutex> data_guard(audio::data_mutex);
    data.buffer = 0;
    try {
        adc.openStream( NULL, &iParams, FORMAT, fs, &bufferFrames, (RtAudioCallback)&input, (void*)&data );
    }
    catch ( RtAudioError& e ) {
        std::cout << '\n' << e.getMessage() << '\n' << std::endl;
        audio::cleanup();
        return;
    }

    data.bufferBytes = bufferFrames * channels * sizeof( MY_TYPE );
    data.totalFrames = (unsigned long) (fs * time);
    data.frameCounter = 0;
    data.channels = channels;
    unsigned long totalBytes;
    totalBytes = data.totalFrames * channels * sizeof( MY_TYPE );

    // Allocate the entire data buffer before starting stream.
    data.buffer = (MY_TYPE*) malloc( totalBytes );
    if ( data.buffer == 0 ) {
        std::cout << "Memory allocation error ... quitting!\n";
        audio::cleanup();
        return;
    }

    try {
        adc.startStream();
    }
    catch ( RtAudioError& e ) {
        std::cout << '\n' << e.getMessage() << '\n' << std::endl;
        audio::cleanup();
        return;
    }

    std::cout << "\nRecording for " << time << " seconds ... (buffer frames = " << bufferFrames << ")." << std::endl;

    while ( adc.isStreamRunning() ) {
        SLEEP( 100 ); // wake every 100 ms to check if we're done
    }

    std::cout << "Recording finished\n";
    if ( adc.isStreamOpen() ) adc.closeStream();
}

void audio::record( unsigned int channels,
                    unsigned int fs,
                    double time,
                    unsigned int device,
                    unsigned int offset){
    if(!adc.isStreamRunning())
        if(record_thread.joinable())
            record_thread.join();
        record_thread = std::thread(&audio::record_hndl, this, channels, fs, time, device, offset);
}

void audio::save_raw_data_hndl(const char* filename, unsigned int channels){
    if(data.buffer){
        // Now write the entire data to the file.
        std::cout << "Saving raw data to " << filename << '\n';
        FILE* fd;
        fd = fopen( filename, "wb" );
        std::lock_guard<std::mutex> data_guard(data_mutex);
        fwrite( data.buffer, sizeof( MY_TYPE ), data.totalFrames * channels, fd );
        fclose( fd );
    }
}

void audio::save_raw_data(const char* filename, unsigned int channels){
    if(save_raw_data_thread.joinable())
        save_raw_data_thread.join();
    save_raw_data_thread = std::thread(&audio::save_raw_data_hndl, this, filename, channels);
}

void audio::cleanup(){
    if ( adc.isStreamOpen() ) adc.closeStream();
    if ( data.buffer ) free( data.buffer );
}