//
// Created by Adarion on 2022/11/25.
//

extern "C" {
    #include <AL/al.h>
}

#include "OGGDecoder.h"

#include <vorbis/vorbisfile.h>

namespace KanoAudio
{
    int OGGDecoder::Decode(const char *path)
    {
        // Open the file
        FILE * file = fopen(path, "rb");
        if (file == nullptr)
            return -1;

        // Create a OggVorbis_File
        OggVorbis_File oggFile;
        if (ov_open(file, &oggFile, nullptr, 0) < 0)
        {
            fclose(file);
            return -1;
        }

        // Get the information of the file
        vorbis_info *info = ov_info(&oggFile, -1);
        if (info == nullptr)
        {
            ov_clear(&oggFile);
            fclose(file);
            return -1;
        }

        // Get the size of the file
        size_ = ov_pcm_total(&oggFile, -1) * info->channels * 2;

        // Get the frequency of the file
        frequency_ = info->rate;

        // Get the channels of the file
        channels_ = info->channels;

        // Get the bits per sample of the file
        bitsPerSample_ = 16;

        // Get the format of the file
        format_ = channels_ == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

        // Get the data of the file
        data_ = std::shared_ptr<uint8_t[]>(new uint8_t[size_]);
        int bitStream = 0;
        int result = 0;
        std::size_t offset = 0;
        while (offset < size_)
        {
            result = ov_read(&oggFile, reinterpret_cast<char *>(data_.get() + offset), size_ - offset, 0, 2, 1,
                             &bitStream);
            if (result > 0)
                offset += result;
            else
                break;
        }

        // Close the file
        ov_clear(&oggFile);

        return 0;
    }

    std::size_t OGGDecoder::GetSize() const
    {
        return size_;
    }

    unsigned int OGGDecoder::GetFrequency() const
    {
        return frequency_;
    }

    unsigned int OGGDecoder::GetChannels() const
    {
        return channels_;
    }

    unsigned int OGGDecoder::GetBitsPerSample() const
    {
        return bitsPerSample_;
    }

    std::shared_ptr<uint8_t[]> OGGDecoder::GetData() const
    {
        return data_;
    }

    int OGGDecoder::GetFormat() const
    {
        return format_;
    }
}