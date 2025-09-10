#pragma once
#include <boost/outcome/result.hpp>
#include <memory>
#include <system_error>

#include "Scan/Token.hpp"

namespace outcome = BOOST_OUTCOME_V2_NAMESPACE;

namespace aflat::scan {

class Scanner {
public:
  Scanner() = delete;

  // Factory that does I/O and reports errors without exceptions
  static outcome::result<Scanner, std::error_code> open(const char *path);

  // RAII: move-only
  ~Scanner() = default;
  Scanner(Scanner &&) noexcept = default;
  Scanner &operator=(Scanner &&) noexcept = default;
  Scanner(const Scanner &) = delete;
  Scanner &operator=(const Scanner &) = delete;

  // Scanning operations
  outcome::result<token::Token, std::error_code> next();

private:
  struct Impl;
  std::unique_ptr<Impl> impl_;

  explicit Scanner(std::unique_ptr<Impl> p) noexcept : impl_(std::move(p)) {}
};

} // namespace aflat::scan
