//
// Created by leo on 2022/4/4.
//

#ifndef LEOML_PARSER_H
#define LEOML_PARSER_H

#include <cstddef>  // for std::size_t
#include <cstdint>
#include <map>
#include <optional>
#include <utility>
#include <vector>

#include "error.hpp"
#include "Lexer.h"
#include "ParseTree.h"

namespace leoml {
    class Parser final {
    private:
        using u64 = std::uint64_t;
        using i64 = std::int64_t;
        Lexer _lexer;
    public:
        Parser(Lexer lexer) : _lexer(lexer.GetInstance()) {}

        ParseTree &Parse() {}
    };

}

#endif //LEOML_PARSER_H
