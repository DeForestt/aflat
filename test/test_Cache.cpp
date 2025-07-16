#include <chrono>
#include <filesystem>
#include <fstream>
#include <thread>

#include "catch.hpp"

bool compileCFile(const std::string &path, bool debug);

TEST_CASE("compileCFile cache reuse", "[cache]") {
  namespace fs = std::filesystem;
  fs::create_directories("src/tmp");
  std::ofstream ofs("src/tmp/foo.c");
  ofs << "int foo(){return 42;}";
  ofs.close();
  fs::remove_all(".cache/tmp");

  REQUIRE(compileCFile("tmp/foo", false));
  fs::path obj(".cache/tmp/foo.o");
  REQUIRE(fs::exists(obj));
  auto t1 = fs::last_write_time(obj);
  std::this_thread::sleep_for(std::chrono::seconds(1));
  REQUIRE(compileCFile("tmp/foo", false));
  auto t2 = fs::last_write_time(obj);
  REQUIRE(t1 == t2);

  fs::remove_all("src/tmp");
  fs::remove_all(".cache/tmp");
}
