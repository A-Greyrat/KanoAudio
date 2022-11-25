//
// Created by Adarion on 2022/11/24.
//

#ifndef ADARION_IAUDIODECODER_H
#define ADARION_IAUDIODECODER_H

#include <memory>
#include <string>

namespace KanoAudio
{
    class IAudioDecoder
    {
    public:
        virtual ~IAudioDecoder() = default;
        // Return Error Code, 0 for success
        virtual int Decode(const char* path) = 0;

        [[nodiscard]] virtual std::size_t GetSize() const = 0;
        [[nodiscard]] virtual unsigned int GetFrequency() const = 0;
        [[nodiscard]] virtual unsigned int GetChannels() const = 0;
        [[nodiscard]] virtual unsigned int GetBitsPerSample() const = 0;
        [[nodiscard]] virtual std::shared_ptr<uint8_t[]> GetData() const = 0;
        [[nodiscard]] virtual int GetFormat() const = 0;
    };

}


#endif //HW_IAUDIODECODER_H
