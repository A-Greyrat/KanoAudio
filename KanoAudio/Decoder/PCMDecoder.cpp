//
// Created by Adarion on 2022/11/25.
//

extern "C" {
    #include <AL/al.h>
};

#include "PCMDecoder.h"

namespace KanoAudio
{

    int PCMDecoder::Decode(const char* path)
    {
        // PCM Format is a raw audio data, so we don't need to decode it.
        // We just need to read the file and get the data.
        FILE *file = fopen(path, "rb");
        if (file == nullptr)
            return -1;

        fseek(file, 0, SEEK_END);
        size_ = ftell(file);

        fseek(file, 0, SEEK_SET);
        data_ = std::shared_ptr<uint8_t[]>(new uint8_t[size_]);
        fread(data_.get(), size_, 1, file);

        fclose(file);

        // PCM frequency is 44100
        frequency_ = 44100;

        // PCM channels is 2
        channels_ = 2;

        // PCM bits per sample is 16
        bitsPerSample_ = 16;

        // PCM format is AL_FORMAT_STEREO16
        format_ = AL_FORMAT_STEREO16;

        return 0;
    }

    std::size_t PCMDecoder::GetSize() const
    {
        return size_;
    }

    unsigned int PCMDecoder::GetFrequency() const
    {
        return frequency_;
    }

    unsigned int PCMDecoder::GetChannels() const
    {
        return channels_;
    }

    unsigned int PCMDecoder::GetBitsPerSample() const
    {
        return bitsPerSample_;
    }

    std::shared_ptr<uint8_t[]> PCMDecoder::GetData() const
    {
        return data_;
    }

    int PCMDecoder::GetFormat() const
    {
        return format_;
    }
}