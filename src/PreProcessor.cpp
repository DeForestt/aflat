#include "PreProcessor.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <algorithm>

// helper functons

std::string slice(std::string &str) {
    int index = str.find("//");
    if (index != std::string::npos) {
        str = str.substr(0, index);
    }
    return str;
}

std::string trim( std::string str )
{
    // remove trailing white space
    while( !str.empty() && std::isspace( str.back() ) ) str.pop_back() ;

    // return residue after leading white space
    std::size_t pos = 0 ;
    while( pos < str.size() && std::isspace( str[pos] ) ) ++pos ;
    return str.substr(pos) ;
}


// Definition
bool Definition::compair(Definition def, std::string str){
    return def.name == str;
};

// PreProcessor

PreProcessor::PreProcessor(){
    std::filesystem::path cwd = std::filesystem::current_path();
    this->root = cwd.string();
    this->definitions.foo = Definition::compair;
}

PreProcessor::~PreProcessor(){
    //dtor
}

std::string PreProcessor::PreProcess(std::string code){
    std::string output;
    
    // loop through each line of code
    std::stringstream input_stringstream(code);
    std::string line;

    while (getline(input_stringstream, line, '\n'))
    {
        line = trim(line);
        //Removes comments
        slice(line);
        
        this->ReplaceDefined(line);

        if(line.substr(0, 6) == ".needs"){
            //include a file
            output += this->Include(line);
        }else if(line.substr(0, 5) == ".root"){
            //changes the root directory
            int startPos = line.find_first_of('\"') + 1;
            int endPos = line.find_last_of('\"');
            root = line.substr(startPos, endPos - startPos);
        }else if(line.substr(0, 4) == ".def"){
            //create a definition
            this->Define(line);
        }
        else{
            //do not change the line
            output += line;
        };
    }

    return output;
}

/* includes an external file */
std::string PreProcessor::Include(std::string line){
    std::string output;
    int startPos = line.find_first_of('\"') + 1;
    int endPos = line.find_last_of('\"');
    std::string relpath = line.substr(startPos, endPos - startPos);
    std::string path = this->root + relpath;
    std::fstream f(path, std::fstream::in);
    std::string content;
    std::getline( f, content, '\0');
    output += content;
    return this->PreProcess(output);
};

/*Replaced defined value with value*/
void PreProcessor::ReplaceDefined(std::string &code){
    //loop through line by spaces
    std::string output;
    std::stringstream input_stringstream(code);
    std::string word;
    while (getline(input_stringstream, word, ' ')){
        //check if word is defined
        if(this->definitions[word] != nullptr){
            //replace word with defined value
            code = code.replace(code.find(word), word.length(), this->definitions[word]->value);
        }
    }

};

/*Define a value*/
void PreProcessor::Define(std::string line){
    //remove the .def
    line = line.substr(4);

    //split line into two strings with the =
    int equalsPos = line.find_first_of('=');
    std::string name = trim(line.substr(0, equalsPos));
    std::string value = trim(line.substr(equalsPos + 1));

    //Store in a Deffinition struct
    Definition def;
    def.name = name;
    def.value = value;

    //Add to the list of definitions
    this->definitions << def;
    return;
};