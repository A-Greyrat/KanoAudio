//
// Created by Adarion on 2022/11/25.
//

#ifndef ADARION_FLACDECODER_H
#define ADARION_FLACDECODER_H

#include "IAudioDecoder.h"

namespace KanoAudio
{
    class FLACDecoder : public IAudioDecoder
    {
    private:
        std::size_t size_ = 0;
        unsigned int frequency_ = 0;
        unsigned int channels_ = 0;
        unsigned int bitsPerSample_ = 0;
        std::shared_ptr<uint8_t[]> data_ = nullptr;
        int format_ = 0;

    public:
        FLACDecoder() = default;
        FLACDecoder(const FLACDecoder &decoder) = delete;
        FLACDecoder &operator=(const FLACDecoder &decoder) = delete;
        FLACDecoder(FLACDecoder &&decoder) noexcept = default;
        FLACDecoder &operator=(FLACDecoder &&decoder) noexcept = default;
        ~FLACDecoder() override = default;

    public:
        int Decode(const char *path) override;

        [[nodiscard]] std::size_t GetSize() const override;
        [[nodiscard]] unsigned int GetFrequency() const override;
        [[nodiscard]] unsigned int GetChannels() const override;
        [[nodiscard]] unsigned int GetBitsPerSample() const override;
        [[nodiscard]] std::shared_ptr<uint8_t[]> GetData() const override;
        [[nodiscard]] int GetFormat() const override;
    };

}

#endif //KANOAUDIO_FLACDECODER_H
