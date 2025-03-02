#include "llamamodule.h"


llamaModule::llamaModule() :
    model_params(llama_model_default_params()), ctx(nullptr) {
    int ngl = 100;
    model_params.n_gpu_layers = ngl;
    std::string model_path = Utils::getCurrentDir().append("\\llm_model\\model.gguf");

    model = llama_model_load_from_file(model_path.c_str(), model_params);
    if (!model) {
        std::cout << "Error in loading the model" << std::endl;
        exit(1);
    }
    resetContext(DEFAULT_CTX_SIZE);
    smpl = llama_sampler_chain_init(llama_sampler_chain_default_params());
    llama_sampler_chain_add(smpl, llama_sampler_init_min_p(0.05f, 1));
    llama_sampler_chain_add(smpl, llama_sampler_init_temp(0.8f));
    llama_sampler_chain_add(smpl, llama_sampler_init_dist(LLAMA_DEFAULT_SEED));
}

std::string llamaModule::generate(const std::string & prompt) {
    std::string response;

    auto vocab = llama_model_get_vocab(model);

    const int n_prompt_tokens = -llama_tokenize(vocab, prompt.c_str(), prompt.size(), NULL, 0, true, true);
    std::vector<llama_token> prompt_tokens(n_prompt_tokens);
    if (llama_tokenize(vocab, prompt.c_str(), prompt.size(), prompt_tokens.data(), prompt_tokens.size(),
                       (llama_get_kv_cache_used_cells(ctx) == 0), true) < 0) {
        std::cout << "Failed to tokenize the prompt" << std::endl;
        exit(1);
    }
    std::cout << "The number of tokens is " << n_prompt_tokens << std::endl;

    llama_batch og_batch = llama_batch_get_one(prompt_tokens.data(), prompt_tokens.size());
    auto batch = og_batch;
    llama_token new_token_id;
    bool resized = false;
    while (true) {
        // Check if we have enough space in the context to evaluate this batch.
        int n_ctx = llama_n_ctx(ctx);
        int n_ctx_used = llama_get_kv_cache_used_cells(ctx);
        // std::cout << "The number of ctx used is " << n_ctx_used
                  // << " batch tokens is " << batch.n_tokens
                  // << " and n_ctx is " << n_ctx << std::endl;
        if (n_ctx_used + batch.n_tokens > n_ctx) {
            int new_size = n_prompt_tokens * 2;
            if (new_size < 131072) {
                std::cout << "Resized everything" << std::endl;
                resetContext(new_size);
                resized = true;
            } else {
                std::cout << "This prompt is too large to process" << std::endl;
                exit(1);
            }
            batch = og_batch;
            response = "";
        } else {
            if (llama_decode(ctx, batch)) {
                std::cout << "Failed to decode" << std::endl;
                exit(1);
            }
            new_token_id = llama_sampler_sample(smpl, ctx, -1);
            if (llama_vocab_is_eog(vocab, new_token_id)) {
                break;
            }
            // Convert the token to a string and add it to the response
            char buf[256];
            int n = llama_token_to_piece(vocab, new_token_id, buf, sizeof(buf), 0, true);
            if (n < 0) {
                std::cout << "Failed to convert token to piece" << std::endl;
            }
            std::string piece(buf, n);
            response += piece;
            // Prepare the next batch with the sampled token
            batch = llama_batch_get_one(&new_token_id, 1);
        }
    }
    if (resized) {
        std::cout << "Resized back" << std::endl;
        resetContext(DEFAULT_CTX_SIZE);
    }
    return response;
}

std::string llamaModule::getAnswer(const std::string& input) {
    messages.push_back({ "user", _strdup(input.c_str()) });
    std::vector<char> formatted(llama_n_ctx(ctx));

    const char* tmpl = llama_model_chat_template(model, nullptr);
    int new_len = llama_chat_apply_template(tmpl, messages.data(), messages.size(), true, formatted.data(), formatted.size());
    if (new_len > (int)formatted.size()) {
        formatted.resize(new_len);
        new_len = llama_chat_apply_template(tmpl, messages.data(), messages.size(), true, formatted.data(), formatted.size());
    }
    if (new_len < 0) {
        fprintf(stderr, "failed to apply the chat template\n");
        exit(1);
    }
    std::string prompt(formatted.begin(), formatted.begin() + new_len);
    return generate(prompt);
}

void llamaModule::resetContext(const int& n_ctx) {
    if (ctx) {
        std::cout << "Resetting the context" << std::endl;
        llama_free(ctx);
        messages.clear();
    }
    llama_context_params ctx_params = llama_context_default_params();
    ctx_params.n_ctx = n_ctx;
    ctx_params.n_batch = n_ctx;

    ctx = llama_init_from_model(model, ctx_params);
    if (!ctx) {
        fprintf(stderr, "Error in creating the Llama context\n");
        exit(1);
    }
}

llamaModule::~llamaModule() {
    messages.clear();
    llama_sampler_free(smpl);
    llama_free(ctx);
    llama_model_free(model);
}
