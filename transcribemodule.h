#ifndef TRANSCRIBEMODULE_H
#define TRANSCRIBEMODULE_H

#include <string>
#include <vector>
#include <cassert>
#include "whisper.h"

class TranscribeModule
{
public:
    TranscribeModule();
    inline const float* getData(){
        assert(done);
        return file_data.data();
    }
    inline std::size_t getSize(){
        assert(done);
        return file_data.size();
    }

    std::string transcribeText(const std::string& file_path, const std::string& lang);
private:
    void read(std::FILE* file);
    std::vector<float> file_data;
    bool done;
};

#endif // TRANSCRIBEMODULE_H
