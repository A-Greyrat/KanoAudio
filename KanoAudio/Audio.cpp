//
// Created by Adarion on 2022/11/25.
//

#include "Audio.h"

namespace
{
#ifndef NO_CREATE_OPENAL_DEVICE_AND_CONTEXT
    [[maybe_unused]]
    int INIT_OPENAL = [] {
        auto device = alcOpenDevice(nullptr);
        auto context = alcCreateContext(device, nullptr);
        alcMakeContextCurrent(context);
        return 0;
    }();
#endif
}


namespace KanoAudio
{
    Audio::~Audio()
    {
        alDeleteSources(1, &source_);
        alDeleteBuffers(1, &buffer_);
    }

    void Audio::Play() const
    {
        if (IsPlaying())
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
        if (!IsPlaying())
            return;

        alSourceStop(source_);
    }

    void Audio::SetVolume(float volume)
    {
        volume_ = volume;
        alSourcef(source_, AL_GAIN, volume_);
    }

    void Audio::SetPitch(float pitch)
    {
        pitch_ = pitch;
        alSourcef(source_, AL_PITCH, pitch_);
    }

    void Audio::SetLooping(bool looping)
    {
        isLooping_ = looping;
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
        ALint state;
        alGetSourcei(source_, AL_SOURCE_STATE, &state);
        return state == AL_PLAYING;
    }

    bool Audio::IsPaused() const
    {
        ALint state;
        alGetSourcei(source_, AL_SOURCE_STATE, &state);
        return state == AL_PAUSED;
    }

    double Audio::GetDuration() const
    {
        return (double) size_ / (double) (frequency_ * channels_ * (bitsPerSample_ >> 3));
    }

    double Audio::GetCurrentTime() const
    {
        ALint sampleOffset;
        alGetSourcei(source_, AL_SAMPLE_OFFSET, &sampleOffset);
        return (double) sampleOffset / (double) frequency_;
    }

    void Audio::SetCurrentTime(double time) const
    {
        alSourcei(source_, AL_SAMPLE_OFFSET, (ALint) (time * frequency_));

        Play();
    }

    std::shared_ptr<Audio> Audio::Create()
    {
        return std::shared_ptr<Audio>(new Audio);
    }
}
