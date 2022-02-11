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

                // Assign a new symbol and return the byteMod
                int assign(std::string symbol, int offset, bool mask, bool isGlobal = false);

                // Get a symbol
                Symbol get(std::string symbol);

                // push a new scope
                void pushScope();

                // pop a scope
                void popScope();

                // Get stack alignment value
                int getStackAlignment();

            private:
                ScopeManager();
                ~ScopeManager() = default;
                static ScopeManager* instance;

                // Stack
                std::vector<gen::Symbol> stack;

                // Global Scope
                std::vector<gen::Symbol> globalStack;

                // hold the current memory location relative to the rbp
                int stackPos;

                // scopeStack holds the number of symbols in the current scope
                std::vector<int> scopeStack;
        };

    }

}