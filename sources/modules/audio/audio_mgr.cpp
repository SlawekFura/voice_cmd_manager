#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>
#include <experimental/filesystem>
#include <queue>
#include <mutex>

#include <sndfile.hh>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/format.hpp>

#include "audio_mgr.h"

extern std::queue<std::string> internal_msgs;
extern std::mutex internal_msgs_mutex;

Audio_Manager::Audio_Manager()
{
    std::cout << "[AUDIO_MGR] Audio_Manager" << std::endl;
    this->stream = nullptr;
    init_portaudio();

    load_music_playlist("/home/pi/projects/speech_to_text/my_resources/music/wav_16k", Track_type::WAV_16);
    load_music_playlist("/home/pi/projects/speech_to_text/my_resources/music/wav_48k", Track_type::WAV_48);
    Audio_Manager::user_data.load_next_track = [this]()
    {        
        internal_msgs.push("nastepna");
    };
    user_data.audio_mgr = this;
}

void Audio_Manager::load_music_playlist(const std::string music_location, Track_type type)
{
    namespace fs = std::experimental::filesystem;

    using directory_iterator = fs::directory_iterator;
    using file_type = fs::file_type;

    size_t playlist_size = std::distance(directory_iterator(music_location), directory_iterator{});

    playlist.reserve(playlist_size);
    for (const auto& dir_entry : directory_iterator(music_location))
    {
        //only wav file supported so far
        if (fs::is_regular_file(dir_entry.status()) and is_wav(std::string(dir_entry.path())))
        {
            playlist.push_back({dir_entry.path(), type});
            std::cout << "[AUDIO_MGR] Load: " << playlist.back().track_name << std::endl;
        }
    }

    if (playlist.empty())
    {
        std::cout << "[AUDIO_MGR] No audio resources, exitting!" << std::endl;
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

void Audio_Manager::init_portaudio()
{
    PaError error = Pa_Initialize();
    if(error != paNoError)
    {
        std::cout << "[AUDIO_MGR] Portaudio not initialized properly!" << std::endl;
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

    for (unsigned long i = 0; i < framesPerBuffer; i++)
    {
        for (unsigned long j = 0; j < data->channel_count; ++j)
        {
            *out++ = data->buffer[data->playbackIndex++];

            if (data->playbackIndex >= data->bufferSampleCount)
            {
                data->playbackIndex = 0;
                std::cout << "[AUDIO_MGR] streamCallback paContinue" << std::endl;

                std::lock_guard<std::mutex> lock(internal_msgs_mutex);
                internal_msgs.push("nastepna");
                return paContinue;
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

bool Audio_Manager::load_track(const std::string soundFile)
{
    PaError error;
    /* Open an audio I/O stream. */

    const int MAX_CHANNELS = 1;
    const size_t BUFFER_LEN = 1024;
    
    using boost::format;
    using boost::io::group;
    std::cout << "[AUDIO_MGR] load_track: " << soundFile << std::endl;
    if (soundFile.empty())
    {
        std::cout << "[AUDIO_MGR] soundFile empty!" << std::endl;
        errno = EINVAL;
        return false;
    }
    boost::filesystem::path soundFilePath(soundFile);
    if (! boost::filesystem::exists(soundFilePath))
    {
        std::cout << "[AUDIO_MGR] soundFilePath not exists!" << std::endl;
        errno = EINVAL;
        return false;
    }
    SNDFILE* sndFile = nullptr;
    SF_INFO sfInfo;
    ::memset(&sfInfo, 0, sizeof(sfInfo));
    sndFile = sf_open(soundFile.c_str(), SFM_READ, &sfInfo);
    if (! sndFile)
    {
        std::cout << "[AUDIO_MGR] sndFile not exists!" << std::endl;
        Pa_Terminate();
        return false;
    }

    Audio_Manager::user_data.bufferSampleCount = sfInfo.channels * sfInfo.frames;
    if (sfInfo.channels > MAX_CHANNELS)
    {
        Pa_Terminate();
        return false;
    }

    std::cout << "[AUDIO_MGR] bufferSampleCount: " << Audio_Manager::user_data.bufferSampleCount << std::endl;

    Audio_Manager::user_data.channel_count = sfInfo.channels;
    sf_read_float(sndFile, Audio_Manager::user_data.buffer,  sfInfo.channels * sfInfo.frames);
}

bool Audio_Manager::play_sound_file(const Track& track)
{
    uint16_t SAMPLE_RATE;
    switch(track.track_kind)
    {
        case Track_type::WAV_16:
            SAMPLE_RATE = 16000;
            break;
        case Track_type::WAV_48:
            SAMPLE_RATE = 48000;
            break;
        default:
            std::cout << "[AUDIO_MGR] unknown track type!" << std::endl;  
            return false;
    }
    unsigned long FRAMES_PER_BUFFER = 256;
    std::cout << "[AUDIO_MGR] SAMPLE_RATE: " << SAMPLE_RATE << std::endl;  
    load_track(track.track_name);
    PaError error;
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
        std::cout << "[AUDIO_MGR] Portaudio could not open a stream!" << std::endl;
        exit(-1);
    }

    if (!stream)
    {
        std::cout << "[AUDIO_MGR] stream null!" << std::endl;
        Pa_Terminate();
        return false;
    }

    error = Pa_StartStream(stream);
    if (error != paNoError)
    {
        std::cout << "[AUDIO_MGR] start stream error: "<< error << std::endl;
        Pa_Terminate();
        return false;
    }

    return true;
}

void Audio_Manager::stop_music_file()
{
    PaError paError = Pa_CloseStream(stream);
    stream = nullptr;
    if (paError != paNoError)
    {
        std::cout << "[AUDIO_MGR] Pa_StopStream error: "<< paError << std::endl;
        Pa_Terminate();
    }
}

void Audio_Manager::reset_track_queue()
{
    music_play_idx = 0;
}
