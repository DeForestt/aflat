#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "Configs.hpp"
#include "catch.hpp"

bool build(std::string path, std::string output, cfg::Mutability mutability,
           bool debug);

TEST_CASE("build logs compile states", "[build]") {
  namespace fs = std::filesystem;
  const auto dir = fs::path("tmp/build_logging");
  fs::remove_all(dir);
  fs::create_directories(dir);
  const auto source = dir / "simple.af";
  const auto output = dir / "simple.s";
  std::ofstream ofs(source);
  ofs << "int main(){return 0;};";
  ofs.close();

  std::stringstream buffer;
  auto *oldBuf = std::cout.rdbuf(buffer.rdbuf());

  bool result =
      build(source.string(), output.string(), cfg::Mutability::Strict, false);
  std::cout.flush();
  std::cout.rdbuf(oldBuf);

  fs::remove_all(dir);

  REQUIRE(result);
  std::string out = buffer.str();
  REQUIRE(out.find("[parsing] " + source.string()) != std::string::npos);
  REQUIRE(out.find("[generating] " + source.string()) != std::string::npos);
  REQUIRE(out.find("[done] " + source.string()) != std::string::npos);
}
