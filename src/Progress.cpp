#include "Progress.hpp"

CompileProgress::CompileProgress(const std::vector<std::string>& sources,
                                 bool quiet) {
  sources_ = sources;
  quiet_ = quiet;
  if (!quiet_) {
    for (size_t i = 0; i < sources.size(); ++i) {
      index_[sources[i]] = i;
      std::cout << "[waiting] " << sources[i] << std::endl;
    }
    std::cout.flush();
  } else {
    for (size_t i = 0; i < sources.size(); ++i) {
      index_[sources[i]] = i;
    }
  }
}

void CompileProgress::update(const std::string& src, const std::string& state) {
  if (quiet_) return;
  auto it = index_.find(src);
  if (it == index_.end()) return;
  size_t line = it->second;
  size_t linesUp = sources_.size() - line;
  std::cout << "\033[" << linesUp << "A";    // move up
  std::cout << "\r\033[K";                   // clear line
  std::cout << "[" << state << "] " << src;  // print new state
  std::cout << "\033[" << linesUp << "B";    // move back down
  std::cout.flush();
}
