#ifndef LLAMAMODULE_H
#define LLAMAMODULE_H

#include "llama.h"
#include "utils.h"
#include <string>
#include <iostream>
#include <vector>

class llamaModule
{
public:
    llamaModule();
    ~llamaModule();
    std::string getAnswer(const std::string& input);
    void resetContext(const int& n_ctx);

private:
    llama_model* model;
    llama_model_params model_params;
    llama_context* ctx;
    bool ctx_initialized;
    llama_sampler* smpl;
    std::vector<llama_chat_message> messages;
    std::string generate(const std::string& prompt);
};

#endif // LLAMAMODULE_H
