#pragma once

#include "Parser/AST.hpp"


namespace parse
{
    namespace lower
    {
        class Lowerer {
            public:
            Lowerer(ast::Statment *root);

            private:
            ast::Statment * root;
            ast::Statment * curr;

            ast::Statment * lower(ast::Statment *stmt);
            ast::Statment * lowerFunction(ast::Function * func);
            ast::Function * findFunction(ast::Statment * stmt);
        };
    } // namespace lower
}
