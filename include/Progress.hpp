#ifndef AFLAT_PROGRESS_HPP
#define AFLAT_PROGRESS_HPP

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

class CompileProgress {
 public:
  explicit CompileProgress(const std::vector<std::string>& sources,
                           bool quiet = false);
  void update(const std::string& src, const std::string& state);

 private:
  std::vector<std::string> sources_;
  std::unordered_map<std::string, size_t> index_;
  bool quiet_ = false;
};

#endif  // AFLAT_PROGRESS_HPP
