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
  explicit Scanner(std::istream &input, token::SourceId source_id = 1);

  ~Scanner();
  Scanner(Scanner &&) noexcept = default;
  Scanner &operator=(Scanner &&) noexcept = default;
  Scanner(const Scanner &) = delete;
  Scanner &operator=(const Scanner &) = delete;

  // Scanning operations
  outcome::result<token::Token, std::error_code> next();

private:
  struct Impl;
  Impl *impl_;
};

} // namespace aflat::scan
