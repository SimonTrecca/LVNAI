#include "audionormalizer.h"

AudioNormalizer::AudioNormalizer() {}

void AudioNormalizer::write_wav_header(std::ofstream &out_file, int sample_rate, int channels,
                 int bits_per_sample, int data_size) {
  out_file.write("RIFF", 4);
  int chunk_size = 36 + data_size;
  out_file.write(reinterpret_cast<const char *>(&chunk_size), 4);
  out_file.write("WAVE", 4);

  // fmt subchunk
  out_file.write("fmt ", 4);
  int subchunk1_size = 16;
  short audio_format = 1;
  out_file.write(reinterpret_cast<const char *>(&subchunk1_size), 4);
  out_file.write(reinterpret_cast<const char *>(&audio_format), 2);
  out_file.write(reinterpret_cast<const char *>(&channels), 2);
  out_file.write(reinterpret_cast<const char *>(&sample_rate), 4);
  int byte_rate = sample_rate * channels * bits_per_sample / 8;
  out_file.write(reinterpret_cast<const char *>(&byte_rate), 4);
  short block_align = channels * bits_per_sample / 8;
  out_file.write(reinterpret_cast<const char *>(&block_align), 2);
  out_file.write(reinterpret_cast<const char *>(&bits_per_sample), 2);

  // data subchunk
  out_file.write("data", 4);
  out_file.write(reinterpret_cast<const char *>(&data_size), 4);
}


// Function to handle audio transcoding
void AudioNormalizer::transcode_audio(const char *input_filename, const char *output_filename) {

    AVFormatContext *input_format_ctx = nullptr;
    if (avformat_open_input(&input_format_ctx, input_filename, nullptr, nullptr) < 0) {
        std::cerr << "Error: Unable to open input file.\n";
        return;
    }

    // Retrieve stream information
    if (avformat_find_stream_info(input_format_ctx, nullptr) < 0) {
        std::cerr << "Error: Could not find stream information.\n";
        avformat_close_input(&input_format_ctx);
        return;
    }

    // Find the best audio stream
    int stream_index = av_find_best_stream(input_format_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (stream_index < 0) {
        std::cerr << "Error: No audio stream found.\n";
        avformat_close_input(&input_format_ctx);
        return;
    }

    // Initialize codec context for decoding
    const AVCodec *decoder = avcodec_find_decoder(input_format_ctx->streams[stream_index]->codecpar->codec_id);
    AVCodecContext *decoder_ctx = avcodec_alloc_context3(decoder);
    avcodec_parameters_to_context(decoder_ctx, input_format_ctx->streams[stream_index]->codecpar);

    if (avcodec_open2(decoder_ctx, decoder, nullptr) < 0) {
        std::cerr << "Error: Unable to open codec.\n";
        avcodec_free_context(&decoder_ctx);
        avformat_close_input(&input_format_ctx);
        return;
    }

    // Ensure channel layout is set
    if (decoder_ctx->channel_layout == 0) {
        decoder_ctx->channel_layout = av_get_default_channel_layout(decoder_ctx->channels);
        if (decoder_ctx->channel_layout == 0) {
            std::cerr << "Error: Unable to determine channel layout.\n";
            avcodec_free_context(&decoder_ctx);
            avformat_close_input(&input_format_ctx);
            return;
        }
    }

    // Set up the resampler
    SwrContext *swr_ctx = swr_alloc_set_opts(
        nullptr,
        AV_CH_LAYOUT_MONO,                      // Output layout (mono)
        AV_SAMPLE_FMT_S16,                      // Output format (16-bit signed)
        16000,                                  // Output sample rate (16kHz)
        decoder_ctx->channel_layout,            // Input layout
        decoder_ctx->sample_fmt,                // Input format
        decoder_ctx->sample_rate,               // Input sample rate
        0, nullptr
        );

    if (!swr_ctx || swr_init(swr_ctx) < 0) {
        std::cerr << "Error: Unable to initialize resample context.\n";
        swr_free(&swr_ctx);
        avcodec_free_context(&decoder_ctx);
        avformat_close_input(&input_format_ctx);
        return;
    }

    // Prepare output WAV file
    std::ofstream output_file(output_filename, std::ios::binary);
    int bits_per_sample = 16;
    int data_size = 0;
    write_wav_header(output_file, 16000, 1, bits_per_sample, data_size);

    // Allocate buffers for decoding and resampling
    AVPacket packet;
    AVFrame *frame = av_frame_alloc();
    int max_dst_nb_samples = av_rescale_rnd(1024, 16000, decoder_ctx->sample_rate, AV_ROUND_UP);
    uint8_t *output_data[1];
    int output_linesize;
    av_samples_alloc(output_data, &output_linesize, 1, max_dst_nb_samples, AV_SAMPLE_FMT_S16, 0);

    // Read, decode, and resample audio packets
    while (av_read_frame(input_format_ctx, &packet) >= 0) {
        if (packet.stream_index == stream_index) {
            if (avcodec_send_packet(decoder_ctx, &packet) == 0) {
                while (avcodec_receive_frame(decoder_ctx, frame) == 0) {
                    int output_samples = swr_convert(swr_ctx, output_data, max_dst_nb_samples,
                                                     (const uint8_t **)frame->data, frame->nb_samples);

                    if (output_samples < 0) {
                        std::cerr << "Error during resampling.\n";
                        continue;
                    }

                    // Calculate buffer size and write to output file
                    int buffer_size = av_samples_get_buffer_size(nullptr, 1, output_samples, AV_SAMPLE_FMT_S16, 1);
                    output_file.write(reinterpret_cast<const char *>(output_data[0]), buffer_size);
                    data_size += buffer_size;
                }
            }
        }
        av_packet_unref(&packet);
    }

    // Update the WAV header with the correct data size
    output_file.seekp(0, std::ios::beg);
    write_wav_header(output_file, 16000, 1, bits_per_sample, data_size);

    // Cleanup
    output_file.close();
    av_freep(&output_data[0]);
    av_frame_free(&frame);
    swr_free(&swr_ctx);
    avcodec_free_context(&decoder_ctx);
    avformat_close_input(&input_format_ctx);
}
