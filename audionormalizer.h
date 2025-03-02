#ifndef AUDIONORMALIZER_H
#define AUDIONORMALIZER_H

#include <iostream>
#include <fstream>

extern "C" {
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}

class AudioNormalizer
{
public:
    AudioNormalizer();
    void transcode_audio(const char *input_filename, const char *output_filename);
private:
    void write_wav_header(std::ofstream &out_file, int sample_rate, int channels, int bits_per_sample, int data_size);
};

#endif // AUDIONORMALIZER_H
