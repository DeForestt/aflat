#ifndef PrePro
#define PrePro

#include <string>
#include <vector>

#include "LinkedListS.hpp"

/*A class to PreProcess The input Code*/

/* A class to store a Definition */
class Definition {
 public:
  std::string name;
  std::string value;

  // Compair a definition and a string
  static bool compair(Definition def, std::string str);
};

class PreProcessor {
 public:
  PreProcessor();
  ~PreProcessor();
  std::string PreProcess(std::string code, std::string exePath,
                         const std::string &currDir = "");
  bool debug = false;

  const std::vector<std::string> &getIncludes() const;

 private:
  std::string root;
  links::SLinkedList<Definition, std::string> definitions;
  std::vector<std::string> includes;

  /* incluse an external file */
  std::string Include(std::string line, std::string exePath);

  /*Define a value*/
  void Define(std::string line);

  /*Replaced defined value with value*/
  void ReplaceDefined(std::string &code);
};

#endif