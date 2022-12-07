//
// Created by Adarion on 2022/11/25.
//

extern "C" {
#include <AL/al.h>
#include <AL/alext.h>
};

#include "WAVDecoder.h"
#include <sndfile.h>

namespace KanoAudio
{

    int WAVDecoder::Decode(const char *path)
    {
        SF_INFO sfinfo;
        SNDFILE *sndfile = sf_open(path, SFM_READ, &sfinfo);
        if (sndfile == nullptr)
        {
            KANO_AUDIO_LOG("[WAVDecoder]: Failed to open file: %s\n", path);
            return -1;
        }

        frequency_ = sfinfo.samplerate;
        channels_ = sfinfo.channels;
        switch (sfinfo.format)
        {
            case SF_FORMAT_WAV | SF_FORMAT_PCM_16:
                format_ = channels_ > 1 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
                bitsPerSample_ = 16;
                break;
            case SF_FORMAT_WAV | SF_FORMAT_PCM_24:
                format_ = channels_ > 1 ? AL_FORMAT_STEREO_FLOAT32 : AL_FORMAT_MONO_FLOAT32;
                bitsPerSample_ = 24;
                break;
            case SF_FORMAT_WAV | SF_FORMAT_PCM_32:
                format_ = channels_ > 1 ? AL_FORMAT_STEREO_FLOAT32 : AL_FORMAT_MONO_FLOAT32;
                bitsPerSample_ = 32;
                break;
            case SF_FORMAT_WAV | SF_FORMAT_PCM_U8:
                format_ = channels_ > 1 ? AL_FORMAT_STEREO8 : AL_FORMAT_MONO8;
                bitsPerSample_ = 8;
                break;
            case SF_FORMAT_WAV | SF_FORMAT_FLOAT:
                format_ = channels_ > 1 ? AL_FORMAT_STEREO_FLOAT32 : AL_FORMAT_MONO_FLOAT32;
                bitsPerSample_ = 32;
                break;
            case SF_FORMAT_WAV | SF_FORMAT_DOUBLE:
                format_ = channels_ > 1 ? AL_FORMAT_STEREO_DOUBLE_EXT : AL_FORMAT_MONO_DOUBLE_EXT;
                bitsPerSample_ = 64;
                break;
            default:
                KANO_AUDIO_LOG("[WAVDecoder]: Unsupported format: %d\n", sfinfo.format);
                return -1;
        }

        if ((sfinfo.format & SF_FORMAT_SUBMASK) == SF_FORMAT_PCM_24)
        {
            // 24-bit WAV is not supported by OpenAL
            // need to convert to 32-bit float
            auto *buffer = new float[sfinfo.frames * sfinfo.channels];
            sf_readf_float(sndfile, buffer, sfinfo.frames);
            size_ = sfinfo.frames * sfinfo.channels * sizeof(float);
            data_ = std::shared_ptr<uint8_t[]>(new uint8_t[size_]);
            memcpy(data_.get(), buffer, size_);

            delete[] buffer;
        }
        else if ((sfinfo.format & SF_FORMAT_SUBMASK) == SF_FORMAT_PCM_32)
        {
            // 32-bit WAV is not supported by OpenAL
            // need to convert to 32-bit float
            auto *buffer = new float[sfinfo.frames * sfinfo.channels];
            sf_readf_float(sndfile, buffer, sfinfo.frames);
            size_ = sfinfo.frames * sfinfo.channels * sizeof(float);
            data_ = std::shared_ptr<uint8_t[]>(new uint8_t[size_]);
            memcpy(data_.get(), buffer, size_);

            delete[] buffer;
        }
        else
        {
            size_ = sfinfo.frames * channels_ * (bitsPerSample_ >> 3);
            data_ = std::shared_ptr<uint8_t[]>(new uint8_t[size_]);
            sf_readf_short(sndfile, reinterpret_cast<short *>(data_.get()), sfinfo.frames);
        }

        sf_close(sndfile);

        return 0;
    }

    std::size_t WAVDecoder::GetSize() const
    { return size_; }

    unsigned int WAVDecoder::GetFrequency() const
    { return frequency_; }

    unsigned int WAVDecoder::GetChannels() const
    { return channels_; }

    unsigned int WAVDecoder::GetBitsPerSample() const
    { return bitsPerSample_; }

    std::shared_ptr<uint8_t[]> WAVDecoder::GetData() const
    { return data_; }

    int WAVDecoder::GetFormat() const
    { return format_; }
} // namespace KanoAudio