#pragma once

#include <SDL.h>

#include "common.hpp"

class Sound {
private:
    SDL_AudioDeviceID m_device;

    SDL_AudioSpec m_audioSpec;
    Uint8*        m_waveStart;
    Uint32        m_waveLength;

public:
    Sound(std::string path) {
        if(!SDL_LoadWAV(path.c_str(), &m_audioSpec, &m_waveStart, &m_waveLength))
            throw std::runtime_error("sound loading error: " + string(SDL_GetError()));
    }

    ~Sound() {
        SDL_FreeWAV(m_waveStart);
        SDL_CloseAudioDevice(m_device);
    }

    void play() {
        int status = SDL_QueueAudio(m_device, m_waveStart, m_waveLength);
        SDL_PauseAudioDevice(m_device,0);
    }

    void stop() {
        SDL_PauseAudioDevice(m_device,1);
    }

    void setup() {
        m_device = SDL_OpenAudioDevice(nullptr, 0, &m_audioSpec, nullptr, SDL_AUDIO_ALLOW_ANY_CHANGE);
        if(0 == m_device){
            throw std::runtime_error("sound device error: " + string(SDL_GetError()));
        }
    }
};