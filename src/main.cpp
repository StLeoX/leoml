#include <iostream>
#include <list>

#include "syntax/Token.h"
#include "syntax/Lexer.h"
#include "syntax/Parser.h"
#include "syntax/ParseTree.h"
#include "syntax/Scope.h"
#include "syntax/Type.h"

static std::string source_path = "";
static std::string output_dir = "";  // "." for example
static std::list<std::string> source_list{};
static Program *TheProgram;

void Usage() {
    printf("Usage: leoml [-o <output>] [options] <source>\n"
           "Options: \n"
           "\t-h      Print this help message.\n"
           "\t-l      Tokenize the source.\n"
           "\t-p      Parse the source.\n"
           "\t-e      Evaluate the source.\n"
           "\t-i      Interactive mode, not support yet.\n"
           "\t-o      Specify output directory. Otherwise print to the stdout.\n");
    exit(0);
}

/// Get File Name without Suffix
/// \param filepath
/// \return filename, no suffix!
std::string GetName(const std::string &path) {
    auto left = path.rfind('/');
    std::string name;
    if (left == std::string::npos)
        name = path;
    name = path.substr(left + 1);
    auto right = name.find('.');
    std::string name_no_suffix;
    if (right == std::string::npos)
        name_no_suffix = name;
    name_no_suffix = name.substr(0, right);
    return name_no_suffix;
}

/// Laod File Content
/// \param filepath
/// \return content
std::string *LoadFile(const std::string &filepath) {
    FILE *f = fopen(filepath.c_str(), "r");
//    if (!f) CompileError("%s: No such file or directory", filepath.c_str());
    auto text = new std::string;
    int c;
    while (EOF != (c = fgetc(f)))
        text->push_back(c);
    fclose(f);
    return text;
}

// Tokenize entry
void Tokenize() {
    printf("tokenizing\n");
    for (auto source:source_list) {
        auto name = GetName(source);
        TokenSequence *ts = new TokenSequence();
        Lexer *lexer = Lexer::New(LoadFile(source), &name);
        lexer->Tokenize(*ts);
        if (output_dir != "") {
            std::string outpath = output_dir + "/" + name + ".ts.txt";
            std::ofstream out{outpath};
            ts->Serialize(out);
        } else {
            ts->Serialize(std::cout);
        }
        delete ts, lexer;
    }
}

// Parse entry
void Parse() {
    printf("parsing\n");
    for (auto source:source_list) {
        auto name = GetName(source);
        TokenSequence *ts = new TokenSequence();
        Lexer *lexer = Lexer::New(LoadFile(source), &name);
        lexer->Tokenize(*ts);
        Parser *parser = Parser::New(*ts);
        parser->Parse();
        if (output_dir != "") {
            std::string outpath = output_dir + "/" + name + ".ts.txt";
            std::ofstream out{outpath};
            parser->Serialize(out);
        } else {
            TheProgram = parser->GetProgram();
            parser->Serialize(std::cout);
        }
        delete ts, parser;
    }
}

void Eval(Program &program) {
    std::cout << "Unsupported yet." << std::endl;
}

void Repl() {
    std::cout << "Unsupported yet." << std::endl;
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
        case 'e':
            Parse();
            Eval(*TheProgram);
            break;
        case 'i':
            Repl();
            break;
        default:
            break;
    }

    return 0;
}
