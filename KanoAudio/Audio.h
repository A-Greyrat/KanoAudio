//
// Created by Adarion on 2022/11/25.
//

#ifndef ADARION_AUDIO_H
#define ADARION_AUDIO_H

extern "C" {
    #include <AL/al.h>
    #include <AL/alc.h>
};

#include <cstddef>
#include <memory>
#include <mutex>
#include <future>

#include "Decoder/IAudioDecoder.h"
// push your own log system here

namespace KanoAudio
{
    class Audio
    {
    private:
        using byte = uint8_t;
        using uint = ALuint;

    protected:
        float volume_ = 1.0f;
        float pitch_ = 1.0f;

        uint buffer_ = 0;
        uint source_ = 0;

        std::size_t size_ = 0;
        uint frequency_ = 0;
        uint channels_ = 0;
        uint bitsPerSample_ = 0;

        bool isLooping_ = false;

        mutable std::mutex mutex_;
    private:
        Audio() = default;

    public:
        Audio(const Audio &audio) = delete;
        Audio &operator=(const Audio &audio) = delete;

        Audio(Audio &&audio) noexcept = delete;
        Audio &operator=(Audio &&audio) noexcept = delete;

        ~Audio();

    public:
        static std::shared_ptr<Audio> Create();

    public:
        void Play() const;
        void Pause() const;
        void Stop() const;
        void Unload();

        void SetVolume(float volume);
        void SetPitch(float pitch);
        void SetLooping(bool looping);
        void SetCurrentTime(double time) const;

        [[nodiscard]] float GetVolume() const;
        [[nodiscard]] float GetPitch() const;
        [[nodiscard]] bool IsLooping() const;
        [[nodiscard]] bool IsPlaying() const;
        [[nodiscard]] bool IsPaused() const;
        [[nodiscard]] double GetDuration() const;
        [[nodiscard]] double GetCurrentTime() const;
        [[nodiscard]] bool IsLoaded() const;

        template <class T, class = typename std::enable_if<std::is_base_of_v<IAudioDecoder, T>>::type>
        void Load(const char* path)
        {
            T decoder;
            if (decoder.Decode(path))
                return;

            if (IsLoaded())
            {
                Stop();

                alDeleteSources(1, &source_);
                alDeleteBuffers(1, &buffer_);
            }

            size_ = decoder.GetSize();
            frequency_ = decoder.GetFrequency();
            channels_ = decoder.GetChannels();
            bitsPerSample_ = decoder.GetBitsPerSample();

            alGenBuffers(1, &buffer_);
            alBufferData(buffer_, decoder.GetFormat(), decoder.GetData().get(), size_, frequency_);
            alGenSources(1, &source_);
            alSourcei(source_, AL_BUFFER, (ALint) buffer_);
        }

        template <class T, class = typename std::enable_if<std::is_base_of_v<IAudioDecoder, T>>::type>
        std::future<void> LoadAsync(const char* path)
        {
            return std::async(std::launch::async, [this, path]()
            {
                T decoder;
                if (decoder.Decode(path))
                    return;

                if (IsLoaded())
                {
                    Stop();

                    alDeleteSources(1, &source_);
                    alDeleteBuffers(1, &buffer_);
                }

                std::lock_guard<std::mutex> lock(mutex_);
                size_ = decoder.GetSize();
                frequency_ = decoder.GetFrequency();
                channels_ = decoder.GetChannels();
                bitsPerSample_ = decoder.GetBitsPerSample();

                alGenBuffers(1, &buffer_);
                alBufferData(buffer_, decoder.GetFormat(), decoder.GetData().get(), size_, frequency_);
                alGenSources(1, &source_);
                alSourcei(source_, AL_BUFFER, (ALint) buffer_);
            });
        }
    };

}

#endif //ADARION_AUDIO_H
