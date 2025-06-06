#if !defined(ERR)
#define ERR

#include <string>

namespace lex {
class Token;
}

namespace err {
class Exception {
 public:
  std::string errorMsg;
  explicit Exception(std::string msg);
  Exception(std::string msg, int line, int columnStart, int columnEnd);
  Exception(std::string msg, lex::Token *tok);
};

std::string formatLoc(int line, int columnStart, int columnEnd);
}  // namespace err

#endif  // ERR