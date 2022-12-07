//
// Created by Adarion on 2022/11/25.
//

extern "C" {
#include <AL/al.h>
}

#include "OGGDecoder.h"

#include <sndfile.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

namespace KanoAudio
{
    int OGGDecoder::Decode(const char *path)
    {
        // Open the file
        FILE *file = fopen(path, "rb");
        if (file == nullptr)
        {
            KANO_AUDIO_LOG("[OGGDecoder]: Failed to open file: %s\n", path);
            return -1;
        }

        // Create a OggVorbis_File
        OggVorbis_File oggFile;
        if (ov_open(file, &oggFile, nullptr, 0) < 0)
        {
            KANO_AUDIO_LOG("[OGGDecoder]: Failed to open OggVorbis_File: %s\n", path);
            fclose(file);
            return -1;
        }

        // Get the information of the file
        vorbis_info *info = ov_info(&oggFile, -1);
        if (info == nullptr)
        {
            KANO_AUDIO_LOG("[OGGDecoder]: Failed to get the info of the file: %s\n", path);
            ov_clear(&oggFile);
            fclose(file);
            return -1;
        }

        size_ = ov_pcm_total(&oggFile, -1) * info->channels * 2;
        frequency_ = info->rate;
        channels_ = info->channels;
        bitsPerSample_ = 16;
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