#include <fstream>
#include <iostream>
#include <cstdio>
#include <list>
#include <filesystem>
#include "syntax/Lexer.h"
#include "syntax/Lexer.cpp"
#include "syntax/Token.h"
#include "syntax/Token.cpp"

std::string source_path = "";
std::string output_dir = "";
static std::list<std::string> source_list;

static void Usage() {
    printf("Usage: leoml [-o <output>] [options] <source>\n"
           "Options: \n"
           "\t-h      Print this help message.\n"
           "\t-l      Tokenize the source.\n"
           "\t-p      Parse the source.\n"
           "\t-o      Specify output directory. Otherwise print to the stdout.\n");
    exit(0);
}

static std::string GetName(const std::string &path) {
    auto pos = path.rfind('/');
    if (pos == std::string::npos)
        return path;
    return path.substr(pos + 1);
}

std::string *LoadFile(const std::string &filepath) {
    FILE *f = fopen(filepath.c_str(), "r");
    if (!f) CompileError("%s: No such file or directory", filepath.c_str());
    auto text = new std::string;
    int c;
    while (EOF != (c = fgetc(f)))
        text->push_back(c);
    fclose(f);
    return text;
}

static void Tokenize() {
    for (auto source:source_list) {
        auto name = GetName(source);
        TokenSequence ts = TokenSequence();
        Lexer* lexer = Lexer::New(LoadFile(source), &name);
        if (output_dir != "") {
            auto outpath = std::filesystem::absolute(output_dir + "/" + name + "tts.txt");
            lexer->Tokenize(ts);
            std::ofstream out(outpath);
            ts.Output(out);
        } else { ts.Output(std::cout); }
    }
}

static void Parse() {
    CompilePanic("");
}

int main(int argc, char *argv[]) {
    if (argc < 2)Usage();
    for (int i = 0; i < argc; ++i) {
        if (argv[i][0] != '-') {
            source_path = std::string(argv[i]);
            source_list.push_back(source_path);
        }
        switch (argv[i][1]) {
            case 'h':
                Usage();
                break;
            case 'l':
                Tokenize();
            case 'p':
                Parse();
            case 'o':
                output_dir = std::string(argv[i + 1]);
        }
    }

    return 0;
}
