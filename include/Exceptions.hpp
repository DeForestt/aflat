#if !defined(ERR)
#define ERR

#include <string>

namespace err {
class Exception {
public:
  std::string errorMsg;
  Exception(std::string msg);
};
} // namespace err

#endif // ERR