#include "audio.h"

audio::~audio(){
    record_thread.join();
    play_thread.join();
    save_raw_data_thread.join();
    audio::input_cleanup();
    audio::output_cleanup();
}

int /*audio::*/input(   void* outputBuffer,
                        void* inputBuffer,
                        unsigned int nBufferFrames,
                        double streamTime,
                        unsigned int status,
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

int /*audio::*/output(  void* outputBuffer,
                        void* inputBuffer,
                        unsigned int nBufferFrames,
                        double streamTime,
                        RtAudioStreamStatus status,
                        void* odata){
    OutputData *oData = (OutputData*) odata;

    // In general, it's not a good idea to do file input in the audio
    // callback function but I'm doing it here because I don't know the
    // length of the file we are reading.
    unsigned int count = fread( outputBuffer, oData->channels * sizeof( MY_TYPE ), nBufferFrames, oData->fd);
    if ( count < nBufferFrames ) {
        unsigned int bytes = (nBufferFrames - count) * oData->channels * sizeof( MY_TYPE );
        unsigned int startByte = count * oData->channels * sizeof( MY_TYPE );
        memset( (char *)(outputBuffer)+startByte, 0, bytes );
        return 1;
    }
    return 0;
}

void audio::record_hndl(unsigned int channels,
                        unsigned int fs,
                        double time,
                        unsigned int device,
                        unsigned int offset,
                        unsigned int bufferFrames){
    if ( adc.getDeviceCount() < 1 ) {
        std::cout << "\nNo audio devices found!\n";
        return;
    }

    // Let RtAudio print messages to stderr.
    adc.showWarnings( true );

    // Set our stream parameters for input only.
    RtAudio::StreamParameters iParams;
    if ( device == 0 )
        iParams.deviceId = adc.getDefaultInputDevice();
    else
        iParams.deviceId = device;
    iParams.nChannels = channels;
    iParams.firstChannel = offset;

    std::lock_guard<std::mutex> idata_guard(audio::idata_mutex);
    idata.buffer = 0;
    try {
        // adc.openStream( NULL, &iParams, FORMAT, fs, &bufferFrames, (RtAudioCallback)&audio::input, (void*)&idata );
        adc.openStream( NULL, &iParams, FORMAT, fs, &bufferFrames, (RtAudioCallback)&input, (void*)&idata );
    }
    catch ( RtAudioError& e ) {
        std::cout << '\n' << e.getMessage() << '\n' << std::endl;
        audio::input_cleanup();
        return;
    }

    idata.bufferBytes = bufferFrames * channels * sizeof( MY_TYPE );
    idata.totalFrames = (unsigned long) (fs * time);
    idata.frameCounter = 0;
    idata.channels = channels;
    unsigned long totalBytes;
    totalBytes = idata.totalFrames * channels * sizeof( MY_TYPE );

    // Allocate the entire data buffer before starting stream.
    idata.buffer = (MY_TYPE*) malloc( totalBytes );
    if ( idata.buffer == 0 ) {
        std::cout << "Memory allocation error ... quitting!\n";
        audio::input_cleanup();
        return;
    }

    try {
        adc.startStream();
    }
    catch ( RtAudioError& e ) {
        std::cout << '\n' << e.getMessage() << '\n' << std::endl;
        audio::input_cleanup();
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
                    unsigned int offset,
                    unsigned int bufferFrames){
    if(!adc.isStreamRunning())
        if(record_thread.joinable())
            record_thread.join();
        record_thread = std::thread(&audio::record_hndl, this, channels, fs, time, device, offset, bufferFrames);
}

void audio::play_hndl(  const char* filename,
                        unsigned int channels,
                        unsigned int fs,
                        double time,
                        unsigned int device,
                        unsigned int offset,
                        unsigned int bufferFrames){
    std::cout << "need to debug this later " << dac.getDeviceCount() << '\n';
    if(dac.getDeviceCount() < 1){
        std::cout << "\nNo audio devices found!\n";
        audio::output_cleanup();
        return;
    }

    std::lock_guard<std::mutex> odata_guard(audio::odata_mutex);
    std::string nfile(audio::save_path);
    nfile += filename;
    odata.fd = fopen(nfile.c_str(), "rb");
    if(!odata.fd){
        std::cout << "Unable to find or open file!\n";
        audio::output_cleanup();
        return;
    }

    //Stream params for output only
    RtAudio::StreamParameters oParams;
    oParams.deviceId = device;
    oParams.nChannels = channels;
    oParams.firstChannel = offset;

    if(device == 0){
        oParams.deviceId = dac.getDefaultOutputDevice();
    }

    odata.channels = channels;
    
    try{
        // dac.openStream(&oParams, NULL, FORMAT, fs, &bufferFrames, (RtAudioCallback)&audio::output, (void*)&odata);
        std::cout << "Open and start stream\n";
        dac.openStream(&oParams, NULL, FORMAT, fs, &bufferFrames, (RtAudioCallback)&output, (void*)&odata);
        dac.startStream();
    }catch(RtAudioError& e){
        std::cout << '\n' << e.getMessage() << '\n';
        audio::output_cleanup();
    }

    std::cout << "\nPlaying raw file " << nfile << "(buffer frames = " << bufferFrames << ").\n";
    while(dac.isStreamRunning()){
        SLEEP(100);
    }

    std::cout << "Playback finished\n";
    if ( dac.isStreamOpen() ) dac.closeStream();
}

void audio::play(   const char* filename,
                    unsigned int channels,
                    unsigned int fs,
                    double time,
                    unsigned int device,
                    unsigned int offset,
                    unsigned int bufferFrames){
    if(!dac.isStreamRunning())
        if(play_thread.joinable())
            play_thread.join();
        play_thread = std::thread(&audio::play_hndl, this, filename, channels, fs, time, device, offset, bufferFrames);
}

void audio::save_raw_data_hndl(const char* filename, unsigned int channels){
    if(idata.buffer){
        // Now write the entire data to the file.
        std::string nfile(audio::save_path);
        nfile += filename;
        std::cout << "Saving raw data to " << nfile << '\n';
        FILE* fd;
        fd = fopen( nfile.c_str(), "wb" );
        std::lock_guard<std::mutex> idata_guard(idata_mutex);
        fwrite( idata.buffer, sizeof( MY_TYPE ), idata.totalFrames * channels, fd );
        fclose( fd );
    }
}

void audio::save_raw_data(const char* filename, unsigned int channels){
    if(save_raw_data_thread.joinable())
        save_raw_data_thread.join();
    save_raw_data_thread = std::thread(&audio::save_raw_data_hndl, this, filename, channels);
}

void audio::input_cleanup(){
    if ( adc.isStreamOpen() ) adc.closeStream();
    if ( idata.buffer ) free( idata.buffer );
}

void audio::output_cleanup(){
    if(odata.fd) fclose(odata.fd);
    if(dac.isStreamOpen()) dac.closeStream();
}