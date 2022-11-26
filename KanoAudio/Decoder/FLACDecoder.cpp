//
// Created by Adarion on 2022/11/25.
//

extern "C" {
#include <AL/al.h>
};

#include "FLACDecoder.h"

namespace
{
    void error_callback(const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, void *client_data)
    {
        fprintf(stderr, "Got error callback: %s\n", FLAC__StreamDecoderErrorStatusString[status]);
    }
}


namespace KanoAudio
{

    void FLACDecoder::MetadataCallback(const FLAC__StreamDecoder *decoder, const FLAC__StreamMetadata *metadata,
                                       void *client_data)
    {
        auto *flacDecoder = static_cast<FLACDecoder *>(client_data);
        flacDecoder->size_ = metadata->data.stream_info.total_samples * metadata->data.stream_info.channels *
                             (metadata->data.stream_info.bits_per_sample / 8);
        flacDecoder->frequency_ = metadata->data.stream_info.sample_rate;
        flacDecoder->channels_ = metadata->data.stream_info.channels;
        flacDecoder->bitsPerSample_ = metadata->data.stream_info.bits_per_sample;
        flacDecoder->data_ = std::shared_ptr<uint8_t[]>(new uint8_t[flacDecoder->size_]);
        flacDecoder->format_ = flacDecoder->channels_ == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
    }

    FLAC__StreamDecoderWriteStatus
    FLACDecoder::WriteCallback(const FLAC__StreamDecoder *decoder, const FLAC__Frame *frame,
                               const FLAC__int32 *const *buffer, void *client_data)
    {
        auto *flacDecoder = static_cast<FLACDecoder *>(client_data);
        auto *data = flacDecoder->data_.get();

        for (int i = 0, j = 0; i < frame->header.blocksize; i++)
        {
            data[flacDecoder->offset_++] = (uint8_t) (buffer[0][i] & 0xFF);
            data[flacDecoder->offset_++] = (uint8_t) ((buffer[0][i] >> 8) & 0xFF);
            if (flacDecoder->channels_ == 2)
            {
                data[flacDecoder->offset_++] = (uint8_t) (buffer[1][i] & 0xFF);
                data[flacDecoder->offset_++] = (uint8_t) ((buffer[1][i] >> 8) & 0xFF);
            }
        }

        return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
    }

    int FLACDecoder::Decode(const char *path)
    {
        // Create a FLAC__StreamDecoder
        FLAC__StreamDecoder *decoder = FLAC__stream_decoder_new();
        if (decoder == nullptr)
        {
            fprintf(stderr, "Failed to create decoder\n");
            return -1;
        }

        FLAC__stream_decoder_set_md5_checking(decoder, true);

        // Initialize the decoder
        FLAC__StreamDecoderInitStatus status =
                FLAC__stream_decoder_init_file(decoder, path, WriteCallback, MetadataCallback,
                                               [](const FLAC__StreamDecoder *decoder,
                                                  FLAC__StreamDecoderErrorStatus status, void *client_data) {
                                                   fprintf(stderr, "Got error callback: %s\n",
                                                           FLAC__StreamDecoderErrorStatusString[status]);
                                               }, this);

        // Check the status
        if (status != FLAC__STREAM_DECODER_INIT_STATUS_OK)
        {
            FLAC__stream_decoder_delete(decoder);
            fprintf(stderr, "Got error while initializing decoder: %s\n",
                    FLAC__StreamDecoderInitStatusString[status]);
            return -1;
        }

        // Decode the file
        if (!FLAC__stream_decoder_process_until_end_of_stream(decoder))
        {
            FLAC__stream_decoder_delete(decoder);
            fprintf(stderr, "Got error while decoding file: %s\n",
                    FLAC__StreamDecoderStateString[FLAC__stream_decoder_get_state(decoder)]);
            return -1;
        }

        FLAC__stream_decoder_delete(decoder);

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