//
// Created by Adarion on 2022/11/25.
//

#include "MP3Decoder.h"

extern "C" {
    #include <mpg123.h>
    #include <AL/al.h>
};

namespace KanoAudio
{

    int MP3Decoder::Decode(const char *path)
    {
        mpg123_handle *handle = mpg123_new(nullptr, nullptr);
        if (handle == nullptr)
        {
            KANO_AUDIO_LOG("[MP3Decoder]: Failed to create mpg123 handle.\n");
            return -1;
        }

        if (mpg123_open(handle, path) != MPG123_OK)
        {
            KANO_AUDIO_LOG("[MP3Decoder]: Failed to open file: %s\n", path);
            return -1;
        }

        // Get the format of the decoded data
        long rate;
        int channels, encoding;
        if (mpg123_getformat(handle, &rate, &channels, &encoding) != MPG123_OK)
        {
            KANO_AUDIO_LOG("[MP3Decoder]: Unable to get format: %s\n", mpg123_strerror(handle));
            return -1;
        }

        // Ensure that this output format will not change (it could, when we allow it).
        mpg123_format_none(handle);
        mpg123_format(handle, rate, channels, encoding);

        // Get the length of the decoded data
        off_t length = mpg123_length(handle);
        if (length == MPG123_ERR)
        {
            KANO_AUDIO_LOG("[MP3Decoder]: Unable to get length: %s\n", mpg123_strerror(handle));
            return -1;
        }

        // the length is in samples, so we need to multiply by the number of channels
        // and the number of bytes per sample
        size_ = length * channels * mpg123_encsize(encoding);
        data_ = std::shared_ptr<uint8_t[]>(new uint8_t[size_]);
        if (mpg123_read(handle, data_.get(), size_, nullptr) != MPG123_OK)
        {
            KANO_AUDIO_LOG("[MP3Decoder]: Unable to read data: %s\n", mpg123_strerror(handle));
            return -1;
        }

        // Clean up
        mpg123_close(handle);
        mpg123_delete(handle);

        // Set the format
        frequency_ = static_cast<unsigned int>(rate);
        channels_ = static_cast<unsigned int>(channels);
        bitsPerSample_ = 16;
        format_ = channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

        return 0;
    }

    std::size_t MP3Decoder::GetSize() const
    {
        return size_;
    }

    unsigned int MP3Decoder::GetFrequency() const
    {
        return frequency_;
    }

    unsigned int MP3Decoder::GetChannels() const
    {
        return channels_;
    }

    unsigned int MP3Decoder::GetBitsPerSample() const
    {
        return bitsPerSample_;
    }

    std::shared_ptr<uint8_t[]> MP3Decoder::GetData() const
    {
        return data_;
    }

    int MP3Decoder::GetFormat() const
    {
        return format_;
    }
}