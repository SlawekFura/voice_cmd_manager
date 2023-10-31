#pragma once

#include <iostream>
#include "portaudio.h"

class Audio_Manager
{
public:
    Audio_Manager();
    void init_audio_mgr();
    void play_music_file();
    void stop_music_file();
    ~Audio_Manager() = default;

    typedef struct
    {
        float buffer[15000000];
        int bufferSampleCount;
        int playbackIndex;
        char channel_count;
        std::function<void()> load_next_track;
    } UserCbkData;
    static UserCbkData user_data; // make it static due to exceeding stack size for non-static
private:

    bool play_sound_file(const std::string);
    void load_music_playlist(const std::string);
    bool load_to_stream(const std::string);
    bool is_wav(std::string path);

    PaStream *stream;
    std::vector<std::string> playlist;
    size_t music_play_idx = 0;
};
