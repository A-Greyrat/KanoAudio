//
// Created by Adarion on 2022/11/25.
//

#include "Audio.h"

namespace
{
#ifndef NO_CREATE_OPENAL_DEVICE_AND_CONTEXT
    ALCdevice *device = nullptr;
    ALCcontext *context = nullptr;

    __attribute__((unused))
    int INIT_OPENAL = [] {
        device = alcOpenDevice(nullptr);
        context = alcCreateContext(device, nullptr);
        alcMakeContextCurrent(context);
        return 0;
    }();

    __attribute__((unused))
    struct CLOSE_OPENAL
    {
        ~CLOSE_OPENAL()
        {
            alcMakeContextCurrent(nullptr);
            alcDestroyContext(context);
            alcCloseDevice(device);
        }
    } close_openal;
#endif
}

namespace KanoAudio
{
    Audio::~Audio()
    {
        if (IsLoaded())
        {
            Stop();
            alDeleteSources(1, &source_);
            alDeleteBuffers(1, &buffer_);
        }
    }

    void Audio::Play() const
    {
        if (IsPlaying() || !IsLoaded())
            return;

        if (!IsPaused())
        {
            alSourcei(source_, AL_BUFFER, (int) buffer_);
            alSourcef(source_, AL_GAIN, volume_);
            alSourcef(source_, AL_PITCH, pitch_);
            alSourcei(source_, AL_LOOPING, isLooping_ ? AL_TRUE : AL_FALSE);
        }
        alSourcePlay(source_);
    }

    void Audio::Pause() const
    {
        if (!IsPlaying())
            return;

        alSourcePause(source_);
    }

    void Audio::Stop() const
    {
        if (!IsPlaying() && !IsPaused())
            return;

        alSourceStop(source_);
    }

    void Audio::SetVolume(float volume)
    {
        volume_ = volume;
        if (IsLoaded())
            alSourcef(source_, AL_GAIN, volume_);
    }

    void Audio::SetPitch(float pitch)
    {
        pitch_ = pitch;
        if (IsLoaded())
            alSourcef(source_, AL_PITCH, pitch_);
    }

    void Audio::SetLooping(bool looping)
    {
        isLooping_ = looping;
        if (IsLoaded())
            alSourcei(source_, AL_LOOPING, isLooping_ ? AL_TRUE : AL_FALSE);
    }

    float Audio::GetVolume() const
    {
        return volume_;
    }

    float Audio::GetPitch() const
    {
        return pitch_;
    }

    bool Audio::IsLooping() const
    {
        return isLooping_;
    }

    bool Audio::IsPlaying() const
    {
        if (!IsLoaded()) return false;

        ALint state;
        alGetSourcei(source_, AL_SOURCE_STATE, &state);
        return state == AL_PLAYING;
    }

    bool Audio::IsPaused() const
    {
        if (!IsLoaded()) return false;

        ALint state;
        alGetSourcei(source_, AL_SOURCE_STATE, &state);
        return state == AL_PAUSED;
    }

    double Audio::GetDuration() const
    {
        if (!IsLoaded()) return 0.0;
        return (double) size_ / (double) (frequency_ * channels_ * (bitsPerSample_ >> 3));
    }

    double Audio::GetCurrentTime() const
    {
        if (!IsLoaded()) return 0.0;
        ALint sampleOffset;
        alGetSourcei(source_, AL_SAMPLE_OFFSET, &sampleOffset);
        return (double) sampleOffset / (double) frequency_;
    }

    void Audio::SetCurrentTime(double time) const
    {
        if (!IsLoaded()) return;
        alSourcei(source_, AL_SAMPLE_OFFSET, (ALint) (time * frequency_));
        Play();
    }

    std::shared_ptr<Audio> Audio::Create()
    {
        return std::shared_ptr<Audio>(new Audio);
    }

    bool Audio::IsLoaded() const
    {
        return size_ != 0;
    }

    void Audio::Unload()
    {
        if (IsLoaded())
        {
            Stop();
            alDeleteSources(1, &source_);
            alDeleteBuffers(1, &buffer_);
            size_ = 0;
            frequency_ = 0;
            channels_ = 0;
            bitsPerSample_ = 0;
        }
    }

}
