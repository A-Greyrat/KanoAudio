//
// Created by Adarion on 2022/11/25.
//

extern "C" {
#include <AL/al.h>
#include <AL/alext.h>
};

#include "FLACDecoder.h"
#include "sndfile.h"

namespace KanoAudio
{
    int FLACDecoder::Decode(const char *path)
    {
        // snd lib
        SF_INFO sfinfo;
        SNDFILE *sndfile = sf_open(path, SFM_READ, &sfinfo);
        if (sndfile == nullptr)
        {
            fprintf(stderr, "Failed to open file: %s (%s)\n", path, sf_strerror(sndfile));
            return -1;
        }

        // snd
        this->frequency_ = sfinfo.samplerate;
        this->channels_ = sfinfo.channels;

        switch (sfinfo.format)
        {
            case SF_FORMAT_FLAC | SF_FORMAT_PCM_16:
                this->format_ = this->channels_ == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
                this->bitsPerSample_ = 16;
                break;
            case SF_FORMAT_FLAC | SF_FORMAT_PCM_24:
                this->format_ = this->channels_ == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
                this->bitsPerSample_ = 24;
                break;
            case SF_FORMAT_FLAC | SF_FORMAT_PCM_32:
                this->format_ = this->channels_ == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
                this->bitsPerSample_ = 32;
                break;
            case SF_FORMAT_FLAC | SF_FORMAT_PCM_U8:
                this->format_ = this->channels_ == 1 ? AL_FORMAT_MONO8 : AL_FORMAT_STEREO8;
                this->bitsPerSample_ = 8;
                break;
            case SF_FORMAT_FLAC | SF_FORMAT_FLOAT:
                this->format_ = this->channels_ == 1 ? AL_FORMAT_MONO_FLOAT32 : AL_FORMAT_STEREO_FLOAT32;
                this->bitsPerSample_ = 32;
                break;
            case SF_FORMAT_FLAC | SF_FORMAT_DOUBLE:
                this->format_ = this->channels_ == 1 ? AL_FORMAT_MONO_DOUBLE_EXT : AL_FORMAT_STEREO_DOUBLE_EXT;
                this->bitsPerSample_ = 64;
                break;
        }

        this->size_ = sfinfo.frames * this->channels_ * (this->bitsPerSample_ >> 3);
        this->data_ = std::shared_ptr<uint8_t[]>(new uint8_t[this->size_]);

        sf_readf_short(sndfile, (short *) this->data_.get(), sfinfo.frames);
        sf_close(sndfile);

        return 0;
    }

    std::size_t FLACDecoder::GetSize() const
    {
        return size_;
    }

    unsigned int FLACDecoder::GetFrequency() const
    {
        return frequency_;
    }

    unsigned int FLACDecoder::GetChannels() const
    {
        return channels_;
    }

    unsigned int FLACDecoder::GetBitsPerSample() const
    {
        return bitsPerSample_;
    }

    std::shared_ptr<uint8_t[]> FLACDecoder::GetData() const
    {
        return data_;
    }

    int FLACDecoder::GetFormat() const
    {
        return format_;
    }


}