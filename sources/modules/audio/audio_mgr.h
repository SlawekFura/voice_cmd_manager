#pragma once

#include <iostream>
#include "portaudio.h"

class Audio_Manager
{
public:
    Audio_Manager();
    void play_music_file();
    void stop_music_file();
    void reset_track_queue();
    ~Audio_Manager() = default;

    typedef struct
    {
        float buffer[15000000];
        int bufferSampleCount;
        int playbackIndex;
        char channel_count;
        std::function<void()> load_next_track;
        Audio_Manager* audio_mgr;
    } UserCbkData;

    static UserCbkData user_data; // make it static due to exceeding stack size for non-static
private:
    enum class Track_type
    {
        WAV_16,
        WAV_48,
        MP3
    };
    typedef struct
    {
        std::string track_name;
        Track_type track_kind;
    }Track;

    void init_portaudio();
    bool play_sound_file(const Track&);
    void load_music_playlist(const std::string, Track_type);
    bool load_track(const std::string);
    bool is_wav(std::string);

    PaStream* stream;

    std::vector<Track> playlist;
    size_t music_play_idx = 0;
};
