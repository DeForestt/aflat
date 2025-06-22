#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

#include "Configs.hpp"
#include "catch.hpp"

bool build(std::string path, std::string output, cfg::Mutability mutability,
           bool debug);

TEST_CASE("build logs compile states", "[build]") {
  namespace fs = std::filesystem;
  fs::create_directories("tmp");
  std::ofstream ofs("tmp/simple.af");
  ofs << "int main(){return 0;};";
  ofs.close();

  std::stringstream buffer;
  auto *oldBuf = std::cout.rdbuf(buffer.rdbuf());

  bool result = build("tmp/simple.af", "tmp/simple.s", cfg::Mutability::Strict,
                       false);
  std::cout.flush();
  std::cout.rdbuf(oldBuf);

  fs::remove("tmp/simple.af");
  if (fs::exists("tmp/simple.s")) fs::remove("tmp/simple.s");
  fs::remove("tmp");

  REQUIRE(result);
  std::string out = buffer.str();
  REQUIRE(out.find("[parsing] tmp/simple.af") != std::string::npos);
  REQUIRE(out.find("[generating] tmp/simple.af") != std::string::npos);
  REQUIRE(out.find("[done] tmp/simple.af") != std::string::npos);
}
