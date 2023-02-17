#include "PreProcessor.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <numeric>

// helper functons

std::string slice(std::string &str) {
  auto index = str.find("//");
  if (index != std::string::npos) {
    str = str.substr(0, index);
  }
  return str;
}

std::string trim(std::string str) {
  // remove trailing white space
  while (!str.empty() && std::isspace(str.back()))
    str.pop_back();

  // return residue after leading white space
  std::size_t pos = 0;
  while (pos < str.size() && std::isspace(str[pos]))
    ++pos;
  return str.substr(pos);
}

// Definition
bool Definition::compair(Definition def, std::string str) {
  return def.name == str;
};

// PreProcessor

PreProcessor::PreProcessor() {
  auto cwd = std::filesystem::current_path();
  this->root = cwd.string();
  this->definitions.foo = Definition::compair;
}

PreProcessor::~PreProcessor() {
  // dtor
}

std::string PreProcessor::PreProcess(std::string code, std::string libPath) {
  std::string output;

  // loop through each line of code
  std::stringstream input_stringstream(code);
  std::string line;

  while (getline(input_stringstream, line, '\n')) {
    // Ignore multiline comments
    if (line.find("/*") != std::string::npos) {
      while (line.find("*/") == std::string::npos) {
        getline(input_stringstream, line, '\n');
        output += '\n';
      }
      line = line.substr(line.find("*/") + 2);
    }

    line = trim(line);

    // Removes comments
    slice(line);

    this->ReplaceDefined(line);

    if (line.substr(0, 6) == ".needs") {
      // include a file
      output += this->Include(line, libPath);
    } else if (line.substr(0, 5) == ".root") {
      // changes the root directory
      auto startPos = line.find_first_of('\"') + 1;
      auto endPos = line.find_last_of('\"');
      root = line.substr(startPos, endPos - startPos);
    } else if (line.substr(0, 4) == ".def") {
      // create a definition
      this->Define(line);
    } else {
      line = line + '\n';
      output += line;
    };
  }

  // write the output to a file
  if (this->debug) {
    std::ofstream outfile;
    outfile.open("output.af");
    outfile << output;
    outfile.close();
  }
  return output;
}

/* includes an external file */
std::string PreProcessor::Include(std::string line, std::string libPath) {
  std::string output;
  std::string path;
  // check if the line has quotes
  if (line.find("\"") != std::string::npos) {
    // get the file name
    auto startPos = line.find_first_of('\"') + 1;
    auto endPos = line.find_last_of('\"');
    auto relpath = line.substr(startPos, endPos - startPos);
    // check if relative path starts with a '/'
    if (relpath[0] != '/') {
      relpath = "/" + relpath;
    }
    if (relpath.find(".gs") == std::string::npos) {
      relpath += ".gs";
    }
    path = this->root + relpath;
  } else if (line.find("<") != std::string::npos) {
    // get the file name
    auto startPos = line.find_first_of('<') + 1;
    auto endPos = line.find_last_of('>');
    auto relpath = line.substr(startPos, endPos - startPos);
    // check if the file ends with .gs
    if (relpath.find(".gs") == std::string::npos) {
      relpath += ".gs";
    }
    path = libPath + relpath;
  }
  // check if the file exists
  if (std::filesystem::exists(path)) {
    std::fstream f(path, std::fstream::in);
    std::string content;
    std::getline(f, content, '\0');
    output += content;
    // remove all new lines from output
    std::string cleanPut;
    if (std::find(this->includes.begin(), this->includes.end(), path) ==
        this->includes.end()) {
      this->includes.push_back(path);
      if (this->debug)
        return this->PreProcess(output, libPath);
      output = this->PreProcess(output, libPath);
      output.reserve(output.size());
      cleanPut = std::accumulate(output.begin(), output.end(), std::string(),
                                 [](std::string &a, char b) {
                                   if (b != '\n')
                                      a += b;
                                    return a;
                                  });
      return this->PreProcess(cleanPut, libPath);
    }
    return "";
  } else {
    throw err::Exception("File name: " + path + "does not exist");
  }
};

/*Replaced defined value with value*/
void PreProcessor::ReplaceDefined(std::string &code) {
  // loop through line by spaces
  std::stringstream input_stringstream(code);
  std::string word;
  while (getline(input_stringstream, word, ' ')) {
    // check if word is defined
    if (this->definitions[word] != nullptr) {
      // replace word with defined value
      code = code.replace(code.find(word), word.length(),
                          this->definitions[word]->value);
    }
  }
};

/*Define a value*/
void PreProcessor::Define(std::string line) {
  // remove the .def
  line = line.substr(4);

  // split line into two strings with the =
  auto equalsPos = line.find_first_of('=');
  auto name = trim(line.substr(0, equalsPos));
  auto value = trim(line.substr(equalsPos + 1));

  // Store in a Deffinition struct
  Definition def;
  def.name = name;
  def.value = value;

  // Add to the list of definitions
  this->definitions << def;
  return;
};