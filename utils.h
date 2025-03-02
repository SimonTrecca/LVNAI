#ifndef UTILS_H
#define UTILS_H
#ifdef _WIN32
#include <Windows.h>
#endif

#define DEFAULT_CTX_SIZE 10240
#include <string>
#include <algorithm>
#include "parser.h"
#include <iostream>
#include <fstream>

class Utils
{
public:


    static std::string getCurrentDir(){
        #ifdef _WIN32
                char path[MAX_PATH] = {0};
                HMODULE hModule = GetModuleHandle(NULL);
                if (hModule != nullptr) {
                    DWORD result = GetModuleFileNameA(hModule, path, MAX_PATH);
                    if (result != 0) {
                        std::string fullPath(path);
                        std::string::size_type pos = fullPath.find_last_of("\\/");
                        return (pos == std::string::npos) ? "" : fullPath.substr(0, pos);
                    }
                }
                return "";
        #else
                return "";
        #endif
    }

    static inline std::string save_path = getCurrentDir().append("\\prompts.xml");

    static bool isBlank(const std::string &str) {
        return std::all_of(str.begin(), str.end(), [](unsigned char c) {
            return std::isspace(c);
        });
    }
    static std::map<std::string, std::string>  &parse(const std::string& filename){
        std::ifstream infile(filename);
        if (!infile.good()) {
            std::map<std::string, std::string> emptyMap;
            save_file(emptyMap);
            std::cout << "I have created a new prompts savefile" << std::endl;
        }

        return parse_file(filename);;
    }
    static void save_file(const std::map<std::string, std::string> &prompts){
        std::ofstream saveFile(save_path);
        saveFile<<"<prompts>"<<std::endl;
        if (!saveFile) {
            std::cerr << "Error opening file for writing\n";
        }
        for (const auto& p : prompts) {
            saveFile<<"<prompt id=\""<<p.first<<"\">" <<p.second<<"</prompt>"<< std::endl;
        }
        saveFile<<"</prompts>";
        saveFile.close();
        std::cout<<"I am done saving the prompts"<<std::endl;
    }
};

#endif // UTILS_H
