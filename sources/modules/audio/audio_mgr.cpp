#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>
#include <experimental/filesystem>

#include <sndfile.hh>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/format.hpp>

#include "audio_mgr.h"

Audio_Manager::Audio_Manager()
{
    std::cout << "Audio_Manager" << std::endl;
    this->stream = nullptr;
    init_audio_mgr();

    load_music_playlist("/home/pi/projects/speech_to_text/my_resources/music/wav_16k");
    Audio_Manager::user_data.load_next_track = [this]()
    {
        this->music_play_idx = (this->music_play_idx < playlist.size()) ? this->music_play_idx : 0;
        this->load_to_stream(playlist[music_play_idx++]);
    };

}

void Audio_Manager::load_music_playlist(const std::string music_location)
{
    namespace fs = std::experimental::filesystem;

    using directory_iterator = fs::directory_iterator;
    using file_type = fs::file_type;

    size_t playlist_size = std::distance(directory_iterator(music_location), directory_iterator{});

    playlist.reserve(playlist_size);
    int idx = 0;
    for (const auto& dir_entry : directory_iterator(music_location))
    {
        if (fs::is_regular_file(dir_entry.status()) and is_wav(std::string(dir_entry.path())))
        {
            playlist.emplace_back(dir_entry.path());
            std::cout << "Load: " << playlist[idx++] << std::endl;
        }
    }

    playlist.erase(playlist.begin()+1);
    if (playlist.empty())
    {
        std::cout << "No audio resources, exitting!" << std::endl;
        exit(-3);
    }
}

bool Audio_Manager::is_wav(std::string path)
{
    constexpr int min_song_path_len = 4; //.wav characters
    if (path.size() >= min_song_path_len)
    {
         std::string extension = path.substr(path.size() - min_song_path_len);
         return extension.compare(".wav") == 0;
    }
    return false;
}

void Audio_Manager::init_audio_mgr()
{
    PaError error = Pa_Initialize();
    if(error != paNoError)
    {
        std::cout << "Portaudio not initialized properly!" << std::endl;
        exit(-1);
    }
}

int streamCallback(const void *inputBuffer, void *outputBuffer,
                                  unsigned long framesPerBuffer,
                                  const PaStreamCallbackTimeInfo* timeInfo,
                                  PaStreamCallbackFlags statusFlags,
                                  void *userData )
{
    Audio_Manager::UserCbkData *data = (Audio_Manager::UserCbkData*)userData;
    float *out = (float*)outputBuffer;
    unsigned long i,j;
    (void) timeInfo; /* Prevent unused variable warnings. */
    (void) statusFlags;

    /* stream out contents of data->buffer looping at end */

    for(unsigned long i = 0; i < framesPerBuffer; i++ )
    {
        for(unsigned long j = 0; j < data->channel_count; ++j ){
            *out++ = data->buffer[data->playbackIndex++];

            if( data->playbackIndex >= data->bufferSampleCount )
            {
                data->load_next_track();
                data->playbackIndex = 0;
            }

        }
    }

    return paContinue;
}

void Audio_Manager::play_music_file()
{
    music_play_idx = (music_play_idx < playlist.size()) ? music_play_idx : 0;
    play_sound_file(playlist[music_play_idx++]);
}

bool Audio_Manager::load_to_stream(const std::string soundFile)
{
    PaError error;
    /* Open an audio I/O stream. */

    const int MAX_CHANNELS = 1;
    const size_t BUFFER_LEN = 1024;
    
    using boost::format;
    using boost::io::group;
    std::cout << "[AUDIO_MGR] load_to_stream: " << soundFile << std::endl;
    if (soundFile.empty())
    {
        std::cout << "soundFile empty!" << std::endl;
        errno = EINVAL;
        return false;
    }
    std::cout << __LINE__ << std::endl;
    boost::filesystem::path soundFilePath(soundFile);
    if (! boost::filesystem::exists(soundFilePath))
    {
        std::cout << "soundFilePath not exists!" << std::endl;
        errno = EINVAL;
        return false;
    }
    SNDFILE* sndFile = nullptr;
    SF_INFO sfInfo;
    ::memset(&sfInfo, 0, sizeof(sfInfo));
    sndFile = sf_open(soundFile.c_str(), SFM_READ, &sfInfo);
    if (! sndFile)
    {
        std::cout << "sndFile not exists!" << std::endl;
        Pa_Terminate();
        return false;
    }

    Audio_Manager::user_data.bufferSampleCount = sfInfo.channels * sfInfo.frames;
    if (sfInfo.channels > MAX_CHANNELS)
    {
        Pa_Terminate();
        return false;
    }

    std::cout << "bufferSampleCount: " << Audio_Manager::user_data.bufferSampleCount << std::endl;
    // int subFormat = sfInfo.format & SF_FORMAT_SUBMASK;

    Audio_Manager::user_data.channel_count = sfInfo.channels;
    sf_read_float(sndFile, Audio_Manager::user_data.buffer,  sfInfo.channels * sfInfo.frames);
}

bool Audio_Manager::play_sound_file(const std::string soundFile)
{
    uint16_t SAMPLE_RATE = 16000;
    unsigned long FRAMES_PER_BUFFER = 256;

    PaError error;
    load_to_stream(soundFile);
    error = Pa_OpenDefaultStream(&stream,
                                 0,          /* no input channels */
                                 1,          /* stereo output */
                                 paFloat32,  /* 32 bit floating point output */
                                 SAMPLE_RATE,
                                 FRAMES_PER_BUFFER, /* frames per buffer, i.e. the number
                                                    of sample frames that PortAudio will
                                                    request from the callback. Many apps
                                                    may want to use
                                                    paFramesPerBufferUnspecified, which
                                                    tells PortAudio to pick the best,
                                                    possibly changing, buffer size.*/
                                //  &Audio_Manager::streamCallback, /* this is your callback function */
                                 streamCallback, /* this is your callback function */
                                 &Audio_Manager::user_data); /*This is a pointer that will be passed to
                                                    your callback*/

    if( error != paNoError )
    {
        std::cout << "Portaudio could not open a stream!" << std::endl;
        exit(-1);
    }

    if (!stream)
    {
        std::cout << "stream null!" << std::endl;
        Pa_Terminate();
        return false;
    }

    error = Pa_StartStream(stream);
    if (error != paNoError)
    {
        std::cout << "start stream error: "<< error << std::endl;
        Pa_Terminate();
        return false;
    }

    return true;
}

void Audio_Manager::stop_music_file()
{
    PaError paError = Pa_AbortStream(stream);
    Audio_Manager::user_data.playbackIndex = 0;
    if (paError != paNoError)
    {
        Pa_Terminate();
    }
    music_play_idx = 0;
}
