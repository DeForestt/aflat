#pragma once
#include "CodeGenerator/CodeGenerator.hpp"
#include "ASM.hpp"
#include <vector>

namespace gen{

    namespace scope{

        class ScopeManager{
            public:
                // Singleton Access
                static ScopeManager& getInstance(){
                    if (!instance) instance = new ScopeManager;
                    return *instance;
                };

                // cannot be copied
                ScopeManager(ScopeManager const&) = delete;

                // cannot be assigned
                void operator=(ScopeManager const&) = delete;

                // Assign a new symbol
                void assign(std::string symbol, AST::Type type, bool mask, int byteMod);

                // Get a symbol
                Symbol* get(std::string symbol);

                // push a new scope
                void pushScope();

                // pop a scope
                void popScope();

            private:
                ScopeManager() = default;
                ~ScopeManager() = default;
                static ScopeManager* instance;

                // Stack
                std::vector<gen::Symbol> stack;

                // scopeStack holds the number of symbols in the current scope
                std::vector<int> scopeStack;

        };

    }

}