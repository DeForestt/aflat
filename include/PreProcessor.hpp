#ifndef PrePro
#define PrePro

#include <string>
#include "LinkedListS.hpp"

/*A class to PreProcess The input Code*/

/* A class to store a Definition */
class Definition
{
    public:
    std::string name;
    std::string value;

    // Compair a definition and a string
    static bool compair(Definition def, std::string str);
};

class PreProcessor{
    public:
        PreProcessor();
        ~PreProcessor();
        std::string PreProcess(std::string code);
    private:
        std::string root;
        links::SLinkedList<Definition, std::string> definitions;

        /* incluse an external file */
        std::string Include(std::string line);

        /*Define a value*/
        void Define(std::string line);

        /*Replaced defined value with value*/
        void ReplaceDefined(std::string &code);

                
};

#endif