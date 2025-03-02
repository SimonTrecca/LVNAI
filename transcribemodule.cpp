#include "transcribemodule.h"
#include <iostream>
#include "utils.h"
#define BUFFER_SIZE 1024
#define NORMALIZE_FACTOR 32768.0f

TranscribeModule::TranscribeModule() :
    file_data(), done(false){
};

void TranscribeModule::read(std::FILE* file) {
    if (!file) return;

    if (std::fseek(file, 0, SEEK_END) != 0) return;
    std::size_t file_dim = static_cast<std::size_t>(std::ftell(file));
    if (file_dim == static_cast<std::size_t>(-1)) return;
    std::rewind(file);

    file_data.reserve(file_dim / sizeof(int16_t));

    std::vector<int16_t> buffer(BUFFER_SIZE);
    std::size_t read;
    while ((read = std::fread(buffer.data(), sizeof(int16_t), BUFFER_SIZE, file)) > 0) {
        if (std::ferror(file)) return;
        for (std::size_t i = 0; i < read; ++i)
            file_data.push_back(buffer[i] / NORMALIZE_FACTOR);
    }

    done = true;
}


std::string TranscribeModule::transcribeText(const std::string& file_path ,const std::string& lang){

    std::FILE* file = std::fopen(file_path.c_str(), "rb");
    if(file== nullptr){
        std::perror("Error in opening file");
        exit(1);
    }

    whisper_context_params cparams = whisper_context_default_params();
    std::cout<<Utils::getCurrentDir()<<std::endl;
    std::string model_path = Utils::getCurrentDir().append("\\whisper_model\\model.bin");
    std::cout<<"Model path is: "<<model_path<<std::endl;
    struct whisper_context* ctx = whisper_init_from_file_with_params(model_path.c_str(), cparams);
    if (!ctx) {
        std::cerr << "Failed to initialize Whisper context." << std::endl;
        exit(1);
    }
    read(file);
    if(file_data.empty()){
        std::cerr << "No audio data read." << std::endl;
        whisper_free(ctx);
        exit(1);
    }

    whisper_full_params wparams = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);

    wparams.n_threads = 12; // Number of threads
    wparams.detect_language = false;
    wparams.language = lang.c_str();

    if (whisper_full(ctx, wparams, getData(), getSize()) != 0) {
        std::cerr << "Failed to process audio." << std::endl;
        whisper_free(ctx);
        exit(1);
    }

    std::string full_text="";
    const int n_segments = whisper_full_n_segments(ctx);
    for (int i = 0; i < n_segments; ++i)
        full_text.append(whisper_full_get_segment_text(ctx, i));

    whisper_free(ctx);
    fclose(file);
    std::vector<float>().swap(file_data);

    return full_text;
}




