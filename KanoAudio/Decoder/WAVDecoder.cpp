//
// Created by Adarion on 2022/11/25.
//

extern "C" {
    #include <AL/al.h>
};

#include "WAVDecoder.h"

namespace
{
    // wav file header
    struct WAVHeader
    {
        char riff[4]; // "RIFF"
        int32_t size; // file size - 8
        char wave[4]; // "WAVE"
        char fmt[4]; // "fmt "
        int32_t fmtSize; // 16
        int16_t format; // 1
        int16_t channels; // 1
        int32_t frequency; // 8000
        int32_t bytesPerSecond; // 16000
        int16_t bytesPerSample; // 2
        int16_t bitsPerSample; // 16
        char data[4]; // "data"
        int32_t dataSize; // file size - 44
    };
}

namespace KanoAudio
{

    int WAVDecoder::Decode(const char *path)
    {
        FILE *file = fopen(path, "rb");
        if (file == nullptr)
            return -1;

        WAVHeader header{};
        fread(&header, sizeof(WAVHeader), 1, file);

        size_ = header.size;
        frequency_ = header.frequency;
        channels_ = header.channels;
        bitsPerSample_ = header.bitsPerSample;

        data_ = std::shared_ptr<uint8_t[]>(new uint8_t[size_]);

        // 44 is the size of WAVHeader, header.dataSize is the size of extra data
        // 4 is the size of "data", 4 is the data chunk size
        fseek(file, 44 + header.dataSize + 4 + 4, SEEK_SET);
        fread(data_.get(), size_, 1, file);
        fclose(file);

        if (channels_ == 1)
            format_ = bitsPerSample_ == 8 ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;
        else if (channels_ == 2)
            format_ = bitsPerSample_ == 8 ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;

        return 0;
    }

    std::size_t WAVDecoder::GetSize() const
    {
        return size_;
    }

    unsigned int WAVDecoder::GetFrequency() const
    {
        return frequency_;
    }

    unsigned int WAVDecoder::GetChannels() const
    {
        return channels_;
    }

    unsigned int WAVDecoder::GetBitsPerSample() const
    {
        return bitsPerSample_;
    }

    std::shared_ptr<uint8_t[]> WAVDecoder::GetData() const
    {
        return data_;
    }

    int WAVDecoder::GetFormat() const
    {
        return format_;
    }
}