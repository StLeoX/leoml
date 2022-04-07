#include <fstream>
#include <iostream>
#include <list>
#include <filesystem>
#include "syntax/Lexer.h"
#include "syntax/Lexer.cpp"
#include "syntax/Token.h"
#include "syntax/Token.cpp"

std::string source_path = "";
std::string output_dir = "";
std::list<std::string> source_list{};

void Usage() {
    printf("Usage: leoml [-o <output>] [options] <source>\n"
           "Options: \n"
           "\t-h      Print this help message.\n"
           "\t-l      Tokenize the source.\n"
           "\t-p      Parse the source.\n"
           "\t-o      Specify output directory. Otherwise print to the stdout.\n");
    exit(0);
}

///
/// \param filepath
/// \return filename
std::string GetName(const std::string &path) {
    auto pos = path.rfind('/');
    if (pos == std::string::npos)
        return path;
    return path.substr(pos + 1);
}

///
/// \param filepath
/// \return content
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

void Tokenize() {
    printf("tokenizing\n");
    for (auto source:source_list) {
        auto name = GetName(source);
        TokenSequence ts = TokenSequence();
        Lexer *lexer = Lexer::New(LoadFile(source), &name);
        lexer->Tokenize(ts);
        if (output_dir != "") {
            auto outpath = std::filesystem::absolute(output_dir + "/" + name + "tts.txt");
            std::ofstream out{outpath};
            ts.Output(out);
        } else { ts.Output(std::cout); }
    }
}

void Parse() {
    CompilePanic("");
}

int main(int argc, char *argv[]) {
    if (argc < 2) Usage();
    if (argc >= 5) {
        assert('o' == argv[argc - 2][1]);
        output_dir = argv[argc - 1];
        for (int i = 2; i < argc - 2; ++i) {
            source_list.push_back(argv[i]);
        }
    }
    for (int i = 2; i < argc; ++i) {
        source_list.push_back(argv[i]);
    }

    switch (argv[1][1]) {
        case 'h':
            Usage();
            break;
        case 'l':
            Tokenize();
            break;
        case 'p':
            Parse();
            break;
        default:
            break;
    }

    return 0;
}
